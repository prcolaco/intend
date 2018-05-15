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
 * Intend C Symbol table entry manipulation
 */

#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Add entry to symbol table node
 *
 * This function adds an entry to a symbol table node. The memory
 * allocated for the node is increased as needed.
 */
static symtab_entry *symtab_node_add(symtab_node *node, symtab_entry entry)
{
    symtab_entry *entries;
    symtab_entry *res;
    int oldsize;

    sanity(node);

    // Needs growth
    if (node->len >= node->size) {
        oldsize = node->size;
        node->size += SYMTAB_NODE_GROWTH;
        entries = oom(realloc(node->entries, (node->size + 1) * sizeof(symtab_entry)));
        memset(entries + oldsize, 0, (node->size - oldsize + 1) * sizeof(symtab_entry));
        node->entries = entries;
    } else {
        entries = node->entries;
    }

    entries[node->len] = entry;

    res = &entries[node->len];

    node->len++;

    return res;
}

/*
 * The FNV1a-32 hash algorithm
 * (http://www.isthe.com/chongo/tech/comp/fnv/)
 */
static unsigned int fnv1a(const char *str, unsigned int hval)
{
    static const unsigned int fnv_32_prime = 0x01000193;

    while (*str) {
        hval ^= *str++;
        hval *= fnv_32_prime;
    }
    return hval;
}

/*
 * Hash symbol name
 *
 * This function computes the hash value of a given symbol
 * name. The 32 bit hash value is reduced to the actual
 * number of bits available for symbol table hash buckets.
 */
static unsigned int symtab_hash(const char *symbol, unsigned int order)
{
    unsigned int hash;

    hash = fnv1a(symbol, 0);
    hash ^= (hash >> (32 - order));
    return (hash & ((1 << order) - 1));
}

/*
 * Add entry to symbol table
 *
 * This function adds an entry to a symbol table. It does not
 * check whether an entry with the same symbol name already
 * exists. If the node for the hash value of the symbol has not
 * yet been allocated, this function will allocate a new node.
 */
symtab_entry *symtab_add(symtab *symtab, symtab_entry entry)
{
    unsigned int pos;

    sanity(symtab && entry.symbol);

    pos = symtab_hash(entry.symbol, symtab->order);
    if (!symtab->nodes[pos]) {
        symtab->nodes[pos] = symtab_node_alloc();
    }

    return symtab_node_add(symtab->nodes[pos], entry);
}

/*
 * Lookup symbol in symbol table
 *
 * This function looks for a given symbol and returns a pointer
 * to entry if the symbol is found in the symbol table. If the
 * symbol is not found, NULL is returned.
 */
symtab_entry *symtab_lookup(symtab *symtab, const char *symbol)
{
    symtab_node *node;
    unsigned int pos, i;

    sanity(symtab);

    if (!symbol) {
        return NULL;
    }

    pos = symtab_hash(symbol, symtab->order);
    if (!symtab->nodes[pos]) {
        return NULL;
    }

    node = symtab->nodes[pos];
    for (i = 0; i < node->len; i++) {
        if (node->entries[i].symbol && strcmp(node->entries[i].symbol, symbol) == 0) {
            return &node->entries[i];
        }
    }
    return NULL;
}

/*
 * Get symbol in symbol table by numeric index
 *
 * This function gets the given symbol at an index and returns a
 * pointer to entry if the symbol is found in the symbol table. If
 * the symbol is not found, NULL is returned.
 */
symtab_entry *symtab_get(symtab *symtab, const int index)
{
    symtab_node *node;
    unsigned int max, pos, i, count;

    sanity(symtab);

    if (index < 0 || index > symtab_num_entries(symtab)) {
        return NULL;
    }

    max = 1 << symtab->order;

    count = 0;
    for (pos = 0; pos < max; pos++) {
        if (!symtab->nodes[pos]) {
            continue;
        }

        node = symtab->nodes[pos];
        for (i = 0; i < node->len; i++) {
            if (node->entries[i].symbol) {
                if (index == count) return &node->entries[i];
                count++;
            }
        }
    }
    return NULL;
}

/*
 * Delete symbol from symbol table
 *
 * This function removes the given symbol from the symbol table.
 * It is not an error if the symbol is not contained in the table.
 * Note that this function does not shrink the memory used by
 * the symbol table node that contained the symbol -- the memory
 * will be reused when another symbol is added to that node.
 */
