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
 * Intend C Register variables and functions in runtime
 */

#include <stdlib.h>

#include "runtime.h"


/*
 * Register a class in the interpreter context
 */
void register_class(intend_state *s, char *name, call_func vector, unsigned int args,
                       char *proto)
{
    signature sig;

    sanity(name && vector && proto);

    sig.type = FUNCTION_TYPE_BUILTIN;
    sig.args  = args;
    sig.name  = name;
    sig.proto = proto;
    sig.call_u.builtin_vector = vector;
    sig.rettype = 'v';
    sig.def = NULL;
    symtab_stack_add_builtin_class(s, name, NULL, &sig);
}

/*
 * Register a function in the interpreter context
 */
void register_function(intend_state *s, char *name, call_func vector, unsigned int args,
                       char *proto, char rettype)
{
    signature sig;

    sanity(name && vector && proto);

    sig.type = FUNCTION_TYPE_BUILTIN;
    sig.args  = args;
    sig.name  = name;
    sig.proto = proto;
    sig.call_u.builtin_vector = vector;
    sig.rettype = rettype;
    symtab_stack_add_global_function(s, name, &sig);
}

/*
 * Register a function in the interpreter context from data struct
 */
void register_function_data(intend_state *s, register_func_data *fdata)
{
    register_function(s, fdata->name, fdata->vector, fdata->args, fdata->proto, fdata->rettype);
}

/*
 * Register a function in the interpreter context from array of data structs
 */
void register_function_array(intend_state *s, register_func_data *farray)
{
    unsigned int i;

    sanity(farray);

    i = 0;
    while (farray[i].name) {
        register_function_data(s, &farray[i]);
        i++;
    }
}

/*
 * Register a variable in the interpreter context
 */
void register_variable(intend_state *s, char *name, value_type type, void *val)
{
    value *v;

    switch (type) {
        case VALUE_TYPE_BOOL:
            v = value_make_bool(*((int *)val));
            break;
        case VALUE_TYPE_INT:
            v = value_make_int(*((int *)val));
            break;
        case VALUE_TYPE_FLOAT:
            v = value_make_float(*((double *)val));
            break;
        case VALUE_TYPE_STRING:
            v = value_make_string((char *)val);
            break;
        case VALUE_TYPE_VOID:
            v = value_make_void();
            break;
        case VALUE_TYPE_RES:	 /* These receive an already created value... */
        case VALUE_TYPE_ARRAY:
        case VALUE_TYPE_STRUCT:
        case VALUE_TYPE_FN:
        default:
            v = value_copy(val);
    }
    symtab_stack_add_global_variable(s, name, v);
    value_free(v);
}

/*
 * Register a variable in the interpreter context from data struct
 */
void register_variable_data(intend_state *s, register_var_data *vdata)
{
    register_variable(s, vdata->name, vdata->type, vdata->value);
}

/*
 * Register a variable in the interpreter context from array of data structs
 */
void register_variable_array(intend_state *s, register_var_data *varray)
{
    unsigned int i;

    sanity(varray);

    i = 0;
    while (varray[i].name) {
        register_variable_data(s, &varray[i]);
        i++;
    }
}

/*
 * Unregister a symbol (function or variable) from the interpreter context
 */
void unregister_symbol(intend_state *s, char *symbol)
{
    sanity(symbol);

	// Only delete if global table not freed already
	// otherwise it is done already, nothing to be done then...
    if(s->global_table) {
		symtab_stack_delete_global(s, symbol);
	}
}

/*
 * Unregister a function from the interpreter context from array of data structs
 */
void unregister_function_array(intend_state *s, register_func_data *farray)
{
    unsigned int i;

    sanity(farray);

    i = 0;
    while (farray[i].name) {
        unregister_symbol(s, farray[i].name);
        i++;
    }
}

/*
 * Unregister a variable from the interpreter context from array of data structs
 */
void unregister_variable_array(intend_state *s, register_var_data *varray)
{
    unsigned int i;

    sanity(varray);

    i = 0;
    while (varray[i].name) {
        unregister_symbol(s, varray[i].name);
        i++;
    }
}

