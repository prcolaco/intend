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
 * Intend C Structures of values
 */

#include <stdlib.h>

#include "runtime.h"

/*
 * Set struct field
 */
void value_set_struct(value *st, const char *pos, value *val)
{
    symtab *sym;

    sanity(st && pos && val);
    sym = st->value_u.struct_val;
    sanity(sym);

    if (val->type != VALUE_TYPE_FN) {
        symtab_add_variable(sym, pos, val);
    } else {
        symtab_add_function(sym, pos, FNSIG_OF(val));
    }
}

/*
 * Get struct field
 */
value *value_get_struct(value *st, const char *pos)
{
    symtab *sym;
    symtab_entry *entry;

    sanity(st && pos);
    sym = st->value_u.struct_val;
    sanity(sym);

    entry = symtab_lookup(sym, pos);
    if (!entry || entry->type == SYMTAB_ENTRY_CLASS) {
        return value_make_void();
    }
    if (entry->type == SYMTAB_ENTRY_VAR) {
        return value_copy(&entry->entry_u.var);
    } else {
        return value_make_fn(&(entry->entry_u.fnc.sigs[0]));
    }
}

/*
 * Delete struct field
 */
void value_delete_struct(value *st, const char *pos)
{
    symtab *sym;

    sanity(st && pos);
    sym = st->value_u.struct_val;
    sanity(sym);

    symtab_delete(sym, pos);
}

/*
 * Set a struct method field
 */
void value_set_struct_method(value *st, const char *method, call_func vector, unsigned int args,
                       char *proto, char rettype)
{
    signature sig;

    sanity(st && method && vector && proto);

    sig.type = FUNCTION_TYPE_BUILTIN;
    sig.args  = args;
    sig.name  = (char *)method;
    sig.proto = proto;
    sig.call_u.builtin_vector = vector;
    sig.rettype = rettype;
    symtab_add_function(st->value_u.struct_val, method, &sig);
}

/*
 * Enter struct namespace
 */
static void enter_struct(intend_state *s, value *val)
{
    symtab_stack_enter(s);
    symtab_stack_add_variable(s, "this", val);
}

/*
 * Leave struct namespace
 */
static value *leave_struct(intend_state *s)
{
    symtab_entry *entry;
    value *res = NULL;

    entry = symtab_stack_lookup(s, "this");
    if (entry) {
        res = value_copy(&entry->entry_u.var);
    }
    symtab_stack_leave(s);
    return res;
}

/*
 * Call a struct method
 */
value *value_call_struct_method(intend_state *s, const char *name, const char *method,
                         unsigned int argc, value **argv)
{
    symtab_entry *entry;
    value *st = NULL;
    value *res, *temp;

    sanity(name && method && argv);

    // Lookup for struct inside current namespace
    entry = symtab_stack_lookup(s, name);
    if (entry && entry->type == SYMTAB_ENTRY_VAR) {
        st = &entry->entry_u.var;
    }
    if (!st || st->type != VALUE_TYPE_STRUCT) {
        fatal(s, "method call on non-struct value");
        return value_make_void();
    }

    // Lookup for method inside struct
    entry = symtab_lookup(st->value_u.struct_val, method);
    if (!entry || entry->type != SYMTAB_ENTRY_FUNCTION) {
        fatal(s, "call to undefined method `%s'", method);
        return value_make_void();
    }

    // Enter the struct namespace
    enter_struct(s, st);

    // Call the method
    res = call_function(s, &(entry->entry_u.fnc.sigs[0]), argc, argv);

    // Exit the struct namespace and get result struct
    temp = leave_struct(s);

    if (temp) {
        // Set struct to result struct
        symtab_stack_add_variable(s, name, st);
        // Free the result struct
        value_free(temp);
    }

    return res;
}

