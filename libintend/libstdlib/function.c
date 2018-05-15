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
 * Intend C Functions on functions
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "stdlib.h"

/*
 * Functions on functions to register
 */
static register_func_data fn_funcs[] = {
    /* functions on functions */
    { "is_builtin",			fn_is_builtin,			1,	"p",	'b'	},
    { "is_userdef",			fn_is_userdef,			1,	"p",	'b'	},
    { "function_name",		fn_name,				1,	"p",	's'	},
    { "call",				fn_call,				1,	"p",	'?'	},
    { "call_array",			fn_call_array,			2,	"pa",	'?'	},
    { "call_method",		fn_call_method,			2,	"pc",	'?'	},
    { "call_method_array",	fn_call_method_array,	3,	"pca",	'?'	},
    { "prototype",			fn_prototype,			1,	"p",	'c'	},
    { "map",				fn_map,					2,	"pa",	'a'	},
    { "filter",				fn_filter,				2,	"pa",	'a'	},
    { "foldl",				fn_foldl,				3,	"p?a",	'?'	},
    { "foldr",				fn_foldr,				3,	"pa?",	'?'	},
    { "take_while",			fn_take_while,			2,	"pa",	'a'	},
    { "drop_while",			fn_drop_while,			2,	"pa",	'a'	},

    /* list terminator */
    { NULL,					NULL,					0,	NULL,	0	}
};

/*
 * Initialize the Functions on functions
 */
void fn_init(intend_state *s)
{
    // Register functions
    register_function_array(s, fn_funcs);
}

/*
 * Call function by reference
 */
value *fn_call(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *res;

    symtab_stack_enter(s);
    if (argc > 1) {
        res = call_function(s, sig, argc - 1, argv + 1);
    } else {
        res = call_function(s, sig, 0, NULL);
    }
    symtab_stack_leave(s);

    return res;
}

/*
 * Call function by reference and array of arguments
 */
value *fn_call_array(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    int uargc      = ARRLEN_OF(argv[1]);
    value **uargv;
    value *res;
    int i;

    symtab_stack_enter(s);
    if (uargc > 0) {
        uargv = oom(calloc(uargc, sizeof(value *)));
        for (i = 0; i < uargc; i++) {
            uargv[i] = &ARR_OF(argv[1])[i]->entry_u.var;
        }
        res = call_function(s, sig, uargc, uargv);
        free(uargv);
    } else {
        res = call_function(s, sig, 0, NULL);
    }
    symtab_stack_leave(s);

    return res;
}

/*
 * Get type description struct
 */
static value *typestruct(char typechar)
{
    const char *name;
    value *typename, *typeforce, *elem;

    if (typechar == '?' || typechar == '*') {
        typename = value_make_string("mixed");
    } else {
        name = call_typename(call_chartype(tolower(typechar)));
        typename = value_make_string(name);
    }
    typeforce = value_make_bool(isupper(typechar));

    elem = value_make_struct();
    value_set_struct(elem, "type", typename);
    value_set_struct(elem, "force", typeforce);

    value_free(typeforce);
    value_free(typename);

    return elem;
}

/*
 * Append type description to array
 */
static void appendtype(value *arr, char typechar)
{
    value *elem;

    elem = typestruct(typechar);
    value_add_to_array(arr, elem);
    value_free(elem);
}

/*
 * Get function prototype
 *
 * Returns an array that lists first the return type of the given
 * function, then the argument types. Each individual type is
 * given as a struct containing the fields "type" and "forced". The
 * former is the name of the expected type, the latter is true when
 * the type of the argument will be enforced.
 */
value *fn_prototype(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    unsigned int i;
    char *next;
    value *ret, *args, *res;

    ret = typestruct(sig->rettype);

    args = value_make_array();
    next = sig->proto;
    for (i = 0; i < sig->args; i++) {
        appendtype(args, *next++);
    }

    res = value_make_struct();
    value_set_struct(res, "ret", ret);
    value_set_struct(res, "args", args);

    value_free(args);
    value_free(ret);
    return res;
}

/*
 * Map function over array
 */
value *fn_map(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *array   = argv[1];
    symtab_entry **data   = ARR_OF(array);
    int len        = ARRLEN_OF(array);
    value *res, *elem;
    int i;

    res = value_make_array();
    for (i = 0; i < len; i++) {
        argv[1] = &data[i]->entry_u.var;
        symtab_stack_enter(s);
        elem = call_function(s, sig, argc - 1, argv + 1);
        symtab_stack_leave(s);
        value_add_to_array(res, elem);
        value_free(elem);
    }
    argv[1] = array;

    return res;
}

/*
 * Filter array with function
 */
