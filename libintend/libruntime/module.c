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
 * Intend C Module load and unload in runtime
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "runtime.h"

/*
 * Module data
 */
typedef struct {
    char *name;
    void *handle;
    void (*init)();
    void (*free)();
    void *next;
} module_data;

/*
 * Module data creation
 */
module_data *module_data_create(char *name)
{
    module_data *mod;

    sanity(name);

    mod = oom(calloc(1, sizeof(module_data)));
    mod->name = oom(strdup(name));

    return mod;
}

/*
 * Module data free
 */
void module_data_free(module_data *mod)
{
    sanity(mod);

    free(mod->name);
    cfree(mod);
}

/*
 * Construct lib/symbol name
 */
char *module_make_name(char *prefix, char *name, char *suffix)
{
    int size;
    char *sym;

    sanity(prefix && name && suffix);

    asprintf(&sym, "%s%s%s", prefix, name, suffix);
    return sym;
}

/*
 * Free lib/symbol name
 */
void module_free_name(char *name)
{
    sanity(name);

    cfree(name);
}

/*
 * Check if a module is loaded
 */
int module_is_preloaded(intend_state *s, char *name)
{
    module_data *mod = s->preload_modules;

    sanity(name);

    // Go through the pre-load modules linked list
    while (mod) {
        // Is this the module we want?
        if (strcmp(mod->name, name) == 0) {
            // Module found
            return 1;
        }
        // Next module
        mod = mod->next;
    }
    // Didn't find the module
    return 0;
}

/*
 * Add a module to pro-load list
 */
void module_add_preload(intend_state *s, char *name)
{
    module_data *mod;

    sanity(name);

    // Check if module is already in preload list
    if (!module_is_preloaded(s, name))	{
        // Create a new module data object
        mod = module_data_create(name);
        // Add it to the head of pre-load modules linked list
        mod->next = s->preload_modules;
        s->preload_modules = mod;
    }
}

/*
 * Check if a module is loaded
 */
int module_is_loaded(intend_state *s, char *name)
{
    module_data *mod = s->modules;

    sanity(name);

    // Go through the modules linked list
    while (mod) {
        // Is this the module we want?
        if (strcmp(mod->name, name) == 0) {
            // Module found
            return 1;
        }
        // Next module
        mod = mod->next;
    }
    // Didn't find the module
    return 0;
}

/*
 * Module loader
 */
int module_loader(intend_state *s, module_data *mod)
{
    void *handle;
    void *init;
    void *free;
    char lib_prefix[] = "libintend_";
    char lib_suffix[] = ".so.1";
    char *lib_name;
    char sym_prefix[] = "intend_";
    char init_suffix[] = "_module_initialize";
    char free_suffix[] = "_module_destroy";
    char *sym_name;

    // Construct init library name
    lib_name = module_make_name(lib_prefix, mod->name, lib_suffix);
    // Load the module library
    handle = dlopen(lib_name, RTLD_LAZY);
    // Destruct init library name
    module_free_name(lib_name);
    // If loaded
    if (handle) {
        // Construct init symbol name
        sym_name = module_make_name(sym_prefix, mod->name, init_suffix);
        // Get the init function pointer
        init = dlsym(handle, sym_name);
        // Destruct init symbol name
        module_free_name(sym_name);
        // If we have an init function
        if (init) {
            // Construct free symbol name
            sym_name = module_make_name(sym_prefix, mod->name, free_suffix);
            // Get the free function pointer
            free = dlsym(handle, sym_name);
            // Destruct free symbol name
            module_free_name(sym_name);
            // If we have an init function
            if (free) {
                // Add it to the head of modules linked list
                mod->handle = handle;
                mod->init = init;
                mod->free = free;
                mod->next = s->modules;
                s->modules = mod;
                // Call module init function with intend state as parameter
                (*(mod->init))(s);
                // Dit it
                return 1;
            }
        }
    }
    // Something went wrong...
    return 0;
}

/*
 * Load a module
 */
int module_load(intend_state *s, char *name)
{
    int res = 1;
    module_data *mod;

    sanity(name);

    // Check if module is already loaded
    if (!module_is_loaded(s, name)) {
        // If safe mode active return false
        if (safe_mode_get(s))
            return 0;

        // Create a new module data object
        mod = module_data_create(name);
        // Call module loader
        if (!(res = module_loader(s, mod))) {
            // Could not load it so free module data object
            module_data_free(mod);
        }
    }
    return res;
}

/*
 * Load all pre-load modules
 */
void module_preload(intend_state *s)
{
    module_data *mod = s->preload_modules;
    module_data *next = NULL;
    char *err;

    // Clear the pre-load modules linked list
    s->preload_modules = NULL;
    // Go through the pre-load modules linked list
    while (mod) {
        // Save the next module
        next = mod->next;
        // Call module loader
        if (!module_loader(s, mod)) {
            // Could not load it so send a warning, since at init fase
            // no exceptions are catched
            nonfatal(s, "error loading pre-loaded module '%s'", mod->name);
            // Free module data object
            module_data_free(mod);
        }
        // Next module
        mod = next;
    }
}

/*
 * Unload a module
 */
int module_unload(intend_state *s, char *name)
{
    int res;
    module_data *mod = s->modules;
    module_data *prev_mod = NULL;

    sanity(name);

    // Check if safe mode status and show warning if true
    if (safe_mode_get(s))
        return 0;

    // Go through the modules linked list
    while (mod) {
        // Is this the module we want?
        if (strcmp(mod->name, name) == 0) {
            // Got it, remove it from linked list
            if (prev_mod) {
                prev_mod->next = mod->next;
            } else {
                s->modules = mod->next;
            }
            // Finish the search
            break;
        }
        // Next module, but save previous
        prev_mod = mod;
        mod = mod->next;
    }
    // Did we find it?
    if (mod) {
        // Call module free function with intend state as parameter
        (*(mod->free))(s);
        // Unload the module library
        dlclose(mod->handle);
        // Free the module data object
        module_data_free(mod);
        return 1;
    }
    return 0;
}

/*
 * Module pre-load teardown
 */
void module_preload_teardown(intend_state *s)
{
    module_data *mod = s->preload_modules;
    module_data *next;

    // Clear the modules linked list
    s->preload_modules = NULL;
    // Go through the modules linked list
    while (mod) {
        // Save the next module
        next = mod->next;
        // Free the module data object
        module_data_free(mod);
        // Next module
        mod = next;
    }
}

/*
 * Module teardown
 */
void module_teardown(intend_state *s)
{
    module_data *mod = s->modules;
    module_data *next;

    // Clear the modules linked list
    s->modules = NULL;
    // Go through the modules linked list
    while (mod) {
        // Save the next module
        next = mod->next;
        // Call module free function with intend state as parameter
        (*(mod->free))(s);
        // Unload the module library
        dlclose(mod->handle);
        // Free the module data object
        module_data_free(mod);
        // Next module
        mod = next;
    }
}

