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
 * Intend C Modules functions
 */

#include <stdio.h>
#include <stdlib.h>

#include "stdlib.h"

/*
 * mod functions to register
 */
static register_func_data mod_funcs[] = {
    { "module_is_loaded",	mod_is_loaded,		1,	"s",	'b'	},
    { "module_load",		mod_load,			1,	"",		'v'	},
    { "module_unload",		mod_unload,			1,	"",		'v'	},

    /* list terminator */
    { NULL,		NULL,			0,	NULL,	0	}
};

/*
 * Initialize the Modules functions
 */
void mod_init(intend_state *s)
{
    // Register functions
    register_function_array(s, mod_funcs);
}

/*
 * Check if a module is loaded
 */
value *mod_is_loaded(intend_state *s, unsigned int argc, value **argv)
{
    char *modname = STR_OF(argv[0]);

    return value_make_bool(module_is_loaded(s, modname));
}

/*
 * Load some modules
 */
value *mod_load(intend_state *s, unsigned int argc, value **argv)
{
    char *modname = NULL;
    char *err = NULL;
    int i;

    for (i = 0; i < argc; i++) {
        modname = STR_OF(argv[i]);
        if (!module_load(s, modname)) {
            // Couldn't load module, throw exception then...
            asprintf(&err, "%s: error loading module", modname);
            except_throw(s, value_make_string(err));
            free(err);
        }
    }
    return value_make_void();
}

/*
 * Unload some modules
 */
value *mod_unload(intend_state *s, unsigned int argc, value **argv)
{
    char *modname = NULL;
    char *err = NULL;
    int i;

    for (i = 0; i < argc; i++) {
        modname = STR_OF(argv[i]);
        if (!module_unload(s, modname)) {
            // Couldn't load module, throw exception then...
            asprintf(&err, "%s: error unloading module", modname);
            except_throw(s, value_make_string(modname));
            free(err);
        }
    }
    return value_make_void();
}

