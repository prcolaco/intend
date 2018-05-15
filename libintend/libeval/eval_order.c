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
 * Intend C Order operator evaluation
 */

#include <stdlib.h>
#include <string.h>

#include "eval.h"

/*
 * compare fn values for equality
 */
static int fns_are_equal(value *a, value *b)
{
    signature *asig = FNSIG_OF(a);
    signature *bsig = FNSIG_OF(b);

    if (asig->type != bsig->type) {
        return 0;
    }
    if (asig->type == FUNCTION_TYPE_BUILTIN) {
        return (asig->call_u.builtin_vector == bsig->call_u.builtin_vector);
    } else {
        return (asig->def == bsig->def);
    }
}

/*
 * order constants
 */
#define ORDER_SMALLER	(-1)
#define ORDER_EQUAL	0
#define ORDER_LARGER	1
#define ORDER_UNKNOWN	42

/*
 * get ordering of two values
 */
static int getorder(value *a, value *b)
{
    int res = ORDER_UNKNOWN;

    switch (a->type) {
        case VALUE_TYPE_VOID:
            res = ORDER_EQUAL;
        case VALUE_TYPE_BOOL:
            if (BOOL_OF(a) < BOOL_OF(b)) {
                res = ORDER_SMALLER;
            } else if (BOOL_OF(a) > BOOL_OF(b)) {
                res = ORDER_LARGER;
            } else {
                res = ORDER_EQUAL;
            }
            break;
        case VALUE_TYPE_INT:
            if (INT_OF(a) < INT_OF(b)) {
                res = ORDER_SMALLER;
            } else if (INT_OF(a) > INT_OF(b)) {
                res = ORDER_LARGER;
            } else {
                res = ORDER_EQUAL;
            }
            break;
        case VALUE_TYPE_FLOAT:
            if (FLOAT_OF(a) < FLOAT_OF(b)) {
                res = ORDER_SMALLER;
            } else if (FLOAT_OF(a) > FLOAT_OF(b)) {
                res = ORDER_LARGER;
            } else {
                res = ORDER_EQUAL;
            }
            break;
        case VALUE_TYPE_STRING:
            if (STRLEN_OF(a) == 0 && STRLEN_OF(b) == 0) {
                res = ORDER_EQUAL;
            } else if (STRLEN_OF(a) == 0) {
                res = ORDER_SMALLER;
            } else if (STRLEN_OF(b) == 0) {
                res = ORDER_LARGER;
            } else {
                int min, tmp;

                if (STRLEN_OF(a) > STRLEN_OF(b)) {
                    res = ORDER_LARGER;
                    min = STRLEN_OF(b);
                } else if (STRLEN_OF(a) < STRLEN_OF(b)) {
                    res = ORDER_SMALLER;
                    min = STRLEN_OF(a);
                } else {
                    res = ORDER_EQUAL;
                    min = STRLEN_OF(a);
                }
                tmp = memcmp(STR_OF(a),
                             STR_OF(b), min);
                if (tmp < 0) {
                    res = ORDER_SMALLER;
                } else if (tmp > 0) {
                    res = ORDER_LARGER;
                }
            }
            break;
        case VALUE_TYPE_FN:
            if (fns_are_equal(a, b)) {
                res = ORDER_EQUAL;
            }
            break;
        case VALUE_TYPE_ARRAY:
        case VALUE_TYPE_STRUCT:
        case VALUE_TYPE_RES:
            /* can't compute, assume not equal */
            break;
    }
    return res;
}

/*
 * Evaluate equality operator
 */
value *eval_order_equal(value *a, value *b)
{
    int res;

    sanity(a && b);

    res = (a->type == b->type && getorder(a, b) == ORDER_EQUAL);
    return value_make_bool(res);
}

/*
 * Evaluate non-equality operator
 */
value *eval_order_not_equal(value *a, value *b)
{
    int res;

    sanity(a && b);

    res = (a->type != b->type || getorder(a, b) != ORDER_EQUAL);
    return value_make_bool(res);
}

/*
 * Evaluate smaller-or-equal operator
 */
value *eval_order_seq(value *a, value *b)
{
    int order;

    sanity(a && b);

    order = getorder(a, b);
    return value_make_bool(order == ORDER_SMALLER || order == ORDER_EQUAL);
}

/*
 * Evaluate larger-or-equal operator
 */
value *eval_order_leq(value *a, value *b)
{
    int order;

    sanity(a && b);

    order = getorder(a, b);
    return value_make_bool(order == ORDER_LARGER || order == ORDER_EQUAL);
}

/*
 * Evaluate smaller operator
 */
value *eval_order_smaller(value *a, value *b)
{
    int order;

    sanity(a && b);

    order = getorder(a, b);
    return value_make_bool(order == ORDER_SMALLER);
}

/*
 * Evaluate larger operator
 */
value *eval_order_larger(value *a, value *b)
{
    int order;

    sanity(a && b);

    order = getorder(a, b);
    return value_make_bool(order == ORDER_LARGER);
}
