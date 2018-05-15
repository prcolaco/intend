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
 * Intend C Runtime functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"

/*
 * Runtime functions to register
 */
static register_func_data rt_funcs[] = {
	{ "sizeof",			rt_size_of,		1,  "?",    'i' },
    { "typeof",			rt_type_of,		1,	"?",	's'	},
    { "classof",		rt_class_of,	1,	"?",	'?'	},
    { "is_void",		rt_is_void,		1,	"?",	'b'	},
    { "is_bool",		rt_is_bool,		1,	"?",	'b'	},
    { "is_int",			rt_is_int,		1,	"?",	'b'	},
    { "is_float",		rt_is_float,	1,	"?",	'b'	},
    { "is_string",		rt_is_string,	1,	"?",	'b'	},
    { "is_array",		rt_is_array,	1,	"?",	'b'	},
    { "is_struct",		rt_is_struct,	1,	"?",	'b'	},
    { "is_fn",			rt_is_fn,		1,	"?",	'b'	},
    { "is_resource",	rt_is_resource,	1,	"?",	'b'	},
    { "is_a",			rt_is_a,		2,	"?s",	'b'	},
    { "is_function",	rt_is_function,	1,	"s",	'b'	},
    { "is_var",			rt_is_var,		1,	"s",	'b'	},
    { "is_class",		rt_is_class,	1,	"s",	'b'	},
    { "is_local",		rt_is_local,	1,	"s",	'b'	},
    { "is_global",		rt_is_global,	1,	"s",	'b'	},
    { "set",			rt_set,			2,	"s?",	'b'	},
    { "get",			rt_get,			1,	"s",	'?'	},
    { "get_static",		rt_get_static,	2,	"ss",	'?'	},
    { "unset",			rt_unset,		0,	"",		'v'	},
    { "assert",			rt_assert,		0,	"",		'v'	},
    { "global",			rt_global,		0,	"",		'v'	},
    { "cast_to",		rt_cast_to,		2,	"?s",	'?'	},
    { "versions",		rt_versions,	0,	"",		'c'	},
    { "exit",			rt_exit,		1,	"i",	'v'	},

    /* list terminator */
    { NULL,			NULL,			0,	NULL,	0	}
};


/*
 * Initialize the Runtime functions
 */
void rt_init(intend_state *s)
{
    // Register functions
    register_function_array(s, rt_funcs);
}

/*
 * Return the size of the passed argument
 */
value *rt_size_of(intend_state *s, unsigned int argc, value **argv)
{
    int size;

    switch (argv[0]->type) {
        case VALUE_TYPE_VOID:
            size = 0;
            break;
        case VALUE_TYPE_BOOL:
            size = sizeof(int);
            break;
        case VALUE_TYPE_INT:
            size = sizeof(int);
            break;
        case VALUE_TYPE_FLOAT:
            size = sizeof(double);
            break;
        case VALUE_TYPE_STRING:
            size = STRLEN_OF(argv[0]);
            break;
        case VALUE_TYPE_ARRAY:
            size = ARRLEN_OF(argv[0]);
            break;
        case VALUE_TYPE_STRUCT:
            size = sizeof(void *);
            break;
        case VALUE_TYPE_FN:
            size = sizeof(void *);
            break;
        case VALUE_TYPE_RES:
            size = sizeof(void *);
            break;
        default:
            size = -1;
    }
    return value_make_int(size);
}

/*
 * Return type of expression
 *
 * This function returns a string representation of the type of
 * the first argument.
 */
value *rt_type_of(intend_state *s, unsigned int argc, value **argv)
{
    char *buf;
    switch (argv[0]->type) {
        case VALUE_TYPE_VOID:
            buf = "void";
            break;
        case VALUE_TYPE_BOOL:
            buf = "bool";
            break;
        case VALUE_TYPE_INT:
            buf = "int";
            break;
        case VALUE_TYPE_FLOAT:
            buf = "float";
            break;
        case VALUE_TYPE_STRING:
            buf = "string";
            break;
        case VALUE_TYPE_ARRAY:
            buf = "array";
            break;
        case VALUE_TYPE_STRUCT:
            buf = "struct";
            break;
        case VALUE_TYPE_FN:
            buf = "fn";
            break;
        case VALUE_TYPE_RES:
            buf = "resource";
            break;
        default:
            buf = "unknown";
    }
    return value_make_string(buf);
}

