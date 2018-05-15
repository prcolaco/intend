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
 * Intend C Value casting
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Cast value to bool
 *
 * This function casts the given value to a bool value.
 * Non-zero numeric and non-empty string and array values
 * are cast to "true".
 */
static value *cast_to_bool(value *val)
{
    value *res;

    res = value_alloc(VALUE_TYPE_BOOL);

    switch (val->type) {
        case VALUE_TYPE_VOID:
            break;
        case VALUE_TYPE_BOOL:
            BOOL_OF(res) = BOOL_OF(val);
            break;
        case VALUE_TYPE_INT:
            BOOL_OF(res) = INT_OF(val);
            break;
        case VALUE_TYPE_FLOAT:
            BOOL_OF(res) = FLOAT_OF(val);
            break;
        case VALUE_TYPE_STRING:
            BOOL_OF(res) = STRLEN_OF(val);
            break;
        case VALUE_TYPE_ARRAY:
            BOOL_OF(res) = ARRLEN_OF(val);
            break;
        case VALUE_TYPE_STRUCT:
            BOOL_OF(res) = symtab_num_entries(val->value_u.struct_val);
            break;
        case VALUE_TYPE_FN:
            BOOL_OF(res) = 1;
            break;
        case VALUE_TYPE_RES:
            BOOL_OF(res) = 1;
            break;
    }
    if (BOOL_OF(res) != 0) {
        BOOL_OF(res) = 1;
    }
    return res;
}

/*
 * Cast value to int
 *
 * This function casts the given value to an int value.
 * Numeric values are cast to their int equivalent or
 * clipped. Strings are interpreted as numbers if possible.
 * Arrays are cast to their length.
 */
static value *cast_to_int(value *val)
{
    value *res;
    int copy = 0;

    res = value_alloc(VALUE_TYPE_INT);

    switch (val->type) {
        case VALUE_TYPE_VOID:
            break;
        case VALUE_TYPE_BOOL:
            copy = BOOL_OF(val);
            break;
        case VALUE_TYPE_INT:
            copy = INT_OF(val);
            break;
        case VALUE_TYPE_FLOAT:
            copy = FLOAT_OF(val);
            break;
        case VALUE_TYPE_STRING:
            if (STRLEN_OF(val)) {
                copy = strtol(STR_OF(val), NULL, 0);
            }
            break;
        case VALUE_TYPE_ARRAY:
            copy = ARRLEN_OF(val);
            break;
        case VALUE_TYPE_STRUCT:
            copy = symtab_num_entries(val->value_u.struct_val);
            break;
        case VALUE_TYPE_FN:
            copy = 1;
            break;
        case VALUE_TYPE_RES:
            copy = 1;
            break;
    }
    INT_OF(res) = copy;
    return res;
}

/*
 * Cast value to float
 *
 * This function casts the given value to a float value.
 * Numeric values are cast to their float equivalent or
 * clipped. Strings are interpreted as numbers if possible.
 * Arrays are cast to their length.
 */
static value *cast_to_float(value *val)
{
    value *res;
    double copy = 0.0;

    res = value_alloc(VALUE_TYPE_FLOAT);

    switch (val->type) {
        case VALUE_TYPE_VOID:
            break;
        case VALUE_TYPE_BOOL:
            copy = BOOL_OF(val);
            break;
        case VALUE_TYPE_INT:
            copy = INT_OF(val);
            break;
        case VALUE_TYPE_FLOAT:
            copy = FLOAT_OF(val);
            break;
        case VALUE_TYPE_STRING:
            if (STRLEN_OF(val)) {
                copy = strtod(STR_OF(val), NULL);
            }
            break;
        case VALUE_TYPE_ARRAY:
            copy = ARRLEN_OF(val);
            break;
        case VALUE_TYPE_STRUCT:
            copy = symtab_num_entries(val->value_u.struct_val);
            break;
        case VALUE_TYPE_FN:
            copy = 1.0;
            break;
        case VALUE_TYPE_RES:
            copy = 1.0;
            break;
    }
    FLOAT_OF(res) = copy;
    return res;
}

/*
 * Cast an array to string
 */
static value *cast_array_to_string(value *val)
{
    int max = ARRLEN_OF(val);
    int i, len, curr = 0;
    char *buf = NULL;
    value *res, *elem;

    for (i = 0; i < max; i++) {
        elem = value_get_array(val, i);
        value_cast_inplace(NULL, &elem, VALUE_TYPE_STRING);
        len = STRLEN_OF(elem);
        if (!buf) {
            curr += len;
            buf = oom(malloc(curr + 1));
            buf[0] = '\0';
        } else {
            curr += len;
            buf = oom(realloc(buf, curr + 1));
        }
        if (len) {
            strcat(buf, STR_OF(elem));
        }
        value_free(elem);
    }
    res = value_make_string(buf);
    free(buf);
    return res;
}

/*
 * Cast value to string
 *
 * This function casts the given value to a string value.
 * Numbers are cast into a string representation, arrays
 * are cast into the string "Array".
 */
