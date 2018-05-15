/***************************************************************************
 *                                                                         *
 *   Intend C - Embeddable Scripting Language                              *
 *                                                                         *
 *   Copyright (C) 2008 by Pedro Reis Colaço <info@intendc.org>            *
 *   http://www.intendc.org                                                *
 *                                                                         *
 *   LICENSE INFORMATION:                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   ACKNOWLEDGEMENTS:                                                     *
 *                                                                         *
 *   This project was based on the work of Pascal Schmidt in project       *
 *   Arena. See http://www.minimalinux.org/arena/ for more information.    *
 *                                                                         *
 ***************************************************************************/

/*
 * Intend C Dumping of values
 */

#include <ctype.h>
#include <math.h>
#include <stdio.h>

#include "runtime.h"

/*
 * Print function prototype
 */
static void print_fn(intend_state *s, signature *sig)
{
    char *pos;

    fprintf(s->stdout, "%s%s fn(", isupper(sig->rettype) ? "forced " : "",
            sig->rettype != '?' ?
            call_typename(call_chartype(tolower(sig->rettype))) : "mixed");

    pos = sig->proto;
    while (pos && *pos != 0) {
        fprintf(s->stdout, "%s%s%s", isupper(*pos) ? "forced " : "",
                *pos != '?' ? call_typename(call_chartype(tolower(*pos))) : "mixed",
                *(pos + 1) != 0 ? ", " : "");
        ++pos;
    }
    fprintf(s->stdout, ")");
}

/*
 * Print escaped string
 */
static void escape_str(intend_state *s, const value *val)
{
    const char *hex = "0123456789abcdef";
    char *str = STR_OF(val);
    int len   = STRLEN_OF(val);
    int i;

    fprintf(s->stdout, "\"");
    if (len > 0) {
        for (i = 0; i < len; i++) {
            if (isprint(str[i])) {
                fprintf(s->stdout, "%c", str[i]);
            } else {
                switch (str[i]) {
                    case 0:
                        fprintf(s->stdout, "\\0");
                        break;
                    case '\b':
                        fprintf(s->stdout, "\\b");
                        break;
                    case 27:
                        fprintf(s->stdout, "\\e");
                        break;
                    case '\f':
                        fprintf(s->stdout, "\\f");
                        break;
                    case '\n':
                        fprintf(s->stdout, "\\n");
                        break;
                    case '\r':
                        fprintf(s->stdout, "\\r");
                        break;
                    case '\t':
                        fprintf(s->stdout, "\\t");
                        break;
                    default:
                        fprintf(s->stdout, "\\x%c%c", hex[str[i] >> 4], hex[str[i] & 15]);
                }
            }
        }
    }
    fprintf(s->stdout, "\"");
}

/*
 * Print indentation prefix
 *
 * Prints two spaces for each level of depth so that nested
 * arrays are printed with increasing indentation per nesting
 * level.
 */
static void depth_prefix(intend_state *s, int depth)
{
    int i;
    for (i = 0; i < depth; i++) {
        fprintf(s->stdout, " ");
    }
}

/*
 * Print value contents
 *
 * This function prints out a dump of the given value. Arrays
 * are dumped recursively so that all elements and nested arrays
 * are printed out in full.
 */
void value_dump(intend_state *s, const value *val, int depth, int skip_flag)
{
    symtab *sym;
    symtab_node *node;
    symtab_entry *entry;
    unsigned int si, sj;
    value *next;
    int i, len;

    if (!val) {
        return;
    }

    if (!skip_flag) {
        depth_prefix(s, depth);
    }
    skip_flag = 0;
    if (!val) {
        fprintf(s->stdout, "void");
    } else {
        switch (val->type) {
            case VALUE_TYPE_VOID:
                fprintf(s->stdout, "void");
                break;
            case VALUE_TYPE_BOOL:
                fprintf(s->stdout, "bool: ");
                if (BOOL_OF(val)) {
                    fprintf(s->stdout, "true");
                } else {
                    fprintf(s->stdout, "false");
                }
                break;
            case VALUE_TYPE_INT:
                fprintf(s->stdout, "int: %i", INT_OF(val));
                break;
            case VALUE_TYPE_FLOAT:
                fprintf(s->stdout, "float: %0.10g", FLOAT_OF(val));
                break;
            case VALUE_TYPE_STRING:
                fprintf(s->stdout, "string[%i]: ", STRLEN_OF(val));
                escape_str(s, val);
                break;
            case VALUE_TYPE_ARRAY:
                fprintf(s->stdout, "array(%i): {\n", ARRLEN_OF(val));
                depth += 2;
                for (i = 0; i < ARRLEN_OF(val); i++) {
                    depth_prefix(s, depth);
                    next = (value *) &(ARR_OF(val)[i]->entry_u.var);
                    if (ARR_OF(val)[i]->symbol) {
                        len = fprintf(s->stdout, "[\"%s\"] ", ARR_OF(val)[i]->symbol);
                    } else {
                        len = fprintf(s->stdout, "[%i] ", i);
                    }
                    skip_flag = 1;
                    depth += len;
                    value_dump(s, next, depth, skip_flag);
                    depth -= len;
                }
                depth -= 2;
                depth_prefix(s, depth);
                fprintf(s->stdout, "}");
                break;
            case VALUE_TYPE_STRUCT:
                fprintf(s->stdout, "struct(%i): {\n", symtab_num_entries(val->value_u.struct_val));
                depth += 2;
                sym = val->value_u.struct_val;
                for (si = 0; si < (unsigned int)(1 << sym->order); si++) {
                    node = sym->nodes[si];
                    if (node) {
                        for (sj = 0; sj < node->len; sj++) {
                            entry = &node->entries[sj];
                            if (entry && entry->type == SYMTAB_ENTRY_VAR) {
                                depth_prefix(s, depth);
                                len = fprintf(s->stdout, ".%s = ", entry->symbol);
                                depth += len;
                                skip_flag = 1;
                                value_dump(s, &entry->entry_u.var, depth, skip_flag);
                                depth -= len;
                            }
                            if (entry && entry->type == SYMTAB_ENTRY_FUNCTION) {
                                depth_prefix(s, depth);
                                fprintf(s->stdout, ".%s = ", entry->symbol);
                                print_fn(s, &(entry->entry_u.fnc.sigs[0]));
                                fprintf(s->stdout, "\n");
                            }
                        }
                    }
                }
                depth -= 2;
                depth_prefix(s, depth);
                fprintf(s->stdout, "}");
                break;
            case VALUE_TYPE_FN:
                fprintf(s->stdout, "fn: ");
                print_fn(s, FNSIG_OF(val));
                break;
            case VALUE_TYPE_RES:
                fprintf(s->stdout, "resource: 0x%08x[#%i]", (int) RES_OF(val),
                        RESREF_OF(val));
                break;
            default:
                fprintf(s->stdout, "unknown");
        }
    }
    fprintf(s->stdout, "\n");
}
