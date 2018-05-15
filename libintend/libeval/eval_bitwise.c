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
 * Intend C Bitwise operator evaluation
 */

#include <stdlib.h>

#include "eval.h"

/*
 * Evaluate bitwise AND
 */
value *eval_bit_and(intend_state *s, value *a, value *b)
{
    sanity(a && b);

    return value_make_int(INT_OF(a) & INT_OF(b));
}

/*
 * Evaluate bitwise OR
 */
value *eval_bit_or(intend_state *s, value *a, value *b)
{
    sanity(a && b);

    return value_make_int(INT_OF(a) | INT_OF(b));
}

/*
 * Evaluate bitwise XOR
 */
value *eval_bit_xor(intend_state *s, value *a, value *b)
{
    sanity(a && b);

    return value_make_int(INT_OF(a) ^ INT_OF(b));
}

/*
 * Evaluate bitwise left shift
 */
value *eval_bit_lshift(intend_state *s, value *a, value *b)
{
    sanity(a && b);

    return value_make_int(INT_OF(a) << INT_OF(b));
}

/*
 * Evaluate bitwise right shift
 */
value *eval_bit_rshift(intend_state *s, value *a, value *b)
{
    sanity(a && b);

    return value_make_int(INT_OF(a) >> INT_OF(b));
}
