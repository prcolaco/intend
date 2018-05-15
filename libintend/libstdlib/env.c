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
 * Intend C Environment functions
 */

#include <stdlib.h>

#include "stdlib.h"

/*
 * Environment functions to register
 */
static register_func_data env[] = {
    /* Environment functions */
    { "getenv",		env_getenv,			1,	"s",	'?'	},
    { "setenv",		env_setenv,			2,	"ss",	'b'	},
    { "unsetenv",		env_unsetenv,	1,	"s",	'v'	},

    /* list terminator */
    { NULL,			NULL,				0,	NULL,	0	}
};

/*
 * Initialize the Environment functions
 */
void env_init(intend_state *s)
{
    register_function_array(s, env);
}

/*
 * Get environment value
 *
 * Tries to get the value of the given environment variable. Returns
 * the value as a string on success, or void on failure.
 */
value *env_getenv(intend_state *s, unsigned int argc, value **argv)
{
    const char *name = STR_OF(argv[0]);
    char *env;

    if (!value_str_compat(argv[0]) || !(env = getenv(name))) {
        return value_make_void();
    }
    return value_make_string(env);
}

/*
 * Set environment value
 *
 * Tries to set the value of the given environment variable. Returns
 * true on success, or false on failure.
 */
value *env_setenv(intend_state *s, unsigned int argc, value **argv)
{
    const char *name = STR_OF(argv[0]);
    const char *val  = STR_OF(argv[1]);
    int ret;

    if (safe_mode_get(s)) {
        nonfatal(s, "evironment set restricted in safe mode");
        return value_make_bool(0);
    }

    if (!value_str_compat(argv[0]) || !value_str_compat(argv[1])) {
        return value_make_bool(0);
    }
    ret = setenv(name, val, 1);

    return value_make_bool(ret);
}

/*
 * Unset environment value
 *
 * Removes the given environment variable.
 */
value *env_unsetenv(intend_state *s, unsigned int argc, value **argv)
{
    const char *name = STR_OF(argv[0]);

    if (safe_mode_get(s)) {
        nonfatal(s, "evironment unset restricted in safe mode");
        return value_make_void();
    }

    if (value_str_compat(argv[0])) {
        unsetenv(name);
    }
    return value_make_void();
}