static value *cast_to_string(value *val)
{
    value *res = NULL;
    char buf[64];

    switch (val->type) {
        case VALUE_TYPE_VOID:
            res = value_make_string(NULL);
            break;
        case VALUE_TYPE_BOOL:
            if (BOOL_OF(val) != 0) {
                res = value_make_string("1");
            } else {
                res = value_make_string(NULL);
            }
            break;
        case VALUE_TYPE_INT:
            sprintf(buf, "%i", INT_OF(val));
            res = value_make_string(buf);
            break;
        case VALUE_TYPE_FLOAT:
            sprintf(buf, "%0.10g", FLOAT_OF(val));
            res = value_make_string(buf);
            break;
        case VALUE_TYPE_STRING:
            res = value_copy(val);
            break;
        case VALUE_TYPE_ARRAY:
            res = cast_array_to_string(val);
            break;
        case VALUE_TYPE_STRUCT:
            res = value_make_string("struct");
            break;
        case VALUE_TYPE_FN:
            res = value_make_string("function");
            break;
        case VALUE_TYPE_RES:
            res = value_make_string("resource");
            break;
    }
    return res;
}

/*
 * Cast a string to an array
 */
static value *cast_string_to_array(value *val)
{
    char *str = STR_OF(val);
    int max = strlen(STR_OF(val));
    int i;
    char buf[] = " ";
    value *res, *elem;

    res = value_make_array();
    for (i = 0; i < max; i++) {
        buf[0] = str[i];
        elem = value_make_string(buf);
        value_add_to_array(res, elem);
        value_free(elem);
    }
    return res;
}

/*
 * Cast value to array
 *
 * This function casts the given value to an array value.
 * Non-array values are cast into a one-element array
 * containing a copy of the original value.
 */
static value *cast_to_array(value *val)
{
    value *res;

    if (val->type == VALUE_TYPE_ARRAY) {
        res = value_copy(val);
    } else if (val->type == VALUE_TYPE_STRING) {
        res = cast_string_to_array(val);
    } else {
        res = value_alloc(VALUE_TYPE_ARRAY);
        ARRKEYS_OF(res) = symtab_alloc(5);
        value_add_to_array(res, val);
    }
    return res;
}

/*
 * Cast value to struct
 *
 * This function casts the given value to a struct value.
 * Non-struct values are cast into a one-field struct with
 * the original type as the field name.
 */
static value *cast_to_struct(value *val)
{
    value *res;

    if (val->type == VALUE_TYPE_STRUCT) {
        res = value_copy(val);
    } else {
        res = value_make_struct();
        value_set_struct(res, "value", val);
    }
    return res;
}

/*
 * Cast value to function
 *
 * Casting non-function values to function is a fatal error.
 */
static value *cast_to_fn(intend_state *s, value *val)
{
    if (val->type != VALUE_TYPE_FN) {
        fatal(s, "cannot cast `%s' to `fn'", call_typename(val->type));
    }
    return value_copy(val);
}

/*
 * Cast value to resource
 *
 * Casting non-resource values to resource is a fatal error.
 */
static value *cast_to_res(intend_state *s, value *val)
{
    if (val->type != VALUE_TYPE_RES) {
        fatal(s, "cannot cast `%s' to `resource'", call_typename(val->type));
    }
    return value_copy(val);
}

/*
 * Cast value to type
 *
 * This function attempts to cast the given value to the
 * given type. The result of the cast or NULL is returned.
 */
value *value_cast(intend_state *s, value *val, value_type type)
{
    value *res = NULL;

    sanity(val);

    switch (type) {
        case VALUE_TYPE_VOID:
            res = value_make_void();
            break;
        case VALUE_TYPE_BOOL:
            res = cast_to_bool(val);
            break;
        case VALUE_TYPE_INT:
            res = cast_to_int(val);
            break;
        case VALUE_TYPE_FLOAT:
            res = cast_to_float(val);
            break;
        case VALUE_TYPE_STRING:
            res = cast_to_string(val);
            break;
        case VALUE_TYPE_ARRAY:
            res = cast_to_array(val);
            break;
        case VALUE_TYPE_STRUCT:
            res = cast_to_struct(val);
            break;
        case VALUE_TYPE_FN:
            res = cast_to_fn(s, val);
            break;
        case VALUE_TYPE_RES:
            res = cast_to_res(s, val);
            break;
    }
    return res;
}

/*
 * Cast value inplace
 *
 * This function casts a value inplace, overwriting the original
 * value.
 */
void value_cast_inplace(intend_state *s, value **val, value_type type)
{
    value *res;

    sanity(val && *val);

    if ((*val)->type == type) {
        return;
    }

    res = value_cast(s, *val, type);
    value_free(*val);
    *val = res;
}

/*
 * Check compatibility with C string functions
 *
 * This function returns true if the pointer from a string value
 * can be passed into a C function that expects a char pointer.
 * Strings with embedded \0 characters are not compatible.
 */
int value_str_compat(const value *val)
{
    void *data;
    int len;

    if (!val || val->type != VALUE_TYPE_STRING) {
        return 0;
    }

    data = STR_OF(val);
    len  = STRLEN_OF(val);

    return (len && memchr(data, 0, len) == NULL);
}