void symtab_delete(symtab *symtab, const char *symbol)
{
    symtab_node *node;
    unsigned int pos, i, size;
    void *cur, *next;

    sanity(symtab);

    if (!symbol) {
        return;
    }

    pos = symtab_hash(symbol, symtab->order);
    if (!symtab->nodes[pos]) {
        return;
    }

    node = symtab->nodes[pos];
    for (i = 0; i < node->len; i++) {
        if (node->entries[i].symbol && strcmp(node->entries[i].symbol, symbol) == 0) {
            // Just clean the entry
            symtab_entry_cleanup(&node->entries[i]);
            break;
        }
    }
}

/*
 * Add variable to symbol table
 *
 * This function adds a variable with the given name and value to
 * the symbol table. The value and its name will be stored as copies.
 * Any previously existing entry with the same name is removed from
 * the symbol table and freed.
 */
symtab_entry *symtab_add_variable(symtab *symtab, const char *name, value *val)
{
    symtab_entry *entry;
    symtab_entry new;

    sanity(symtab && name && val);

    // Optimization for equal value assignments and large
    // data types, like strings, arrays and structs
    entry = symtab_lookup(symtab, name);
    if (entry && &entry->entry_u.var == val)
        return entry;

    // If entry already in symbol table
    if (entry && entry->symbol) {
        // If not the same type
        if (entry->type != SYMTAB_ENTRY_VAR) {
            symtab_entry_cleanup(entry);
            entry->symbol = xstrdup(name);
            entry->type = SYMTAB_ENTRY_VAR;
        }
        value_copy_to(&entry->entry_u.var, val);
    } else {
        memset(&new, 0, sizeof(symtab_entry));
        new.type = SYMTAB_ENTRY_VAR;
        new.symbol = xstrdup(name);
        value_copy_to(&new.entry_u.var, val);
        entry = symtab_add(symtab, new);
    }

    return entry;
}

/*
 * Add function to symbol table
 *
 * This function adds a function with the given name and signature
 * to the symbol table. The name and signature will be stored as
 * copies. Any previously existing entry with the same name is removed
 * from the symbol table and freed.
 */
symtab_entry *symtab_add_function(symtab *symtab, const char *name,
                                  signature *sig)
{
    char *symbol;
    symtab_entry *entry;
    symtab_entry new;
    void *sigs;

    sanity(symtab && name && sig);

    // Optimization for equal value assignments
    entry = symtab_lookup(symtab, name);
    if (entry && entry->entry_u.fnc.sigs && &(entry->entry_u.fnc.sigs[0]) == sig)
        return entry;

    // If not the same entry type
    if (entry && entry->type != SYMTAB_ENTRY_FUNCTION)
        symtab_entry_cleanup(entry);

    // If entry already in symbol table
    if (entry && entry->symbol) {
        symtab_entry_recycle(entry);
        if (entry->entry_u.fnc.len >= entry->entry_u.fnc.size) {
            entry->entry_u.fnc.size += FUNCTION_SIG_GROWTH;
            sigs = oom(realloc(entry->entry_u.fnc.sigs, entry->entry_u.fnc.size * sizeof(signature)));
            entry->entry_u.fnc.sigs = sigs;
        }
        //call_sig_copy_to(&(entry->entry_u.fnc.sigs[entry->entry_u.fnc.len - 1]), sig);
        call_sig_copy_to(&(entry->entry_u.fnc.sigs[0]), sig);	//TODO
        //entry->entry_u.fnc.len++;
        entry->entry_u.fnc.len = 1;								//TODO
    } else {
        memset(&new, 0, sizeof(symtab_entry));
        new.type = SYMTAB_ENTRY_FUNCTION;
        new.symbol = xstrdup(name);
        new.entry_u.fnc.len = 1;
        new.entry_u.fnc.size = FUNCTION_SIG_GROWTH;
        new.entry_u.fnc.sigs = oom(malloc(FUNCTION_SIG_GROWTH * sizeof(signature)));
        call_sig_copy_to(&(new.entry_u.fnc.sigs[0]), sig);
        entry = symtab_add(symtab, new);
    }

    return entry;
}

/*
 * Get variable from symbol table
 *
 * This function gets a variable with the given name from the symbol
 * table and returns its value. If there is no variable with that
 * name NULL is returned.
 */
value *symtab_get_variable(symtab *symtab, const char *name)
{
    symtab_entry *entry;

    sanity(symtab && name);

    // Look for name in the symbol table
    entry = symtab_lookup(symtab, name);
    // Is it found?
    if (entry) {
        // Is it a variable?
        if (entry->type == SYMTAB_ENTRY_VAR) {
            // Return its value
            return &entry->entry_u.var;
        }
    }
    // Not found, return NULL
    return NULL;
}

