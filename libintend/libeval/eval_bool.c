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
 * Intend C Boolean operator evaluation
 */

#include <stdlib.h>

#include "eval.h"

/*
 * Evaluate boolean AND
 */
value *eval_bool_and(intend_state *s, expr *a, expr *b)
{
    int bool_val;
    value *val;

    sanity(a && b);

    val = eval_expr(s, a);
    value_cast_inplace(s, &val, VALUE_TYPE_BOOL);
    bool_val = BOOL_OF(val);
    value_free(val);

    if (!bool_val) {
        return value_make_bool(0);
    }

    val = eval_expr(s, b);
    value_cast_inplace(s, &val, VALUE_TYPE_BOOL);

    return val;
}

/*
 * Evaluate boolean OR
 */
value *eval_bool_or(intend_state *s, expr *a, expr *b)
{
    int bool_val;
    value *val;

    sanity(a && b);

    val = eval_expr(s, a);
    value_cast_inplace(s, &val, VALUE_TYPE_BOOL);
    bool_val = BOOL_OF(val);
    value_free(val);

    if (bool_val) {
        return value_make_bool(1);
    }

    val = eval_expr(s, b);
    value_cast_inplace(s, &val, VALUE_TYPE_BOOL);

    return val;
}

/*
 * Evaluate conditional expression
 */
value *eval_expr_if(intend_state *s, expr *ex)
{
    value *test, *res;

    sanity(ex && ex->inner && ex->index && ex->elif);

    test = eval_expr(s, ex->inner);
    value_cast_inplace(s, &test, VALUE_TYPE_BOOL);

    if (BOOL_OF(test)) {
        res = eval_expr(s, ex->index);
    } else {
        res = eval_expr(s, ex->elif);
    }

    value_free(test);
    return res;
}
