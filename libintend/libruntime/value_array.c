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
 * Intend C Array functions
 */

#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Append value to end of array with a given key
 *
 * This function appends a copy of the given value to the
 * end of the array and assigns it the given key.
 */
void value_add_to_key_array(value *arr, char *pos, value *val)
{
    symtab_entry *entry;
    symtab_entry **entries;

    sanity(arr && val && arr->type == VALUE_TYPE_ARRAY);

    if (ARRSIZE_OF(arr) <= ARRLEN_OF(arr)) {
        ARRSIZE_OF(arr) += ARRAY_GROWTH;
        entries = oom(realloc(ARR_OF(arr), (ARRSIZE_OF(arr) + 1) * sizeof(symtab_entry *)));
        ARR_OF(arr) = entries;
    } else {
        entries = ARR_OF(arr);
    }

    // Add value to keys symbol table
    entry = symtab_lookup(ARRKEYS_OF(arr), pos);
    if (entry) {
        value_cleanup(&entry->entry_u.var);
        value_copy_to(&entry->entry_u.var, val);
    } else {
        // Add to symbol table
        entry = symtab_add_variable(ARRKEYS_OF(arr), pos, val);
        // Add to the data array
        entries[ARRLEN_OF(arr)++] = entry;
        // Add NULL sentinel
        entries[ARRLEN_OF(arr)] = NULL;
    }
}

/*
 * Append value to end of array
 *
 * This function appends a copy of the given value to the
 * end of the array.
 */
void value_add_to_array(value *arr, value *val)
{
    symtab_entry *entry;
    symtab_entry **entries;

    sanity(arr && val && arr->type == VALUE_TYPE_ARRAY);

    if (ARRSIZE_OF(arr) <= ARRLEN_OF(arr)) {
        ARRSIZE_OF(arr) += ARRAY_GROWTH;
        entries = oom(realloc(ARR_OF(arr), (ARRSIZE_OF(arr) + 1) * sizeof(symtab_entry *)));
        ARR_OF(arr) = entries;
    } else {
        entries = ARR_OF(arr);
    }

    // Add to the data array
    entry = symtab_entry_alloc();
    entry->symbol = NULL;
    entry->type = SYMTAB_ENTRY_VAR;
    value_copy_to(&entry->entry_u.var, val);
    entries[ARRLEN_OF(arr)++] = entry;

    /*
     * Maintain a NULL sentinel at the end of the pointer array.
     * This makes iteration over the array entries easy.
     */
    entries[ARRLEN_OF(arr)] = NULL;
}

/*
 * Set value at specific array key
 *
 * This function puts a copy of the given value at a specified
 * key in the array. If the key does not exist then it will be
 * added at the end of the array.
 */
void value_set_key_array(value *arr, char *pos, value *val)
{
    symtab_entry *entry;
    value *copy;

    sanity(arr && val && arr->type == VALUE_TYPE_ARRAY);

    entry = symtab_lookup(ARRKEYS_OF(arr), pos);

    if (entry) {
        value_cleanup(&entry->entry_u.var);
        value_copy_to(&entry->entry_u.var, val);
    } else {
        value_add_to_key_array(arr, pos, val);
    }
}

/*
 * Set value at specific array index
 *
 * This function puts a copy of the given value at a specified
 * index in the array. If the index is higher then the current
 * maximum index, the missing indices are filled in with void
 * values.
 */
void value_set_array(value *arr, int pos, value *val)
{
    value *copy;
    symtab_entry *entry;

    sanity(arr && val && arr->type == VALUE_TYPE_ARRAY);

    if (pos < 0) {
        pos = ARRLEN_OF(arr) + pos;
    }
    if (pos < 0) {
        pos = 0;
    }

    if (pos > ARRLEN_OF(arr)) {
        value *fill = value_make_void();
        while (pos > ARRLEN_OF(arr)) {
            value_add_to_array(arr, fill);
        }
        value_free(fill);
    }

    if (pos == ARRLEN_OF(arr)) {
        value_add_to_array(arr, val);
    } else {
        entry = ARR_OF(arr)[pos];
        value_cleanup(&entry->entry_u.var);
        value_copy_to(&entry->entry_u.var, val);
    }
}

/*
 * Get value from specific array key
 *
 * This functions returns a copy of the value stored at
 * a specific array key. Returns a void value if the
 * key is not found in the array.
 */
value *value_get_key_array(value *arr, char *pos)
{
    symtab_entry *entry;

    sanity(arr && arr->type == VALUE_TYPE_ARRAY);

    entry = symtab_lookup(ARRKEYS_OF(arr), pos);

    if (entry) {
        return value_copy(&entry->entry_u.var);
    } else {
        return value_make_void();
    }
}

/*
 * Get value from specific array index
 *
 * This functions returns a copy of the value stored at
 * a specific array index. Returns a void value if the
 * index is out of bounds for the array.
 */
value *value_get_array(value *arr, int pos)
{
    symtab_entry *entry;

    sanity(arr && arr->type == VALUE_TYPE_ARRAY);

    if (pos < 0) {
        pos = ARRLEN_OF(arr) + pos;
    }

    if (pos < 0 || pos > (ARRLEN_OF(arr) - 1)) {
        return value_make_void();
    }

    entry = ARR_OF(arr)[pos];
    return value_copy(&entry->entry_u.var);
}

/*
 * Delete element from array
 *
 * This function removes the array element at the given index.
 * The element is freed and replaced by a void value.
 */
void value_delete_key_array(value *arr, char *pos)
{
    symtab_entry *entry;

    sanity(arr && arr->type == VALUE_TYPE_ARRAY);

    entry = symtab_lookup(ARRKEYS_OF(arr), pos);

    if (entry) {
        value_cleanup(&entry->entry_u.var);
        memset(&entry->entry_u.var, 0, sizeof(value));
        entry->entry_u.var.type = VALUE_TYPE_VOID;
    }
}

/*
 * Delete element from array
 *
 * This function removes the array element at the given index.
 * The element is freed and replaced by a void value.
 */
void value_delete_array(value *arr, int pos)
{
    symtab_entry *entry;

    sanity(arr && arr->type == VALUE_TYPE_ARRAY);

    if (pos < 0) {
        pos = ARRLEN_OF(arr) + pos;
    }

    if (pos < 0 || pos >= ARRLEN_OF(arr)) {
        return;
    }

    entry = ARR_OF(arr)[pos];
    value_cleanup(&entry->entry_u.var);
    memset(&entry->entry_u.var, 0, sizeof(value));
    entry->entry_u.var.type = VALUE_TYPE_VOID;
}