/*
 * Get function from symbol table
 *
 * This function gets a function with the given name from the symbol
 * table and returns its signature. If there is no function with that
 * name NULL is returned.
 */
signature *symtab_get_function(symtab *symtab, const char *name)
{
    symtab_entry *entry;

    sanity(symtab && name);

    // Look for name in the symbol table
    entry = symtab_lookup(symtab, name);
    // Is it found?
    if (entry) {
        // Is it a variable?
        if (entry->type == SYMTAB_ENTRY_FUNCTION) {
            // Return its signature
            return &(entry->entry_u.fnc.sigs[0]);
        }
    }
    // Not found, return NULL
    return NULL;
}

/*
 * Add class to symbol table
 *
 * This function adds a class with the given name, parent class,
 * and definition to the symbol table. The name and parent will
 * be stored as copies. Any previously existing entry with the same
 * name is removed from the symbol table and freed.
 */
symtab_entry *symtab_add_class(symtab *symtab, const char *name, const char *parent,
                               void *def)
{
    char *symbol, *pcopy = NULL;
    symtab_entry *entry;
    symtab_entry new;

    sanity(symtab && name && def);

    // Optimization for equal value assignments
    entry = symtab_lookup(symtab, name);
    if (entry && entry->entry_u.cls.definition == def)
        return entry;

    symbol = oom(malloc(strlen(name) + 1));
    strcpy(symbol, name);

    if (parent) {
        pcopy = oom(malloc(strlen(parent) + 1));
        strcpy(pcopy, parent);
    }

    // If entry already in symbol table
    if (entry) {
        symtab_entry_cleanup(entry);
        entry->type = SYMTAB_ENTRY_CLASS;
        entry->symbol = symbol;
        entry->entry_u.cls.type = CLASS_TYPE_USERDEF;
        entry->entry_u.cls.parent = pcopy;
        entry->entry_u.cls.definition = def;
        entry->entry_u.cls.constructor = NULL;
    } else {
        new.type = SYMTAB_ENTRY_CLASS;
        new.symbol = symbol;
        new.entry_u.cls.type = CLASS_TYPE_USERDEF;
        new.entry_u.cls.parent = pcopy;
        new.entry_u.cls.definition = def;
        new.entry_u.cls.constructor = NULL;
        entry = symtab_add(symtab, new);
    }

    return entry;
}

/*
 * Add builtin class to symbol table
 *
 * This function adds a builtin class with the given name, parent class,
 * and definition to the symbol table. The name and parent will
 * be stored as copies. Any previously existing entry with the same
 * name is removed from the symbol table and freed.
 */
symtab_entry *symtab_add_builtin_class(symtab *symtab, const char *name, const char *parent,
                                       signature *con)
{
    char *symbol, *pcopy = NULL;
    symtab_entry *entry;
    symtab_entry new;

    sanity(symtab && name && con);

    // Optimization for equal value assignments
    entry = symtab_lookup(symtab, name);
    if (entry && entry->entry_u.cls.constructor == con)
        return entry;

    symbol = oom(malloc(strlen(name) + 1));
    strcpy(symbol, name);

    if (parent) {
        pcopy = oom(malloc(strlen(parent) + 1));
        strcpy(pcopy, parent);
    }

    // If entry already in symbol table
    if (entry) {
        symtab_entry_cleanup(entry);
        entry->type = SYMTAB_ENTRY_CLASS;
        entry->symbol = symbol;
        entry->entry_u.cls.type = CLASS_TYPE_BUILTIN;
        entry->entry_u.cls.parent = pcopy;
        entry->entry_u.cls.definition = NULL;
        entry->entry_u.cls.constructor = call_sig_copy(con);
    } else {
        new.type = SYMTAB_ENTRY_CLASS;
        new.symbol = symbol;
        new.entry_u.cls.type = CLASS_TYPE_BUILTIN;
        new.entry_u.cls.parent = pcopy;
        new.entry_u.cls.definition = NULL;
        new.entry_u.cls.constructor = call_sig_copy(con);
        entry = symtab_add(symtab, new);
    }

    return entry;
}

/*
 * Count number of symtab entries
 */
int symtab_num_entries(symtab *sym)
{
    symtab_node *node;
    unsigned int i, j;
    int count = 0;

    sanity(sym);

    for (i = 0; i < (unsigned int)(1 << sym->order); i++) {
        node = sym->nodes[i];
        if (node) {
            for (j = 0; j < node->len; j++) {
                if (node->entries[j].symbol) {
                    ++count;
                }
            }
        }
    }
    return count;
}
