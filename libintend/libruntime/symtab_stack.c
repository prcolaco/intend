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
 * Intend C Symbol table stack
 */

#include <stdlib.h>

#include "runtime.h"

/*
 * Tear down symbol table stack
 *
 * Deallocates all memory used by the symbol table stack.
 */
void symtab_stack_teardown(intend_state *s)
{
    sanity(s->global_table);

    while (s->local_depth > 0) {
        symtab_stack_leave(s);
    }

    free(s->local_tables);
    s->local_tables = NULL;

    symtab_free(s->global_table);
    s->global_table = NULL;
}

/*
 * Initialize symbol table stack
 *
 * This function allocates memory for the global symbol table
 * and resets the local symbol table stack to depth 0.
 */
void symtab_stack_init(intend_state *s)
{
    if (!s->global_table) {
        s->global_table = oom(symtab_alloc(0));

        s->local_depth = 0;
        s->local_tables = NULL;
    }
}

/*
 * Enter local symbol table
 *
 * This function adds a new local symbol table to the stack. The
 * new symbol table obscures the previous toplevel local table.
 */
void symtab_stack_enter(intend_state *s)
{
    symtab **local;
    symtab *newtab;

    newtab = symtab_alloc(7);

    local = oom(realloc(s->local_tables,
                        (s->local_depth + 1) * sizeof(symtab *)));

    s->local_tables = local;
    local[s->local_depth] = newtab;
    ++s->local_depth;
}

/*
 * Leave local symbol table
 *
 * This function removes and frees the topmost local symbol table.
 * The previous local symbol table becomes visible again.
 */
void symtab_stack_leave(intend_state *s)
{
    symtab **local = s->local_tables;

    if (s->local_depth == 0) {
        return;
    }
    --s->local_depth;
    symtab_free(local[s->local_depth]);
}

/*
 * Pop symbol table stack
 *
 * This function returns the topmost local symbol table and makes
 * the previous local symbol table visible again.
 */
symtab *symtab_stack_pop(intend_state *s)
{
    symtab **local = s->local_tables;

    sanity(s->local_depth > 0);

    --s->local_depth;
    return local[s->local_depth];
}

/*
 * Get stack depth
 *
 * This function returns the current depth of the local symbol
 * table stack. It returns 0 if there are no local symbol tables
 * at the moment.
 */
unsigned int symtab_stack_depth(intend_state *s)
{
    return s->local_depth;
}

/*
 * Add symbol table entry to stack
 *
 * This functions adds the given entry to the topmost symbol
 * table of the stack. This means the global table if no
 * local symbol table exists.
 */
void symtab_stack_add(intend_state *s, symtab_entry entry)
{
    symtab **local = s->local_tables;

    if (s->local_depth > 0) {
        symtab_add(local[s->local_depth-1], entry);
    } else {
        symtab_add(s->global_table, entry);
    }
}

/*
 * Put variable into global symbol table
 *
 * This function adds the given variable to the global symbol
 * table, no matter whether a current local table exists.
 */
void symtab_stack_add_global_variable(intend_state *s, const char *name, value *val)
{
    sanity(name && val);
    symtab_add_variable(s->global_table, name, val);
}

/*
 * Put function into global symbol table
 *
 * This function adds the given function to the global symbol
 * table, no matter whether a current local table exists.
 */
void symtab_stack_add_global_function(intend_state *s, const char *name,
                                      signature *sig)
{
    sanity(name && sig);
    symtab_add_function(s->global_table, name, sig);
}

/*
 * Add variable entry to stack
 *
 * This functions adds the given variable to the topmost symbol
 * table of the stack. This means the global table if no local
 * symbol table exists.
 */
void symtab_stack_add_variable(intend_state *s, const char *name, value *val)
{
    symtab **local = s->local_tables;

    sanity(name && val);

    if (s->local_depth > 0) {
        symtab_add_variable(local[s->local_depth-1], name, val);
    } else {
        symtab_add_variable(s->global_table, name, val);
    }
}

/*
 * Add function entry to stack
 *
 * This function adds the given function to the topmost symbol
 * table of the stack. This means the global table if no local
 * symbol table exists.
 */
