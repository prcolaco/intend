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
 * Intend C Array functions
 */

#include <stdio.h>
#include <stdlib.h>

#include "stdlib.h"

/*
 * Array functions to register
 */
static register_func_data array_funcs[] = {
    { "mkarray",        array_mkarray,      0,  "",     'a' },
    { "mkkeyarray",     array_mkkeyarray,   0,  "",     'a' },
    { "qsort",          array_sort,         1,  "a",    'a' },
    { "is_sorted",      array_is_sorted,    1,  "a",    'b' },
    { "array_unset",    array_unset,        2,  "ai",   'a' },
    { "array_compact",  array_compact,      1,  "a",    'a' },
    { "array_search",   array_search,       2,  "a?",   '?' },
    { "array_merge",    array_merge,        0,  "",     'a' },
    { "array_reverse",  array_reverse,      1,  "a",    'a' },

    /* list terminator */
    { NULL,             NULL,               0,  NULL,   0   }
};

/*
 * Initialize the Array functions
 */
void array_init(intend_state *s)
{
    // Register functions
    register_function_array(s, array_funcs);
}

/*
 * Comparison function for qsort
 */
static int compar(const void *a, const void *b)
{
    value *first = &(*(symtab_entry **)a)->entry_u.var;
    value *second = &(*(symtab_entry **)b)->entry_u.var;
    value *res, *cast = NULL;
    int order;

    if (first->type != second->type) {
        cast = value_cast(NULL, second, first->type);
        second = cast;
    }

    res = eval_order_seq(first, second);
    order = BOOL_OF(res);

    value_free(res);
    value_free(cast);

    if (order) {
        return -1;
    } else {
        return 1;
    }
}

/*
 * Create array from parameter values
 */
value *array_mkarray(intend_state *s, unsigned int argc, value **argv)
{
    value *arr;
    unsigned int i;

    arr = value_make_array();
    for (i = 0; i < argc; i++) {
        value_add_to_array(arr, argv[i]);
    }
    return arr;
}

/*
 * Create key array from parameter value pairs
 */
value *array_mkkeyarray(intend_state *s, unsigned int argc, value **argv)
{
    value *arr, *val;
    unsigned int i;

    arr = value_make_array();
    for (i = 0; i < argc; i++) {
        if (i + 1 < argc) {
            val = argv[i + 1];
        } else {
            val = value_make_void();
        }
        if (TYPE_OF(argv[i]) == VALUE_TYPE_INT) {
            value_set_array(arr, INT_OF(argv[i]), val);
        } else if (TYPE_OF(argv[i]) == VALUE_TYPE_VOID) {
            value_add_to_array(arr, val);
        } else {
            value_cast_inplace(s, &argv[i], VALUE_TYPE_STRING);
            value_add_to_key_array(arr, STR_OF(argv[i]), val);
        }
        if (i + 1 >= argc) {
            value_free(val);
        }
        i++;
    }
    return arr;
}

/*
 * Quicksort
 *
 * This function sorts a numerically indexed array based on the
 * contained values.
 */
value *array_sort(intend_state *s, unsigned int argc, value **argv)
{
    qsort(ARR_OF(argv[0]), ARRLEN_OF(argv[0]),
          sizeof(symtab_entry *), compar);
    return value_copy(argv[0]);
}

/*
 * Check if sorted
 *
 * This function returns true if the argument array is sorted.
 */
value *array_is_sorted(intend_state *s, unsigned int argc, value **argv)
{
    symtab_entry **next = ARR_OF(argv[0]);
    int max = ARRLEN_OF(argv[0]);
    int i, res;

    for (i = 0; i < max - 1; i++) {
        res = compar(next, next + 1);
        if (res > 0) {
            return value_make_bool(0);
        }
        next++;
    }
    return value_make_bool(1);
}

/*
 * Delete value from array
 *
 * This function removes the value at the given index from the
 * array and returns the resulting array.
 */
value *array_unset(intend_state *s, unsigned int argc, value **argv)
{
    value_delete_array(argv[0], INT_OF(argv[1]));
    return value_copy(argv[0]);
}

/*
 * Compact array
 *
 * Removes all void values from the given array and returns an
 * array of the remaining values.
 */
value *array_compact(intend_state *s, unsigned int argc, value **argv)
{
    value *arr = argv[0];
    value *copy;
    int i;

    copy = value_make_array();
    for (i = 0; i < ARRLEN_OF(arr); i++) {
        if (ARR_OF(arr)[i] &&
                ARR_OF(arr)[i]->entry_u.var.type != VALUE_TYPE_VOID) {
            if (ARR_OF(arr)[i]->symbol) {
                value_add_to_key_array(copy, ARR_OF(arr)[i]->symbol, &ARR_OF(arr)[i]->entry_u.var);
            } else {
                value_add_to_array(copy, &ARR_OF(arr)[i]->entry_u.var);
            }
        }
    }
    return copy;
}

/*
 * Search for element
 *
 * Returns the index of the element in the array, or void if the element
 * is not found.
 */
value *array_search(intend_state *s, unsigned int argc, value **argv)
{
    value *cmp;
    int i, cmpval;

    for (i = 0; i < ARRLEN_OF(argv[0]); i++) {
        cmp = eval_order_equal(&ARR_OF(argv[0])[i]->entry_u.var, argv[1]);
        cmpval = BOOL_OF(cmp);
        value_free(cmp);
        if (cmpval) {
            return value_make_int(i);
        }
    }
    return value_make_void();
}

/*
 * Merge multiple arrays
 *
 * Returns a new array with the values from all input arrays.
 */
value *array_merge(intend_state *s, unsigned int argc, value **argv)
{
    unsigned int i;
    int j;
    value *arr;

    arr = value_make_array();

    for (i = 0; i < argc; i++) {
        value_cast_inplace(s, &argv[i], VALUE_TYPE_ARRAY);
        for (j = 0; j < ARRLEN_OF(argv[i]); j++) {
            if (ARR_OF(argv[i])[j]->symbol) {
                value_add_to_key_array(arr, ARR_OF(arr)[j]->symbol, &ARR_OF(argv[i])[j]->entry_u.var);
            } else {
                value_add_to_array(arr, &ARR_OF(argv[i])[j]->entry_u.var);
            }
        }
    }
    return arr;
}

/*
 * Reverse array
 *
 * Reverses the order of the elements in the given array and returns
 * the reversed array.
 */
value *array_reverse(intend_state *s, unsigned int argc, value **argv)
{
    int i;
    int len = ARRLEN_OF(argv[0]);
    value *arr;

    arr = value_make_array();
    for (i = 0; i < len; i++) {
        if (ARR_OF(argv[0])[len - i - 1]->symbol) {
            value_add_to_key_array(arr, ARR_OF(argv[0])[len - i - 1]->symbol, &ARR_OF(argv[0])[len - i - 1]->entry_u.var);
        } else {
            value_add_to_array(arr, &ARR_OF(argv[0])[len - i - 1]->entry_u.var);
        }
    }
    return arr;
}
