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
 * Intend C Evaluation library
 *
 * This library provides functions needed to evaluate statements
 * and expressions in accordance with the defined semantics of
 * the language.
 */

#ifndef INTEND_EVAL_H
#define INTEND_EVAL_H

#include "../libmisc/misc.h"
#include "../libruntime/runtime.h"
#include "../libparser/parser.h"

/*
 * Helper functions
 */
void eval_free_floats(intend_state *s);
void eval_call_args(intend_state *s, unsigned int argc, expr **args,
                    value ***argv);
void free_call_args(intend_state *s, unsigned int argc, value ***argv);
void update_call_args(intend_state *s, signature *sig, unsigned int argc,
                      expr **args);

/*
 * Simple expressions
 */
value *eval_const_void(void);
value *eval_const_bool(expr *ex);
value *eval_const_int(expr *ex);
value *eval_const_float(expr *ex);
value *eval_const_string(intend_state *s, expr *ex);
value *eval_cast(intend_state *s, expr *ex);
value *eval_assign(intend_state *s, expr *ex);
value *eval_assign_array(intend_state *s, expr *ex);
value *eval_ref(intend_state *s, expr *ex);
value *eval_ref_array(intend_state *s, expr *ex);
value *eval_ref_index(intend_state *s, expr *ex);
value *eval_call(intend_state *s, expr *ex);
value *eval_new(intend_state *s, expr *ex);
value *eval_static(intend_state *s, expr *ex);
value *eval_method(intend_state *s, expr *ex);
value *eval_static_ref(intend_state *s, expr *ex);
value *eval_prefix(intend_state *s, expr *ex);
value *eval_postfix(intend_state *s, expr *ex);
value *eval_infix(intend_state *s, expr *ex);
value *eval_lambda(intend_state *s, expr *ex);

void eval_assign_array_direct(intend_state *s, const char *name,
                              int argc, expr **index, value *val);

/*
 * Math evaluation
 */
value *eval_math_plus(intend_state *s, value *a, value *b);
value *eval_math_minus(intend_state *s, value *a, value *b);
value *eval_math_mul(intend_state *s, value *a, value *b);
value *eval_math_div(intend_state *s, value *a, value *b);
value *eval_math_mod(intend_state *s, value *a, value *b);
value *eval_math_pow(intend_state *s, value *a, value *b);

/*
 * Boolean math evaluation
 */
value *eval_bool_and(intend_state *s, expr *a, expr *b);
value *eval_bool_or(intend_state *s, expr *a, expr *b);
value *eval_expr_if(intend_state *s, expr *ex);

/*
 * Bitwise math evaluation
 */
value *eval_bit_and(intend_state *s, value *a, value *b);
value *eval_bit_or(intend_state *s, value *a, value *b);
value *eval_bit_xor(intend_state *s, value *a, value *b);
value *eval_bit_lshift(intend_state *s, value *a, value *b);
value *eval_bit_rshift(intend_state *s, value *a, value *b);

/*
 * Order evaluation
 */
value *eval_order_equal(value *a, value *b);
value *eval_order_not_equal(value *a, value *b);
value *eval_order_seq(value *a, value *b);
value *eval_order_leq(value *a, value *b);
value *eval_order_smaller(value *a, value *b);
value *eval_order_larger(value *a, value *b);

/*
 * String evaluation
 */
value *eval_string_plus(intend_state *s, value *a, value *b);

/*
 * Top-level expression evaluation
 */
value *eval_expr(intend_state *s, expr *ex);

/*
 * Statement evaluation
 */
void eval_stmt(intend_state *s, stmt *st, int cookie);
void eval_stmt_switch(intend_state *s, stmt *st, int cookie);
void eval_stmt_list(intend_state *s, stmt_list *list, int cookie);

#endif
