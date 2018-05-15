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
 * Intend C Math operator evaluation
 */

#include <math.h>
#include <stdlib.h>

#include "eval.h"

/*
 * Evaluate addition
 */
value *eval_math_plus(intend_state *s, value *a, value *b)
{
    value *res;

    sanity(a && b);

    if (a->type == VALUE_TYPE_INT) {
        res = value_make_int(INT_OF(a) + INT_OF(b));
    } else {
        res = value_make_float(FLOAT_OF(a) + FLOAT_OF(b));
    }
    return res;
}

/*
 * Evaluate subtraction
 */
value *eval_math_minus(intend_state *s, value *a, value *b)
{
    value *res;

    sanity(a && b);

    if (a->type == VALUE_TYPE_INT) {
        res = value_make_int(INT_OF(a) - INT_OF(b));
    } else {
        res = value_make_float(FLOAT_OF(a) - FLOAT_OF(b));
    }
    return res;
}

/*
 * Evaluate multiplication
 */
value *eval_math_mul(intend_state *s, value *a, value *b)
{
    value *res;

    sanity(a && b);

    if (a->type == VALUE_TYPE_INT) {
        res = value_make_int(INT_OF(a) * INT_OF(b));
    } else {
        res = value_make_float(FLOAT_OF(a) * FLOAT_OF(b));
    }
    return res;
}

/*
 * Evaluate division
 */
value *eval_math_div(intend_state *s, value *a, value *b)
{
    value *res = NULL;

    sanity(a && b);

    if (a->type == VALUE_TYPE_INT) {
        if (INT_OF(b) != 0) {
            res = value_make_int(INT_OF(a) / INT_OF(b));
        } else {
            fatal(s, "integer division by zero");
            return value_make_int(0);
        }
    } else {
        if (FLOAT_OF(b) != 0.0) {
            res = value_make_float(FLOAT_OF(a) / FLOAT_OF(b));
        } else {
            fatal(s, "floating point division by zero");
            return value_make_float(0.0);
        }
    }
    return res;
}

/*
 * Evaluate modulus
 */
value *eval_math_mod(intend_state *s, value *a, value *b)
{
    value *va = a;
    value *vb = b;
    value *res = NULL;

    sanity(a && b);

	// Cast both values to int if not ints
    if (a->type != VALUE_TYPE_INT) va = value_cast(s, a, VALUE_TYPE_INT);
	if (b->type != VALUE_TYPE_INT) vb = value_cast(s, b, VALUE_TYPE_INT);
	// Perform the modulus
    if (INT_OF(vb) != 0) {
        res = value_make_int(INT_OF(va) % INT_OF(vb));
    } else {
        fatal(s, "integer division by zero");
        res = value_make_int(0);
    }
    // Free temp values
    if(va != a) value_free(va);
    if(vb != b) value_free(vb);

    return res;
}

/*
 * Evaluate power
 */
value *eval_math_pow(intend_state *s, value *a, value *b)
{
    value *res;
    int ival = 1, iorig;
    value *va = a;
    value *vb = b;

    sanity(a && b);

	// Cast both values to int if not ints
    if (a->type != VALUE_TYPE_INT) va = value_cast(s, a, VALUE_TYPE_INT);
	if (b->type != VALUE_TYPE_INT) vb = value_cast(s, b, VALUE_TYPE_INT);
	// Perform the pow
    iorig = INT_OF(vb);
    if (iorig < 0) iorig = 0;
    while (iorig-- > 0) {
        ival *= INT_OF(va);
    }
    res = value_make_int(ival);
    // Free temp values if needed
    if(va != a) value_free(va);
    if(vb != b) value_free(vb);

    return res;
}