/*
 * Return class of value
 *
 * Returns the class of a struct value if it was instantiated from a
 * class definition and the "__class" member has not been overwritten
 * since. It returns void otherwise.
 */
value *rt_class_of(intend_state *s, unsigned int argc, value **argv)
{
    if (argv[0]->type != VALUE_TYPE_STRUCT) {
        return value_make_void();
    }
    return value_get_struct(argv[0], "__class");
}

/*
 * Check whether all arguments have given type
 */
static value *is_type(unsigned int argc, value **argv, unsigned int type)
{
    unsigned int i;

    for (i = 0; i < argc; i++) {
        if (argv[i]->type != type) {
            return value_make_bool(0);
        }
    }
    return value_make_bool(1);
}

/*
 * Check whether arguments are all voids
 */
value *rt_is_void(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_VOID);
}

/*
 * Check whether arguments are all bools
 */
value *rt_is_bool(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_BOOL);
}

/*
 * Check whether arguments are all ints
 */
value *rt_is_int(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_INT);
}

/*
 * Check whether arguments are all floats
 */
value *rt_is_float(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_FLOAT);
}

/*
 * Check whether arguments are all strings
 */
value *rt_is_string(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_STRING);
}

/*
 * Check whether arguments are all arrays
 */
value *rt_is_array(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_ARRAY);
}

/*
 * Check whether arguments are all structs
 */
value *rt_is_struct(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_STRUCT);
}

/*
 * Check whether arguments are all functions
 */
value *rt_is_fn(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_FN);
}

/*
 * Check whether arguments are all resources
 */
value *rt_is_resource(intend_state *s, unsigned int argc, value **argv)
{
    return is_type(argc, argv, VALUE_TYPE_RES);
}

/*
 * Check whether struct is of a given class  (including parent classes)
 */
static value *is_class(intend_state *s, value *val, const char *wanted)
{
    symtab_entry *entry;
    value *type;
    char *typename;
    int res = 0;

    type = value_get_struct(val, "__class");
    if (type->type != VALUE_TYPE_STRING) {
        value_free(type);
        return value_make_bool(0);
    }
    typename = STR_OF(type);

    while (typename) {
        entry = symtab_stack_lookup(s, typename);
        if (!entry || entry->type != SYMTAB_ENTRY_CLASS) {
            break;
        }
        if (strcmp(entry->symbol, wanted) == 0) {
            res = 1;
            break;
        }
        typename = entry->entry_u.cls.parent;
    }

    value_free(type);
    return value_make_bool(res);
}

/*
 * Return numeric type for type name
 */
static int nametype(const char *name)
{
    int wanted = 0;

    if (strcmp(name, "void") == 0) {
        wanted = VALUE_TYPE_VOID;
    } else if (strcmp(name, "bool") == 0) {
        wanted = VALUE_TYPE_BOOL;
    } else if (strcmp(name, "int") == 0) {
        wanted = VALUE_TYPE_INT;
    } else if (strcmp(name, "float") == 0) {
        wanted = VALUE_TYPE_FLOAT;
    } else if (strcmp(name, "string") == 0) {
        wanted = VALUE_TYPE_STRING;
    } else if (strcmp(name, "array") == 0) {
        wanted = VALUE_TYPE_ARRAY;
    } else if (strcmp(name, "struct") == 0) {
        wanted = VALUE_TYPE_STRUCT;
    } else if (strcmp(name, "fn") == 0) {
        wanted = VALUE_TYPE_FN;
    } else if (strcmp(name, "resource") == 0) {
        wanted = VALUE_TYPE_RES;
    }
    return wanted;
}

