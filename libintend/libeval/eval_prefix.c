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
 * Intend C Prefix operator evaluation
 */

#include <stdlib.h>

#include "eval.h"

/*
 * Type promotion for prefix operators
 *
 * Promotes the value to int if the operator only allows
 * ints, otherwise float is also allowed.
 */
static void promote_prefix(intend_state *s, value **one, int allow_float)
{
    if (!allow_float || (*one)->type != VALUE_TYPE_FLOAT) {
        value_cast_inplace(s, one, VALUE_TYPE_INT);
    }
}

/*
 * Evaluate unary minus operator
 */
static value *unary_minus(intend_state *s, expr *ex)
{
    value *val;

    val = eval_expr(s, ex);
    promote_prefix(s, &val, 1);

    if (val->type == VALUE_TYPE_INT) {
        INT_OF(val) = - INT_OF(val);
    } else if (val->type == VALUE_TYPE_FLOAT) {
        FLOAT_OF(val) = - FLOAT_OF(val);
    }
    return val;
}

/*
 * Evaluate logical NOT operator
 */
static value *logical_not(intend_state *s, expr *ex)
{
    value *val;

    val = eval_expr(s, ex);
    value_cast_inplace(s, &val, VALUE_TYPE_BOOL);

    BOOL_OF(val) ^= 1;

    return val;
}

/*
 * Evaluate pre-increment operator
 */
static value *preincrement(intend_state *s, expr *ex)
{
    value *val;

    val = eval_expr(s, ex);
    promote_prefix(s, &val, 0);

    ++INT_OF(val);

    if (!s->except_flag && !s->exit_flag) {
        if (ex->type == EXPR_REF) {
            symtab_stack_add_variable(s, ex->name, val);
        } else if (ex->type == EXPR_REF_ARRAY) {
            eval_assign_array_direct(s, ex->name, ex->argc, ex->argv, val);
        }
    }

    return val;
}

/*
 * Evaluate pre-decrement operator
 */
static value *predecrement(intend_state *s, expr *ex)
{
    value *val;

    val = eval_expr(s, ex);
    promote_prefix(s, &val, 0);

    --INT_OF(val);

    if (!s->except_flag && !s->exit_flag) {
        if (ex->type == EXPR_REF) {
            symtab_stack_add_variable(s, ex->name, val);
        } else if (ex->type == EXPR_REF_ARRAY) {
            eval_assign_array_direct(s, ex->name, ex->argc, ex->argv, val);
        }
    }

    return val;
}

/*
 * Evaluate bit-negation operator
 */
value *negate(intend_state *s, expr *ex)
{
    value *val;

    val = eval_expr(s, ex);
    promote_prefix(s, &val, 0);

    INT_OF(val) = ~ INT_OF(val);

    return val;
}

/*
 * Evaluate prefix operator
 */
value *eval_prefix(intend_state *s, expr *ex)
{
    value *res = NULL;

    sanity(ex);

    switch (ex->op) {
        case OPTYPE_MINUS:
            res = unary_minus(s, ex->inner);
            break;
        case OPTYPE_NOT:
            res = logical_not(s, ex->inner);
            break;
        case OPTYPE_PREINC:
            res = preincrement(s, ex->inner);
            break;
        case OPTYPE_PREDEC:
            res = predecrement(s, ex->inner);
            break;
        case OPTYPE_NEG:
            res = negate(s, ex->inner);
            break;
        default:
            sanity(0);
    }
    return res;
}
