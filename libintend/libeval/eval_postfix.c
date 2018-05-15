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
 * Intend C Postfix operator evaluation
 */

#include <stdlib.h>

#include "eval.h"

/*
 * Evaluate post-increment operator
 */
static value *postincrement(intend_state *s, expr *ex)
{
    value *val, *copy;

    val = eval_expr(s, ex);
    value_cast_inplace(s, &val, VALUE_TYPE_INT);

    copy = value_make_int(INT_OF(val));

    ++INT_OF(val);

    if (!s->except_flag && !s->exit_flag) {
        if (ex->type == EXPR_REF) {
            symtab_stack_add_variable(s, ex->name, val);
        } else if (ex->type == EXPR_REF_ARRAY) {
            eval_assign_array_direct(s, ex->name, ex->argc, ex->argv, val);
        }
    }
    value_free(val);

    return copy;
}

/*
 * Evaluate post-decrement operator
 */
static value *postdecrement(intend_state *s, expr *ex)
{
    value *val, *copy;

    val = eval_expr(s, ex);
    value_cast_inplace(s, &val, VALUE_TYPE_INT);

    copy = value_make_int(INT_OF(val));

    --INT_OF(val);

    if (!s->except_flag && !s->exit_flag) {
        if (ex->type == EXPR_REF) {
            symtab_stack_add_variable(s, ex->name, val);
        } else if (ex->type == EXPR_REF_ARRAY) {
            eval_assign_array_direct(s, ex->name, ex->argc, ex->argv, val);
        }
    }
    value_free(val);

    return copy;
}

/*
 * Evaluate postfix operator
 */
value *eval_postfix(intend_state *s, expr *ex)
{
    value *res = NULL;

    sanity(ex);

    switch (ex->op) {
        case OPTYPE_POSTINC:
            res = postincrement(s, ex->inner);
            break;
        case OPTYPE_POSTDEC:
            res = postdecrement(s, ex->inner);
            break;
        default:
            sanity(0);
    }
    return res;
}