/*
 * Check whether argument has specified type
 *
 * The type is given as a string identifying the desired type.
 */
value *rt_is_a(intend_state *s, unsigned int argc, value **argv)
{
    const char *name = STR_OF(argv[1]);
    unsigned int wanted = 0;

    if (!value_str_compat(argv[1])) {
        return value_make_bool(0);
    }

    wanted = nametype(name);
    if (wanted == 0 && argv[0]->type == VALUE_TYPE_STRUCT) {
        return is_class(s, argv[0], name);
    }
    return value_make_bool(argv[0]->type == wanted);
}

/*
 * Check existence of symtab entry of specific type
 */
static value *isdefined(intend_state *s, value *val, unsigned int wanted)
{
    char *name = STR_OF(val);
    symtab_entry *entry;

    if (!value_str_compat(val)) {
        return value_make_bool(0);
    }

    entry = symtab_stack_lookup(s, name);
    return value_make_bool(entry && entry->type == wanted);
}

/*
 * Check existence of function
 *
 * This function returns true if the given function name exists in
 * the current symbol table scope.
 */
value *rt_is_function(intend_state *s, unsigned int argc, value **argv)
{
    return isdefined(s, argv[0], SYMTAB_ENTRY_FUNCTION);
}

/*
 * Check existence of variable
 *
 * This function returns true if the given variable name exists in
 * the current symbol table scope.
 */
value *rt_is_var(intend_state *s, unsigned int argc, value **argv)
{
    return isdefined(s, argv[0], SYMTAB_ENTRY_VAR);
}

/*
 * Check existence of class
 *
 * This function returns true if the given class name exists in
 * the current symbol table scope.
 */
value *rt_is_class(intend_state *s, unsigned int argc, value **argv)
{
    return isdefined(s, argv[0], SYMTAB_ENTRY_CLASS);
}

/*
 * Check for local symbol
 *
 * Returns true if the given name is from the local scope.
 */
value *rt_is_local(intend_state *s, unsigned int argc, value **argv)
{
    char *name = STR_OF(argv[0]);

    if (!value_str_compat(argv[0])) {
        return value_make_bool(0);
    }
    return value_make_bool(symtab_stack_local(s, name));
}

/*
 * Check for global symbol
 */
value *rt_is_global(intend_state *s, unsigned int argc, value **argv)
{
    char *name = STR_OF(argv[0]);
    symtab_entry *entry;

    if (!value_str_compat(argv[0])) {
        return value_make_bool(0);
    }
    entry = symtab_stack_lookup(s, name);
    return value_make_bool(entry && !symtab_stack_local(s, name));
}

/*
 * Define a variable
 *
 * This function uses its first parameter as a variable names and
 * assigns the value of the second parameter to the new variable.
 * It returns true on success and false on failure.
 */
value *rt_set(intend_state *s, unsigned int argc, value **argv)
{
    char *name = STR_OF(argv[0]);

    if (!value_str_compat(argv[0])) {
        return value_make_bool(0);
    }

    if (argv[1]->type != VALUE_TYPE_FN) {
        symtab_stack_add_variable(s, name, argv[1]);
    } else {
        symtab_stack_add_function(s, name, FNSIG_OF(argv[1]));
    }
    return value_make_bool(1);
}

/*
 * Variable reference
 *
 * This function takes the name of a variable as argument and returns
 * the value of the variable if it exists. If not, a void value is
 * returned.
 */
value *rt_get(intend_state *s, unsigned int argc, value **argv)
{
    char *name = STR_OF(argv[0]);
    symtab_entry *entry;

    if (!value_str_compat(argv[0])) {
        return value_make_void();
    }

    entry = symtab_stack_lookup(s, name);
    if (!entry || entry->type == SYMTAB_ENTRY_CLASS) {
        return value_make_void();
    }
    if (entry->type == SYMTAB_ENTRY_VAR) {
        return value_copy(&entry->entry_u.var);
    } else {
        return value_make_fn(&(entry->entry_u.fnc.sigs[0]));
    }
}

