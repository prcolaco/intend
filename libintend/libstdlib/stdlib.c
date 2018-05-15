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
 * Intend C Initialize stdlib functions
 */

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "stdlib.h"

/*
 * stdlib variables to register
 */
int		stdlib_SAFE_MODE	= 0;
int		stdlib_INT_MIN		= INT_MIN;
int		stdlib_INT_MAX		= INT_MAX;
int		stdlib_FLT_DIG		= DBL_DIG;
int		stdlib_FLT_MANT_DIG	= DBL_MANT_DIG;
int		stdlib_FLT_MAX_EXP	= DBL_MAX_EXP;
int		stdlib_FLT_MIN_EXP	= DBL_MIN_EXP;
int		stdlib_FLT_RADIX	= FLT_RADIX;
double	stdlib_FLT_EPSILON	= DBL_EPSILON;
double	stdlib_FLT_MAX		= DBL_MAX;
double	stdlib_FLT_MIN		= DBL_MIN;

static register_var_data vars[] = {
    { "SAFE_MODE",		VALUE_TYPE_BOOL,	&stdlib_SAFE_MODE		},
    { "INT_MIN",		VALUE_TYPE_INT,		&stdlib_INT_MIN			},
    { "INT_MAX",		VALUE_TYPE_INT,		&stdlib_INT_MAX			},
    { "FLT_DIG",		VALUE_TYPE_INT,		&stdlib_FLT_DIG			},
    { "FLT_MANT_DIG",	VALUE_TYPE_INT,		&stdlib_FLT_MANT_DIG	},
    { "FLT_MAX_EXP",	VALUE_TYPE_INT,		&stdlib_FLT_MAX_EXP		},
    { "FLT_MIN_EXP",	VALUE_TYPE_INT,		&stdlib_FLT_MIN_EXP		},
    { "FLT_RADIX",		VALUE_TYPE_INT,		&stdlib_FLT_RADIX		},
    { "FLT_EPSILON",	VALUE_TYPE_FLOAT,	&stdlib_FLT_EPSILON		},
    { "FLT_MAX",		VALUE_TYPE_FLOAT,	&stdlib_FLT_MAX			},
    { "FLT_MIN",		VALUE_TYPE_FLOAT,	&stdlib_FLT_MIN			},

    { NULL,				VALUE_TYPE_VOID,	NULL					}
};

/*
 * Register command line arguments
 */
static void register_args(intend_state *s, int argc, char **argv)
{
    value *val, *vector;
    int i;

    val = value_make_int(argc);
    symtab_stack_add_variable(s, "argc", val);
    value_free(val);

    vector = value_make_array();
    for (i = 0; i < argc; i++) {
        val = value_make_string(argv[i]);
        value_add_to_array(vector, val);
        value_free(val);
    }
    symtab_stack_add_variable(s, "argv", vector);
    value_free(vector);
}

/*
 * object class tostring method
 */
value *object_tostring(intend_state *s, unsigned int argc, value **argv)
{
    value *val;
    value *this = symtab_stack_get_variable(s, "this");

    val = value_get_struct(this, "_object");
    value_cast_inplace(s, &val, VALUE_TYPE_STRING);

    return val;
}

/*
 * object class tofloat method
 */
value *object_tofloat(intend_state *s, unsigned int argc, value **argv)
{
    value *val;
    value *this = symtab_stack_get_variable(s, "this");

    val = value_get_struct(this, "_object");
    value_cast_inplace(s, &val, VALUE_TYPE_FLOAT);

    return val;
}

/*
 * object class toint method
 */
value *object_toint(intend_state *s, unsigned int argc, value **argv)
{
    value *val;
    value *this = symtab_stack_get_variable(s, "this");

    val = value_get_struct(this, "_object");
    value_cast_inplace(s, &val, VALUE_TYPE_INT);

    return val;
}

/*
 * object class constructor
 */
value *object_constructor(intend_state *s, unsigned int argc, value **argv)
{
    value *val = argv[0];
    value *this = symtab_stack_get_variable(s, "this");

    value_set_struct(this, "_object", val);
    value_set_struct_method(this, "tostring", object_tostring, 0, "", 's');
    value_set_struct_method(this, "tofloat", object_tofloat, 0, "", 'f');
    value_set_struct_method(this, "toint", object_toint, 0, "", 'i');

    return value_make_void();
}

/*
 * Initialize stdlib functions
 *
 * This function registers the stdlib functions in the current
 * symbol table. Usually this means the global symbol table.
 */
void stdlib_init(intend_state *s, int argc, char **argv)
{
    // Register command line arguments
    register_args(s, argc, argv);

    // Register common vars
    register_variable_array(s, vars);

    // Register object class
    register_class(s, "object", object_constructor, 1, "?");

    // Initialize all lib parts
    rt_init(s);
    mod_init(s);
    str_init(s);
    time_init(s);
    rnd_init(s);
    env_init(s);
    lc_init(s);
    fn_init(s);
    array_init(s);
    struct_init(s);
}

