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
 * Intend C Variable reference evaluation
 */

#include <stdio.h>
#include <stdlib.h>

#include "eval.h"

/*
 * Evaluate variable reference
 */
value *eval_ref(intend_state *s, expr *ex)
{
    symtab_entry *entry;

    sanity(ex && ex->name);

    entry = symtab_stack_lookup(s, ex->name);
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
 * Get element from numerical array
 */
static value *array_get(intend_state *s, value *arr, int argc, expr **index)
{
    value *pos, *val, *res;
    int realpos = 0, is_struct;
    char *realidx = NULL;

    is_struct = (index[0]->type == EXPR_FIELD);

    if (!arr || (!is_struct && arr->type != VALUE_TYPE_ARRAY) ||
            (is_struct && arr->type != VALUE_TYPE_STRUCT)) {
        return value_make_void();
    }

    pos = eval_expr(s, index[0]);
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

    if (is_struct) {
        val = value_get_struct(arr, realidx);
    } else {
        if (realidx) {
            val = value_get_key_array(arr, realidx);
        } else {
            val = value_get_array(arr, realpos);
        }
    }
    value_free(pos);

    if (argc == 1) {
        return val;
    } else {
        res = array_get(s, val, argc - 1, index + 1);
        value_free(val);
        return res;
    }
}

/*
 * Evaluate array variable reference
 */
value *eval_ref_array(intend_state *s, expr *ex)
{
    value *res = NULL;
    symtab_entry *entry;

    sanity(ex && ex->name);

    entry = symtab_stack_lookup(s, ex->name);
    if (!entry || entry->type != SYMTAB_ENTRY_VAR) {
        return value_make_void();
    }
    res = array_get(s, &entry->entry_u.var, ex->argc, ex->argv);
    return res;
}

/*
 * Evaluate index reference
 */
value *eval_ref_index(intend_state *s, expr *ex)
{
    value *val, *res;

    sanity(ex && ex->inner);

    val = eval_expr(s, ex->inner);
    res = array_get(s, val, ex->argc, ex->argv);

    value_free(val);
    return res;
}