/*
 * Unset variable or function
 *
 * Removes a variable or function from the current symbol table. In
 * the case of variables, the memory used by their values will also
 * be freed.
 */
value *rt_unset(intend_state *s, unsigned int argc, value **argv)
{
    unsigned int i;

    for (i = 0; i < argc; i++) {
        value_cast_inplace(s, &argv[i], VALUE_TYPE_STRING);
        if (value_str_compat(argv[i])) {
            symtab_stack_delete(s, STR_OF(argv[i]));
        }
    }

    return value_make_void();
}

/*
 * Debugging assertion
 *
 * Terminates the program if one the arguments evaluates to false.
 */
value *rt_assert(intend_state *s, unsigned int argc, value **argv)
{
    unsigned int i;

    for (i = 0; i < argc; i++) {
        value_cast_inplace(s, &argv[i], VALUE_TYPE_BOOL);
        if (!BOOL_OF(argv[i])) {
            fatal(s, "assertion failure");
            break;
        }
    }
    return value_make_void();
}

/*
 * Make symbols global
 *
 * This function treats its arguments as variable names and
 * adds them to the global scope.
 */
value *rt_global(intend_state *s, unsigned int argc, value **argv)
{
    symtab_entry *entry;
    unsigned int i;

    for (i = 0; i < argc; i++) {
        value_cast_inplace(s, &argv[i], VALUE_TYPE_STRING);
        if (STR_OF(argv[i])) {
            entry = symtab_stack_lookup(s, STR_OF(argv[i]));
            if (entry && entry->type == SYMTAB_ENTRY_VAR) {
                symtab_stack_add_global_variable(s, entry->symbol, &entry->entry_u.var);
            }
        }
    }
    return value_make_void();
}

/*
 * Get static class member by name(s)
 */
value *rt_get_static(intend_state *s, unsigned int argc, value **argv)
{
    char *class = STR_OF(argv[0]);
    char *name = STR_OF(argv[1]);
    symtab_entry *entry;
    expr ex;

    if (!value_str_compat(argv[0]) || !value_str_compat(argv[1])) {
        return value_make_void();
    }

    entry = symtab_stack_lookup(s, class);
    if (!entry || entry->type != SYMTAB_ENTRY_CLASS) {
        return value_make_void();
    }

    ex.type  = EXPR_STATIC_REF;
    ex.tname = class;
    ex.name  = name;

    return eval_static_ref(s, &ex);
}

/*
 * Cast to given type
 */
value *rt_cast_to(intend_state *s, unsigned int argc, value **argv)
{
    const char *name = STR_OF(argv[1]);
    int wanted;

    wanted = nametype(name);
    if (wanted == 0) {
        fatal(s, "unknown type name `%s'", name);
        return value_make_void();
    }
    return value_cast(s, argv[0], wanted);
}

/*
 * Get language and library versions
 */
value *rt_versions(intend_state *s, unsigned int argc, value **argv)
{
    value *res, *elem;
    char ver[] = VERSION;
    int vm, vn, vr;

    if (sscanf(ver, "%i.%i.%i", &vm, &vn, &vr)) {
        res = value_make_struct();

        elem = value_make_string(ver);
        value_set_struct(res, "version", elem);
        value_free(elem);

        elem = value_make_int(vm);
        value_set_struct(res, "major", elem);
        value_free(elem);

        elem = value_make_int(vn);
        value_set_struct(res, "minor", elem);
        value_free(elem);

        elem = value_make_int(vr);
        value_set_struct(res, "release", elem);
        value_free(elem);
    } else {
        res = value_make_void();
    }

    return res;
}

/*
 * Terminate program
 *
 * Terminates the program and returns its int argument as
 * the program's exit status.
 */
value *rt_exit(intend_state *s, unsigned int argc, value **argv)
{
    int status = INT_OF(argv[0]);

    s->exit_flag  = 1;
    s->exit_value = status;
    return value_make_void();
}

