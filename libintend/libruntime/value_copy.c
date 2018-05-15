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
 * Intend C Value copy functions
 */

#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Copy array value
 *
 * This functions returns a full recursive copy of the
 * given input array.
 */
static value *copy_array(value *copy, value *val)
{
    int size  = ARRSIZE_OF(val);
    symtab_entry **next = ARR_OF(val);

    if (!ARRKEYS_OF(copy))
        ARRKEYS_OF(copy) = symtab_alloc(5);

    if (next) {
        while (*next) {
            if ((*next)->symbol) {
                value_add_to_key_array(copy, (*next)->symbol, &(*next)->entry_u.var);
            } else {
                value_add_to_array(copy, &(*next)->entry_u.var);
            }
            next++;
        }
    }
    return copy;
}

/*
 * Copy struct value
 *
 * This functions returns a full recursive copy of the
 * given input struct.
 */
static value *copy_struct(value *copy, value *val)
{
    symtab *nsym, *sym = STRUCT_OF(val);

    nsym = symtab_copy(sym);
    STRUCT_OF(copy) = nsym;

    return copy;
}

/*
 * Copy resource value
 *
 * Increases the reference count of the original resource
 * structure.
 */
value *copy_resource(value *copy, value *val)
{
    copy->value_u.res_val = val->value_u.res_val;
    ++RESREF_OF(copy);

    return copy;
}

/*
 * Copy value to pre-allocated value
 *
 * This function makes a recursive copy of the input value
 * to a pre-allocated destination.
 */
value *value_copy_to(value *copy, value *val)
{
    int len;
    int size;
    char *str;

    sanity(copy && val);

    if (copy->type != val->type || val->type > VALUE_TYPE_STRING) {
        value_cleanup(copy);
        copy->type = val->type;
    }

    switch (val->type) {
        case VALUE_TYPE_VOID:
        case VALUE_TYPE_BOOL:
        case VALUE_TYPE_INT:
        case VALUE_TYPE_FLOAT:
            *copy = *val;
            break;
        case VALUE_TYPE_STRING:
            // Get the dimensions of source
            len = STRLEN_OF(val);
            size = STRSIZE_OF(val);
            // Get the string of copy
            str = STR_OF(copy);
            // Alloc or realloc if size does not match
            if (!str) {
                str = oom(malloc(size + 1));
                STR_OF(copy) = str;
            } else if (STRSIZE_OF(copy) != size) {
                str = oom(realloc(str, size + 1));
                STR_OF(copy) = str;
            }
            // Clear the string
            memset(str, 0, size + 1);
            // Copy the string from source if any
            if (len > 0) {
                memcpy(str, STR_OF(val), len);
            }
            // Set the destination dimensions
            STRLEN_OF(copy) = len;
            STRSIZE_OF(copy) = size;
            break;
        case VALUE_TYPE_ARRAY:
            copy_array(copy, val);
            break;
        case VALUE_TYPE_STRUCT:
            copy_struct(copy, val);
            break;
        case VALUE_TYPE_FN:
            FNSIG_OF(copy) = call_sig_copy(FNSIG_OF(val));
            break;
        case VALUE_TYPE_RES:
            copy_resource(copy, val);
            break;
    }
    return copy;
}

/*
 * Copy value
 *
 * This function makes a recursive copy of the input value.
 */
value *value_copy(value *val)
{
    value *copy = NULL;

    sanity(val);

    copy = value_alloc(val->type);
    return value_copy_to(copy, val);
}