void symtab_stack_add_function(intend_state *s, const char *name,
                               signature *sig)
{
    symtab **local = s->local_tables;

    sanity(name && sig);

    if (s->local_depth > 0) {
        symtab_add_function(local[s->local_depth-1], name, sig);
    } else {
        symtab_add_function(s->global_table, name, sig);
    }
}

/*
 * Add class entry to stack
 *
 * This function adds the given class to the topmost symbol table
 * of the stack. This means the global table if no local symbol
 * table exists.
 */
void symtab_stack_add_class(intend_state *s, const char *name,
                            const char *parent, void *def)
{
    symtab **local = s->local_tables;

    sanity(name && def);

    if (s->local_depth > 0) {
        symtab_add_class(local[s->local_depth-1], name, parent, def);
    } else {
        symtab_add_class(s->global_table, name, parent, def);
    }
}

/*
 * Add builtin class entry to stack
 *
 * This function adds the given class to the topmost symbol table
 * of the stack. This means the global table if no local symbol
 * table exists.
 */
void symtab_stack_add_builtin_class(intend_state *s, const char *name,
                                    const char *parent, signature *con)
{
    sanity(name && con);

    symtab_add_builtin_class(s->global_table, name, parent, con);
}

/*
 * Lookup symbol in stack
 *
 * This function looks for the given symbol name in the topmost
 * local symbol table. If the symbol is not found there, it is
 * also searched for in the global table.
 */
symtab_entry *symtab_stack_lookup(intend_state *s, const char *symbol)
{
    symtab **local = s->local_tables;
    symtab_entry *entry = NULL;

    if (s->local_depth > 0) {
        entry = symtab_lookup(local[s->local_depth-1], symbol);
    }
    if (!entry) {
        entry = symtab_lookup(s->global_table, symbol);
    }
    return entry;
}

/*
 * Get variable from stack
 *
 * This function gets a variable with the given name from the stack
 * and returns its value. If there is no variable with that name
 * NULL is returned.
 */
value *symtab_stack_get_variable(intend_state *s, const char *name)
{
    symtab **local = s->local_tables;
    value *val = NULL;

    if (s->local_depth > 0) {
        val = symtab_get_variable(local[s->local_depth-1], name);
    }
    if (!val) {
        val = symtab_get_variable(s->global_table, name);
    }
    return val;
}

/*
 * Get function from stack
 *
 * This function gets a function with the given name from the stack
 * and returns its signature. If there is no function with that name
 * NULL is returned.
 */
signature *symtab_stack_get_function(intend_state *s, const char *name)
{
    symtab **local = s->local_tables;
    signature *func = NULL;

    if (s->local_depth > 0) {
        func = symtab_get_function(local[s->local_depth-1], name);
    }
    if (!func) {
        func = symtab_get_function(s->global_table, name);
    }
    return func;
}

/*
 * Check locality
 *
 * This function returns 1 if the given symbol name would be
 * resolved from a local symbol table. If the global table is
 * active at the moment, the answer is always "yes".
 */
int symtab_stack_local(intend_state *s, const char *symbol)
{
    symtab **local = s->local_tables;
    symtab_entry *entry;

    if (s->local_depth > 0) {
        entry = symtab_lookup(local[s->local_depth-1], symbol);
        return (entry != NULL);
    }
    return 1;
}

/*
 * Delete symbol from stack
 *
 * This function deletes the given symbol name from the topmost
 * symbol table in the stack. This means the global table if no
 * local symbol table exists.
 */
void symtab_stack_delete(intend_state *s, const char *symbol)
{
    symtab **local = s->local_tables;

    if (s->local_depth > 0) {
        symtab_delete(local[s->local_depth-1], symbol);
    } else {
        symtab_delete(s->global_table, symbol);
    }
}

/*
 * Delete symbol from global stack
 *
 * This function deletes the given symbol name from the global
 * table.
 */
void symtab_stack_delete_global(intend_state *s, const char *symbol)
{
    sanity(symbol);

    symtab_delete(s->global_table, symbol);
}