value *fn_filter(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *array   = argv[1];
    symtab_entry **data   = ARR_OF(array);
    int len        = ARRLEN_OF(array);
    value *res, *test;
    int i;

    res = value_make_array();
    for (i = 0; i < len; i++) {
        argv[1] = &data[i]->entry_u.var;
        symtab_stack_enter(s);
        test = call_function(s, sig, argc - 1, argv + 1);
        symtab_stack_leave(s);
        value_cast_inplace(s, &test, VALUE_TYPE_BOOL);
        if (BOOL_OF(test)) {
            value_add_to_array(res, &data[i]->entry_u.var);
        }
        value_free(test);
    }
    argv[1] = array;

    return res;
}

/*
 * Fold array from left
 */
value *fn_foldl(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *init    = argv[1];
    value *array   = argv[2];
    symtab_entry **data   = ARR_OF(array);
    int len        = ARRLEN_OF(array);
    value *temp    = value_copy(init);
    value *step;
    int i;

    for (i = 0; i < len; i++) {
        argv[1] = temp;
        argv[2] = &data[i]->entry_u.var;
        symtab_stack_enter(s);
        step = call_function(s, sig, argc - 1, argv + 1);
        symtab_stack_leave(s);
        value_free(temp);
        temp = step;
    }
    argv[1] = init;
    argv[2] = array;

    return temp;
}

/*
 * Fold array from right
 */
value *fn_foldr(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *array   = argv[1];
    symtab_entry **data   = ARR_OF(array);
    int len        = ARRLEN_OF(array);
    value *init    = argv[2];
    value *temp    = value_copy(init);
    value *step;
    int i;

    for (i = len - 1; i >= 0; i--) {
        argv[1] = &data[i]->entry_u.var;
        argv[2] = temp;
        symtab_stack_enter(s);
        step = call_function(s, sig, argc - 1, argv + 1);
        symtab_stack_leave(s);
        value_free(temp);
        temp = step;
    }
    argv[1] = array;
    argv[2] = init;

    return temp;
}

/*
 * Take elements from array while condition is true
 */
value *fn_take_while(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *array   = argv[1];
    symtab_entry **data   = ARR_OF(array);
    int len        = ARRLEN_OF(array);
    int i, flag;
    value *check, *res;

    res = value_make_array();
    for (i = 0; i < len; i++) {
        argv[1] = &data[i]->entry_u.var;
        symtab_stack_enter(s);
        check = call_function(s, sig, argc - 1, argv + 1);
        symtab_stack_leave(s);
        value_cast_inplace(s, &check, VALUE_TYPE_BOOL);
        flag = BOOL_OF(check);
        value_free(check);
        if (flag) {
            value_add_to_array(res, &data[i]->entry_u.var);
        } else {
            break;
        }
    }
    argv[1] = array;

    return res;
}

/*
 * Drop elements from array while condition is true
 */
value *fn_drop_while(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *array   = argv[1];
    symtab_entry **data   = ARR_OF(array);
    int len        = ARRLEN_OF(array);
    int i, flag;
    value *check, *res;

    for (i = 0; i < len; i++) {
        argv[1] = &data[i]->entry_u.var;
        symtab_stack_enter(s);
        check = call_function(s, sig, argc - 1, argv + 1);
        symtab_stack_leave(s);
        value_cast_inplace(s, &check, VALUE_TYPE_BOOL);
        flag = BOOL_OF(check);
        value_free(check);
        if (!flag) {
            break;
        }
    }
    argv[1] = array;

    res = value_make_array();
    for (; i < len; i++) {
        value_add_to_array(res, &data[i]->entry_u.var);
    }

    return res;
}

/*
 * Call method by name and struct, arguments as varargs
 */
value *fn_call_method(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    value *res;

    symtab_stack_enter(s);
    symtab_stack_add_variable(s, "this", argv[1]);
    res = call_function(s, sig, argc - 2, argv + 2);
    symtab_stack_leave(s);

    return res;
}

/*
 * Call method by name and struct, arguments as array
 */
value *fn_call_method_array(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);
    int     uargc  = ARRLEN_OF(argv[2]);
    value **uargv;
    value *res;
    int i;

    symtab_stack_enter(s);
    symtab_stack_add_variable(s, "this", argv[1]);
    if (uargc > 0) {
        uargv = oom(calloc(uargc, sizeof(value *)));
        for (i = 0; i < uargc; i++) {
            uargv[i] = &ARR_OF(argv[2])[i]->entry_u.var;
        }
        res = call_function(s, sig, uargc, uargv);
        free(uargv);
    } else {
        res = call_function(s, sig, 0, NULL);
    }
    symtab_stack_leave(s);

    return res;
}

/*
 * Check whether function is a builtin function
 */
value *fn_is_builtin(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);

    return value_make_bool(sig->type == FUNCTION_TYPE_BUILTIN);
}

/*
 * Check whether function is a user-defined function
 */
value *fn_is_userdef(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);

    return value_make_bool(sig->type == FUNCTION_TYPE_USERDEF);
}

/*
 * Return function name of function value
 */
value *fn_name(intend_state *s, unsigned int argc, value **argv)
{
    signature *sig = FNSIG_OF(argv[0]);

    return value_make_string(sig->name);
}
