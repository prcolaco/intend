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
 * Intend C Struct functions
 */

#include <stdlib.h>

#include "stdlib.h"

/*
 * Struct functions to register
 */
static register_func_data struct_funcs[] = {
    { "mkstruct",		struct_mkstruct,	0,	"",		'c'	},
    { "struct_get",		struct_get,			2,	"cs",	'?'	},
    { "struct_set",		struct_set,			3,	"cs?",	'c'	},
    { "struct_unset",	struct_unset,		2,	"cs",	'c'	},
    { "struct_fields",	struct_fields,		1,	"c",	'a'	},
    { "struct_methods",	struct_methods,		1,	"c",	'a'	},
    { "is_field",		struct_is_field,	2,	"cs",	'b'	},
    { "is_method",		struct_is_method,	2,	"cs",	'b'	},
    { "struct_merge",	struct_merge,		0,	"",		'c'	},

    /* list terminator */
    { NULL,				NULL,				0,	NULL,	0	}
};

/*
 * Initialize the Struct functions
 */
void struct_init(intend_state *s)
{
    // Register functions
    register_function_array(s, struct_funcs);
}

/*
 * Create struct from parameter value pairs
 */
value *struct_mkstruct(intend_state *s, unsigned int argc, value **argv)
{
    value *st, *elem;
    unsigned int i = 0;

    st = value_make_struct();
    while (i < argc) {
        value_cast_inplace(s, &argv[i], VALUE_TYPE_STRING);
        if (STRLEN_OF(argv[i]) > 0) {
            if (i + 1 < argc) {
                value_set_struct(st, STR_OF(argv[i]), argv[i+1]);
            } else {
                elem = value_make_void();
                value_set_struct(st, STR_OF(argv[i]), elem);
                value_free(elem);
            }
        }
        i += 2;
    }
    return st;
}

/*
 * Delete value from struct
 *
 * This function the field with the given name from the struct
 * and returns the resulting struct.
 */
value *struct_unset(intend_state *s, unsigned int argc, value **argv)
{
    char *field = STR_OF(argv[1]);

    if (field) {
        value_delete_struct(argv[0], field);
    }
    return value_copy(argv[0]);
}

/*
 * Get field from struct
 */
value *struct_get(intend_state *s, unsigned int argc, value **argv)
{
    char *field = STR_OF(argv[1]);

    if (!field) {
        return value_make_void();
    }
    return value_get_struct(argv[0], field);
}

/*
 * Set field in struct
 */
value *struct_set(intend_state *s, unsigned int argc, value **argv)
{
    char *field = STR_OF(argv[1]);

    if (field) {
        value_set_struct(argv[0], field, argv[2]);
    }
    return value_copy(argv[0]);
}

/*
 * Get names of structure elements of some type
 */
static value *getelements(value *val, unsigned int wanted)
{
    symtab *sym = val->value_u.struct_val;
    symtab_node *node;
    symtab_entry *entry;
    value *name, *arr;
    unsigned int i, j;

    arr = value_make_array();

    for (i = 0; i < (unsigned int)(1 << sym->order); i++) {
        node = sym->nodes[i];
        if (node) {
            for (j = 0; j < node->len; j++) {
                entry = &node->entries[j];
                if (entry && entry->type == wanted) {
                    name = value_make_string(entry->symbol);
                    value_add_to_array(arr, name);
                    value_free(name);
                }
            }
        }
    }
    return arr;
}

/*
 * Get structure fields
 *
 * Returns an array of strings containing the field names of the
 * given structure.
 */
value *struct_fields(intend_state *s, unsigned int argc, value **argv)
{
    return getelements(argv[0], SYMTAB_ENTRY_VAR);
}

/*
 * Get structure methods
 *
 * Returns an array of strings containing the method names of the
 * given structure.
 */
value *struct_methods(intend_state *s, unsigned int argc, value **argv)
{
    return getelements(argv[0], SYMTAB_ENTRY_FUNCTION);
}

/*
 * Check for element with specific type
 */
static value *checkelement(value *hay, value *needle, unsigned int wanted)
{
    symtab *sym = hay->value_u.struct_val;
    char *field = STR_OF(needle);
    symtab_entry *entry;

    if (!field) {
        return value_make_bool(0);
    }
    entry = symtab_lookup(sym, field);
    return value_make_bool(entry && entry->type == wanted);
}

/*
 * Check field existence
 *
 * Returns true if the given field name exists in the given struct.
 */
value *struct_is_field(intend_state *s, unsigned int argc, value **argv)
{
    return checkelement(argv[0], argv[1], SYMTAB_ENTRY_VAR);
}

/*
 * Check method existence
 *
 * Returns true if the given method name exists in the given struct.
 */
value *struct_is_method(intend_state *s, unsigned int argc, value **argv)
{
    return checkelement(argv[0], argv[1], SYMTAB_ENTRY_FUNCTION);
}

/*
 * Merge multiple structs
 *
 * Returns a structure with all fields from the input structures. If
 * a field occurs multiple times, the value from last structure wins.
 */
value *struct_merge(intend_state *s, unsigned int argc, value **argv)
{
    symtab *sym;
    symtab_node *node;
    symtab_entry *entry;
    unsigned int i, si, sj;
    value *st;

    st = value_make_struct();

    for (i = 0; i < argc; i++) {
        value_cast_inplace(s, &argv[i], VALUE_TYPE_STRUCT);
        sym = argv[i]->value_u.struct_val;
        for (si = 0; si < (unsigned int)(1 << sym->order); si++) {
            node = sym->nodes[si];
            if (node) {
                for (sj = 0; sj < node->len; sj++) {
                    entry = &node->entries[sj];
                    if (entry && entry->type == SYMTAB_ENTRY_VAR) {
                        value_set_struct(st, entry->symbol, &entry->entry_u.var);
                    }
                    if (entry && entry->type == SYMTAB_ENTRY_FUNCTION) {
                        symtab_add_function(st->value_u.struct_val, entry->symbol,
                                            &(entry->entry_u.fnc.sigs[0]));
                    }
                }
            }
        }
    }
    return st;
}
