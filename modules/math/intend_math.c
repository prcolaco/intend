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
 * Math module
 */

#include <math.h>
#include <stdlib.h>

#include <intend.h>

#include "intend_math.h"

/*
 * Math functions to register
 */
static intend_function_data math_funcs[] = {
    { "exp",        math_exp,       1,  "F",    'f' },
    { "log",        math_log,       1,  "F",    'f' },
    { "log10",      math_log10,     1,  "F",    'f' },
    { "sqrt",       math_sqrt,      1,  "F",    'f' },
    { "ceil",       math_ceil,      1,  "F",    'f' },
    { "floor",      math_floor,     1,  "F",    'f' },
    { "fabs",       math_fabs,      1,  "F",    'f' },
    { "sin",        math_sin,       1,  "F",    'f' },
    { "cos",        math_cos,       1,  "F",    'f' },
    { "tan",        math_tan,       1,  "F",    'f' },
    { "asin",       math_asin,      1,  "F",    'f' },
    { "acos",       math_acos,      1,  "F",    'f' },
    { "atan",       math_atan,      1,  "F",    'f' },
    { "sinh",       math_sinh,      1,  "F",    'f' },
    { "cosh",       math_cosh,      1,  "F",    'f' },
    { "tanh",       math_tanh,      1,  "F",    'f' },
    { "abs",        math_abs,       1,  "I",    'i' },
    { "fmod",       math_fmod,      1,  "FF",   'i' },
    { "powf",       math_powf,      1,  "FF",   'i' },

    /* list terminator */
    { NULL,     NULL,           0,  NULL,   0   }
};

/*
 * Initialize the Math module
 */
void intend_math_module_initialize(intend_ctx ctx)
{
    intend_register_function_array(ctx, math_funcs);
}

/*
 * Destroy the Math module
 */
void intend_math_module_destroy(intend_ctx ctx)
{
    intend_unregister_function_array(ctx, math_funcs);
}

/*
 * Compute exponential
 */
intend_value math_exp(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = exp(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute natural logarithm
 */
intend_value math_log(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = log(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute base-10 logarithm
 */
intend_value math_log10(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = log10(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute square root
 */
intend_value math_sqrt(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = sqrt(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute ceiling (rounding up)
 */
intend_value math_ceil(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = ceil(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute floor (rounding down)
 */
intend_value math_floor(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = floor(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute absolute value
 */
intend_value math_fabs(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = fabs(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute sine
 */
intend_value math_sin(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = sin(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute cosine
 */
intend_value math_cos(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = cos(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute tangent
 */
intend_value math_tan(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = tan(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute arc-sine
 */
intend_value math_asin(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = asin(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute arc-cosine
 */
intend_value math_acos(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = acos(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute arc-tangent
 */
intend_value math_atan(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = atan(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute hyperbolic sine
 */
intend_value math_sinh(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = sinh(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute hyperbolic cosine
 */
intend_value math_cosh(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = cosh(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute hyperbolic tangent
 */
intend_value math_tanh(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = tanh(intend_float_value(argv[0]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}

/*
 * Compute absolute value of integer
 */
intend_value math_abs(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int ret;

    ret = abs(intend_int_value(argv[0]));
    return intend_create_value(INTEND_TYPE_INT, &ret);
}

/*
 * Compute modulus of floats
 */
intend_value math_fmod(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    if(intend_float_value(argv[1]) == 0) {
        intend_fatal_error(ctx, "float division by zero");
        ret = 0.0;
    } else {
        ret = fmod(intend_float_value(argv[0]), intend_float_value(argv[1]));
    }
    return intend_create_value(INTEND_TYPE_INT, &ret);
}

/*
 * Compute power of floats
 */
intend_value math_powf(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double ret;

    ret = pow(intend_float_value(argv[0]), intend_float_value(argv[1]));
    return intend_create_value(INTEND_TYPE_FLOAT, &ret);
}
