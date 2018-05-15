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
 * Intend C Random number functions
 */

#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "stdlib.h"

/*
 * Random variables to register
 */
int	rnd_RAND_MAX		= RAND_MAX;

static register_var_data rnd_vars[] = {
    { "RAND_MAX",	VALUE_TYPE_INT,		&rnd_RAND_MAX	},

    { NULL,			VALUE_TYPE_VOID,	NULL			}
};

/*
 * Random functions to register
 */
static register_func_data rnd_funcs[] = {
    { "rand",	rnd_random,		2,	"ii",	'i'	},
    { "srand",	rnd_srandom,	1,	"i",	'v'	},

    /* list terminator */
    { NULL,		NULL,			0,	NULL,	0	}
};

/*
 * Initialize the Random number functions
 */
void rnd_init(intend_state *s)
{
    // Register variables
    register_variable_array(s, rnd_vars);

    // Register functions
    register_function_array(s, rnd_funcs);
}

/*
 * Generate random int
 *
 * This function generates a random unsigned integer inside a
 * specific range. The upper and lower bound are considered part
 * of the range.
 */
value *rnd_random(intend_state *s, unsigned int argc, value **argv)
{
    int lower = INT_OF(argv[0]);
    int upper = INT_OF(argv[1]);
    int scale, res;

    if (lower > upper) {
        return value_make_void();
    }

    if (lower > RAND_MAX) {
        return value_make_int(lower);
    }

    if (upper > RAND_MAX) {
        upper = RAND_MAX;
    }

    if (!s->seed_init) {
        srand(time(NULL));
        s->seed_init = 1;
    }

    scale = upper - lower + 1;
    res = (rand() % scale) + lower;

    return value_make_int(res);
}

/*
 * Seed random generator
 *
 * This function seeds the random generator with the given unsigned
 * int value.
 */
value *rnd_srandom(intend_state *s, unsigned int argc, value **argv)
{
    srand(INT_OF(argv[0]));
    return value_make_void();
}
