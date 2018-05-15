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
 * Intend C Memory management
 */

#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Allocate new value
 *
 * This function allocates storage for a new value of the
 * given type and returns a pointer to the new value.
 */
value *value_alloc(value_type type)
{
    value *val;

    val = oom(calloc(1, sizeof(value)));
    val->type = type;
    return val;
}

/*
 * Cleans a value
 *
 * This function frees the memory occupied by a value.
 * For an array, all its elements are freed before
 * the array itself is.
 */
void value_cleanup(value *val)
{
    symtab_entry **next;

    // Avoid null values
    if (!val) return;

    switch (val->type) {
        case VALUE_TYPE_VOID:
        case VALUE_TYPE_BOOL:
        case VALUE_TYPE_INT:
        case VALUE_TYPE_FLOAT:
            break;
        case VALUE_TYPE_STRING:
            free(STR_OF(val));
            break;
        case VALUE_TYPE_ARRAY:
            next = ARR_OF(val);
            if (next) {
                while (*next) {
                    if ((*next)->symbol) {
                        symtab_delete(ARRKEYS_OF(val), (*next)->symbol);
                    } else {
                        symtab_entry_free(*next);
                    }
                    next++;
                }
            }
            free(ARR_OF(val));
            symtab_free(ARRKEYS_OF(val));
            break;
        case VALUE_TYPE_STRUCT:
            symtab_free(val->value_u.struct_val);
            break;
        case VALUE_TYPE_FN:
            call_sig_free(FNSIG_OF(val));
            break;
        case VALUE_TYPE_RES:
            if (val->value_u.res_val) {
                --RESREF_OF(val);
                if (RESREF_OF(val) == 0) {
                    if (RESRELEASE_OF(val)) {
                        RESRELEASE_OF(val)(RES_OF(val));
                    }
                    free(val->value_u.res_val);
                }
            }
            break;
    }
    memset(val, 0, sizeof(value));
}

/*
 * Frees a value
 *
 * This function cleans the value memory and frees it
 * after.
 */
void value_free(value *val)
{
    // Avoid null values
    if (!val) return;

    value_cleanup(val);

    free(val);
}
