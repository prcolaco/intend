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
 * Intend C Constructor functions
 */

#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Make void value
 *
 * This function returns a new void value.
 */
value *value_make_void(void)
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_VOID);
    return copy;
}

/*
 * Make bool value
 *
 * This functions returns a new bool value. The value
 * contains "false" if the given C integer is 0 and
 * "true" otherwise.
 */
value *value_make_bool(int val)
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_BOOL);
    if (val) {
        BOOL_OF(copy) = 1;
    }
    return copy;
}

/*
 * Make int value
 *
 * This functions returns a new int value with the same
 * numerical value as the given C integer.
 */
value *value_make_int(int val)
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_INT);
    INT_OF(copy) = val;
    return copy;
}

/*
 * Make float value
 *
 * This function returns a new float value with the same
 * numerical value as the given C double-precision float.
 */
value *value_make_float(double val)
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_FLOAT);
    FLOAT_OF(copy) = val;
    return copy;
}

/*
 * Make string value
 *
 * This function returns a new string value with the same
 * content as the given C string. The value contains a
 * newly allocated copy of the string data.
 */
value *value_make_string(const char *str)
{
    int len, size;
    char *cstr;
    value *copy;

    if (str) {
        len = strlen(str);
    } else {
        len = 0;
    }

    copy = value_alloc(VALUE_TYPE_STRING);

    size = len < STRING_MIN_CHARS ? STRING_MIN_CHARS : len;
    cstr = oom(calloc(size + 1, 1));
    STRLEN_OF(copy) = len;
    STRSIZE_OF(copy) = size;
    STR_OF(copy) = cstr;

    if (len > 0) {
        memcpy(cstr, str, len + 1);
    }

    return copy;
}

/*
 * Make string from memory buffer
 *
 * This function returns a new string with the same contents as the
 * given memory buffer. In contrast to value_make_string(), this
 * function can create string with embedded 0 bytes.
 */
value *value_make_memstring(const void *buf, int len)
{
    int size;
    char *mem;
    value *copy;

    if (!buf || len == 0) {
        return value_make_string(NULL);
    }

    copy = value_alloc(VALUE_TYPE_STRING);

    size = len < STRING_MIN_CHARS ? STRING_MIN_CHARS : len;
    mem = oom(calloc(size + 1, 1));
    STRLEN_OF(copy) = len;
    STRSIZE_OF(copy) = size;
    STR_OF(copy) = mem;

    if (len > 0) {
        memcpy(mem, buf, len);
    }

    return copy;
}

/*
 * Make array value
 *
 * This functions returns a new array value representing
 * an empty array.
 */
value *value_make_array(void)
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_ARRAY);
    ARRKEYS_OF(copy) = symtab_alloc(5);
    return copy;
}

/*
 * Make struct value
 *
 * This function returns a new struct value with no fields.
 */
value *value_make_struct(void)
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_STRUCT);
    copy->value_u.struct_val = symtab_alloc(5);

    return copy;
}

/*
 * Make function value
 *
 * This function returns a new function value pointing to the given
 * signature.
 */
value *value_make_fn(void *sig)
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_FN);
    FNSIG_OF(copy) = call_sig_copy(sig);

    return copy;
}

/*
 * Make resource value
 */
value *value_make_resource(void *data, void (*release)(void *), void *(*get)(void *))
{
    value *copy;

    copy = value_alloc(VALUE_TYPE_RES);

    copy->value_u.res_val = oom(malloc(sizeof(value_res)));
    RESREF_OF(copy) = 1;
    RES_OF(copy)     = data;
    RESRELEASE_OF(copy)  = release;
    RESGET_OF(copy)      = get;

    return copy;
}
