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
 * Intend C List functions module
 */

#include <stdlib.h>

#include <intend.h>

// To use eval routines (this is piracy)
#include <eval.h>

#include "intend_list.h"

/*
 * List functions to register
 */
static intend_function_data list_funcs[] = {
    { "nil",            list_nil,           0,  "",     'a' },
    { "cons",           list_cons,          2,  "?a",   'a' },
    { "head",           list_head,          1,  "a",    '?' },
    { "tail",           list_tail,          1,  "a",    'a' },
    { "last",           list_last,          1,  "a",    '?' },
    { "init",           list_initial,       1,  "a",    'a' },
    { "take",           list_take,          2,  "ai",   'a' },
    { "drop",           list_drop,          2,  "ai",   'a' },
    { "length",         list_length,        1,  "a",    'i' },
    { "null",           list_null,          1,  "a",    'b' },
    { "elem",           list_elem,          2,  "a?",   'b' },
    { "intersperse",    list_intersperse,   2,  "a?",   'a' },
    { "replicate",      list_replicate,     2,  "?i",   'a' },

    /* list terminator */
    { NULL,             NULL,               0,  NULL,   0   }
};

/*
 * Initialize the List module
 */
void intend_list_module_initialize(intend_ctx ctx)
{
    intend_register_function_array(ctx, list_funcs);
}

/*
 * Destroy the List module
 */
void intend_list_module_destroy(intend_ctx ctx)
{
    intend_unregister_function_array(ctx, list_funcs);
}

/*
 * Create empty list
 */
intend_value list_nil(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return intend_create_value(INTEND_TYPE_ARRAY, NULL);
}

/*
 * Prepend element to list
 */
intend_value list_cons(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value elem = argv[0];
    intend_value data = argv[1];
    intend_value *delem = NULL;
    int          len = intend_array_count(argv[1]);
    int i;
    intend_value res;

    res = intend_create_value(INTEND_TYPE_ARRAY, elem, NULL);
    for (i = 0; i < len; i++) {
        delem = intend_array_get(data, i);
        intend_array_add(res, delem);
        intend_free_value(delem);
    }
    return res;
}

/*
 * Get first element of list
 */
intend_value list_head(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    int          len = intend_array_count(argv[0]);
    intend_value elem;

    if (len == 0) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    elem = intend_array_get(data, 0);

    return elem;
}

/*
 * Get all elements of list except the first
 */
intend_value list_tail(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    intend_value *delem = NULL;
    int          len = intend_array_count(argv[0]);
    int i;
    intend_value res;

    res = intend_create_value(INTEND_TYPE_ARRAY, NULL);
    for (i = 1; i < len; i++) {
        delem = intend_array_get(data, i);
        intend_array_add(res, delem);
        intend_free_value(delem);
    }

    return res;
}

/*
 * Get last element of list
 */
intend_value list_last(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    int          len = intend_array_count(argv[0]);
    intend_value elem;

    if (len == 0) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    elem = intend_array_get(data, len - 1);

    return elem;
}

/*
 * Get all elements of list except the last
 */
intend_value list_initial(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    intend_value *delem = NULL;
    int          len = intend_array_count(argv[0]);
    int i;
    intend_value res;

    res = intend_create_value(INTEND_TYPE_ARRAY, NULL);
    for (i = 0; i < (len - 1); i++) {
        delem = intend_array_get(data, i);
        intend_array_add(res, delem);
        intend_free_value(delem);
    }

    return res;
}

/*
 * Take a number of elements from the front of list
 */
intend_value list_take(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    intend_value *delem = NULL;
    int          len = intend_array_count(argv[0]);
    int          count = intend_int_value(argv[1]);
    int i;
    intend_value res;

    if (count > len) count = len;

    res = intend_create_value(INTEND_TYPE_ARRAY, NULL);
    for (i = 0; i < count; i++) {
        delem = intend_array_get(data, i);
        intend_array_add(res, delem);
        intend_free_value(delem);
    }
    return res;
}

/*
 * Drop a number of elements from the front of list
 */
intend_value list_drop(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    intend_value *delem = NULL;
    int          len = intend_array_count(argv[0]);
    int          count = intend_int_value(argv[1]);
    int i;
    intend_value res;

    if (count > len) count = len;

    res = intend_create_value(INTEND_TYPE_ARRAY, NULL);
    for (i = count; i < len; i++) {
        delem = intend_array_get(data, i);
        intend_array_add(res, delem);
        intend_free_value(delem);
    }

    return res;
}

/*
 * Length of list
 */
intend_value list_length(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int len = intend_array_count(argv[0]);

    return intend_create_value(INTEND_TYPE_INT, &len);
}

/*
 * Check for empty list
 */
intend_value list_null(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int len = intend_array_count(argv[0]);
    int empty = (len == 0);

    return intend_create_value(INTEND_TYPE_BOOL, &empty);
}

/*
 * Check for element in list
 */
intend_value list_elem(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    int          len = intend_array_count(argv[0]);
    int i, flag = 0;
    intend_value check;

    for (i = 0; i < len; i++) {
        check = eval_order_equal(intend_array_get(data, i), argv[1]);
        flag = intend_bool_value(check);
        intend_free_value(check);
        if (flag) break;
    }

    return intend_create_value(INTEND_TYPE_BOOL, &flag);
}

/*
 * Intersperse value between list elements
 */
intend_value list_intersperse(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value data = argv[0];
    intend_value *delem;
    int          len = intend_array_count(argv[0]);
    int i;
    intend_value res;

    res = intend_create_value(INTEND_TYPE_ARRAY, NULL);
    for (i = 0; i < len; i++) {
        delem = intend_array_get(data, i);
        intend_array_add(res, delem);
        intend_free_value(delem);
        if (i != (len - 1)) {
            intend_array_add(res, argv[1]);
        }
    }

    return res;
}

/*
 * Replicate element to create list
 */
intend_value list_replicate(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int count = intend_int_value(argv[1]);
    int i;
    intend_value res;

    res = intend_create_value(INTEND_TYPE_ARRAY, NULL);
    for (i = 0; i < count; i++) {
        intend_array_add(res, argv[0]);
    }

    return res;
}
