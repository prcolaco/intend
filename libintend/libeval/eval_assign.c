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
 * Assignment evaluation
 */

#include <stdlib.h>

#include "eval.h"

/*
 * Evaluate variable assignment
 */
value *eval_assign(intend_state *s, expr *ex)
{
    value *val;

    sanity(ex && ex->name);

    val = eval_expr(s, ex->inner);

    if (!s->except_flag && !s->exit_flag) {
        if (val->type != VALUE_TYPE_FN) {
            symtab_stack_add_variable(s, ex->name, val);
        } else {
            symtab_stack_add_function(s, ex->name, FNSIG_OF(val));
        }
    }
    return val;
}

/*
 * Set value in nested array
 */
value *array_set(intend_state *s, value *arr, int argc, expr **index,
                 value *val)
{
    value *pos, *elem;
    int realpos = 0, is_struct, is_inner_struct;
    char *realidx = NULL;

    sanity(arr && index && *index && val);

    is_struct = (index[0]->type == EXPR_FIELD);
    is_inner_struct = (argc > 1 && index[1]->type == EXPR_FIELD);

    pos = eval_expr(s, index[0]);

    if (s->except_flag || s->exit_flag) {
        value_free(pos);
        return value_make_void();
    }

    if (is_struct) {
        realidx = STR_OF(pos);
    } else {
        if (TYPE_OF(pos) == VALUE_TYPE_STRING) {
            realidx = STR_OF(pos);
        } else {
            value_cast_inplace(s, &pos, VALUE_TYPE_INT);
            realpos = INT_OF(pos);
        }
    }

    if (argc > 1) {
        if (is_struct) {
            elem = value_get_struct(arr, realidx);
        } else {
            if (realidx) {
                elem = value_get_key_array(arr, realidx);
            } else {
                elem = value_get_array(arr, realpos);
            }
        }
        if (is_inner_struct) {
            if (elem->type != VALUE_TYPE_STRUCT) {
                value_free(elem);
                elem = value_make_struct();
            }
        } else {
            if (elem->type != VALUE_TYPE_ARRAY) {
                value_free(elem);
                elem = value_make_array();
            }
        }
        elem = array_set(s, elem, argc - 1, index + 1, val);
        if (is_struct) {
            value_set_struct(arr, realidx, elem);
        } else {
            if (realidx) {
                value_set_key_array(arr, realidx, elem);
            } else {
                value_set_array(arr, realpos, elem);
            }
        }
        value_free(elem);
        value_free(pos);
        return arr;
    } else {
        if (is_struct) {
            value_set_struct(arr, realidx, val);
        } else {
            if (realidx) {
                value_set_key_array(arr, realidx, val);
            } else {
                value_set_array(arr, realpos, val);
            }
        }
        value_free(pos);
        return arr;
    }
}

/*
 * Direct array assignment
 *
 * This function can be used by other evaluation functions to
 * make updates to array elements.
 */
void eval_assign_array_direct(intend_state *s, const char *name,
                              int argc, expr **index, value *val)
{
    symtab_entry *entry;
    value *arr = NULL;
    int is_struct;
    int is_new = 0;

    sanity(name && index && *index && val);

    is_struct = (index[0]->type == EXPR_FIELD);

    entry = symtab_stack_lookup(s, name);
    if (!entry || entry->type != SYMTAB_ENTRY_VAR) {
        if (is_struct) {
            arr = value_make_struct();
            is_new = 1;
        } else {
            arr = value_make_array();
            is_new = 1;
        }
    } else {
        if (is_struct) {
            if (entry->entry_u.var.type == VALUE_TYPE_STRUCT) {
                // Optimization for equal value assignments and large
                // data types, like strings, arrays and structs
                //arr = value_copy(entry->entry_u.var);
                arr = &entry->entry_u.var;
            } else {
                arr = value_make_struct();
                is_new = 1;
            }
        } else {
            if (entry->entry_u.var.type == VALUE_TYPE_ARRAY) {
                // Optimization for equal value assignments and large
                // data types, like strings, arrays and structs
                //arr = value_copy(entry->entry_u.var);
                arr = &entry->entry_u.var;
            } else {
                arr = value_make_array();
                is_new = 1;
            }
        }
    }

    arr = array_set(s, arr, argc, index, val);
    if (!s->except_flag && !s->exit_flag) {
        symtab_stack_add_variable(s, name, arr);
    }
    // Optimization for equal value assignments and large
    // data types, like strings, arrays and structs
    if (is_new) value_free(arr);
}

/*
 * Evaluate array assignment
 */
value *eval_assign_array(intend_state *s, expr *ex)
{
    value *val;

    sanity(ex && ex->name && ex->argc > 0 && ex->argv[0]);

    val = eval_expr(s, ex->inner);
    if (!s->except_flag && !s->exit_flag) {
        eval_assign_array_direct(s, ex->name, ex->argc, ex->argv, val);
    }
    return val;
}
