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
 * Intend C Symbol table memory management
 */

#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Allocate a symbol table entry
 */
symtab_entry *symtab_entry_alloc(void)
{
    return oom(calloc(sizeof(symtab_entry), 1));
}

/*
 * Recycles symbol table entry
 *
 * This function frees the contained data of a symtab entry
 * that needs freeing without freeing the function signatures array.
 * This function is for internal use of symbol tables only.
 */
void symtab_entry_recycle(symtab_entry *entry)
{
    int i;

    // Avoid null entries
    if (!entry) return;

    switch (entry->type) {
        case SYMTAB_ENTRY_VAR:
            value_cleanup(&entry->entry_u.var);
            break;
        case SYMTAB_ENTRY_FUNCTION:
            for (i = 0; i < entry->entry_u.fnc.len; i++) {
                call_sig_cleanup(&(entry->entry_u.fnc.sigs[i]));
            }
            entry->entry_u.fnc.len = 0;
            break;
        case SYMTAB_ENTRY_CLASS:
            if (entry->entry_u.cls.parent) free(entry->entry_u.cls.parent);
            if (entry->entry_u.cls.type == CLASS_TYPE_BUILTIN) {
                if (entry->entry_u.cls.constructor) call_sig_free(entry->entry_u.cls.constructor);
            }
            break;
    }
}

/*
 * Cleans symbol table entry
 *
 * This function frees the contained data of a symtab entry
 * that needs freeing.
 */
void symtab_entry_cleanup(symtab_entry *entry)
{
    int i;

    // Avoid null entries
    if (!entry) return;

    if (entry->symbol) free(entry->symbol);
    switch (entry->type) {
        case SYMTAB_ENTRY_VAR:
            value_cleanup(&entry->entry_u.var);
            break;
        case SYMTAB_ENTRY_FUNCTION:
            for (i = 0; i < entry->entry_u.fnc.len; i++) {
                call_sig_cleanup(&(entry->entry_u.fnc.sigs[i]));
            }
            free(entry->entry_u.fnc.sigs);
            entry->entry_u.fnc.len = 0;
            entry->entry_u.fnc.size = 0;
            break;
        case SYMTAB_ENTRY_CLASS:
            if (entry->entry_u.cls.parent) free(entry->entry_u.cls.parent);
            if (entry->entry_u.cls.type == CLASS_TYPE_BUILTIN) {
                if (entry->entry_u.cls.constructor) call_sig_free(entry->entry_u.cls.constructor);
            }
            break;
    }
    memset(entry, 0, sizeof(symtab_entry));
}

/*
 * Frees a symbol table entry
 */
void symtab_entry_free(symtab_entry *entry)
{
    // Avoid null entries
    if (!entry) return;

    // Cleanup entry
    symtab_entry_cleanup(entry);

    // Free self
    free(entry);
}

/*
 * Allocate symbol table node
 */
symtab_node *symtab_node_alloc(void)
{
    return oom(calloc(sizeof(symtab_node), 1));
}

/*
 * Free symbol table node
 *
 * This function frees a symbol table node and all contained
 * symbol table entries.
 */
void symtab_node_free(symtab_node *node)
{
    unsigned int i;

    if (!node) {
        return;
    }

    // Cleanup all entries in node
    for (i = 0; i < node->len; i++)
        symtab_entry_cleanup(&node->entries[i]);

    // Free the entries array
    free(node->entries);
#if DEBUG == 1
    memset(node, 0, sizeof(symtab_node));
#endif
    free(node);
}

/*
 * Allocate symbol table
 *
 * This function allocates a symbol table of the given order,
 * where the order is the power of two used as the number of
 * hash buckets. If 0 is passed, a default of 11 is used, which
 * results in 2048 hash table buckets.
 */
symtab *symtab_alloc(unsigned int order)
{
    symtab *table;

    if (order > 31) order = 31;
    if (order <= 0) order = SYMTAB_DEFAULT_ORDER;

    table = oom(calloc(sizeof(symtab), 1));
    table->nodes = oom(calloc(sizeof(symtab_node *), 1 << order));
    table->order = order;

    return table;
}

/*
 * Symtab entry duplication
 */
static symtab_entry *entrydup(symtab_entry *copy, symtab_entry *orig)
{
    int i;

    if (!orig || orig->type == SYMTAB_ENTRY_CLASS) {
        return NULL;
    }

    copy->symbol = xstrdup(orig->symbol);
    copy->type   = orig->type;

    switch (orig->type) {
        case SYMTAB_ENTRY_VAR:
            value_copy_to(&copy->entry_u.var, &orig->entry_u.var);
            break;
        case SYMTAB_ENTRY_FUNCTION:
            copy->entry_u.fnc.len = orig->entry_u.fnc.len;
            copy->entry_u.fnc.size = orig->entry_u.fnc.size;
            copy->entry_u.fnc.sigs = oom(malloc(copy->entry_u.fnc.size * sizeof(signature)));
            for (i = 0; i < copy->entry_u.fnc.len; i++) {
                call_sig_copy_to(&(copy->entry_u.fnc.sigs[i]), &(orig->entry_u.fnc.sigs[i]));
            }
            break;
        case SYMTAB_ENTRY_CLASS:
            /* not needed... */
            break;
    }

    return copy;
}

/*
 * Copy a symbol table and return new address
 */
symtab *symtab_copy(symtab *sym)
{
    symtab *copy;
    symtab_node *node, *nnode;
    unsigned int i, j;

    copy = symtab_alloc(sym->order);
    for (i = 0; i < (unsigned int)(1 << sym->order); i++) {
        node = sym->nodes[i];
        if (node) {
            copy->nodes[i] = nnode = symtab_node_alloc();
            nnode->len = node->len;
            nnode->size = node->size;
            nnode->entries =
                oom(calloc(sizeof(symtab_entry *) * node->size, 1));
            for (j = 0; j < node->len; j++) {
                entrydup(&nnode->entries[j], &node->entries[j]);
            }
        }
    }

    return copy;
}

/*
 * Free symbol table
 *
 * This function frees a symbol table and all contained nodes
 * and entries.
 */
void symtab_free(symtab *symtab)
{
    int i;
    int j;
    if (!symtab) {
        return;
    }

    for (i = 0; i < (1 << symtab->order); i++) {
        symtab_node_free(symtab->nodes[i]);
    }
#if DEBUG == 1
    memset(symtab->nodes, 0, sizeof(symtab_node *) * (1 << symtab->order));
#endif
    symtab->order = 0;
    free(symtab->nodes);
    free(symtab);
}
