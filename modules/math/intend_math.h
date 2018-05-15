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
 * Intend C Math functions interface module
 */

#ifndef INTEND_MODULE_MATH_H
#define INTEND_MODULE_MATH_H

/*
 * Initialize the Math module
 */
void intend_math_module_initialize(intend_ctx ctx);

/*
 * Destroy the Math module
 */
void intend_math_module_destroy(intend_ctx ctx);

/*
 * Compute exponential
 */ 
intend_value math_exp(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute natural logarithm
 */
intend_value math_log(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute base-10 logarithm
 */
intend_value math_log10(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute square root
 */
intend_value math_sqrt(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute ceiling (rounding up)
 */
intend_value math_ceil(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute floor (rounding down)
 */
intend_value math_floor(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute absolute value
 */
intend_value math_fabs(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute sine
 */
intend_value math_sin(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute cosine
 */
intend_value math_cos(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute tangent
 */
intend_value math_tan(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute arc-sine
 */
intend_value math_asin(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute arc-cosine
 */
intend_value math_acos(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute arc-tangent
 */
intend_value math_atan(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute hyperbolic sine
 */
intend_value math_sinh(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute hyperbolic cosine
 */
intend_value math_cosh(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute hyperbolic tangent
 */
intend_value math_tanh(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute absolute value of integer
 */
intend_value math_abs(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute modulus of floats
 */
intend_value math_fmod(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compute power of floats
 */
intend_value math_powf(intend_ctx ctx, unsigned int argc, intend_value *argv);

#endif
