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
 * Intend C Sandbox functions
 */

// To avoid warnings on strndup function
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Sandbox separator constant
 */
const char sandbox_separator[] = ":/";

/*
 * Sandbox data
 */
typedef struct {
    char *name;
    char *path;
    int access;
    void *next;
} sandbox_data;

/*
 * Sandbox data creation
 */
sandbox_data *sandbox_data_create(char *name, char *path, int access)
{
    sandbox_data *sb;

    sanity(name);

    sb = oom(calloc(1, sizeof(sandbox_data)));
    sb->name = oom(strdup(name));
    sb->path = oom(strdup(path));
    sb->access = access;

    return sb;
}

/*
 * Sandbox data free
 */
void sandbox_data_free(sandbox_data *sb)
{
    sanity(sb);

    free(sb->name);
    free(sb->path);
    cfree(sb);
}

/*
 * Add a sandbox to state
 */
void sandbox_add(intend_state *s, char *name, char *path, int access)
{
    sandbox_data *sb;

    sanity(name && path);

    // Create a new sandbox data object
    sb = sandbox_data_create(name, path, access);

    // Add it to the head of sandboxes linked list
    sb->next = s->sandboxes;
    s->sandboxes = sb;
}

/*
 * Get safebox dir from state
 */
char *sandbox_get(intend_state *s, char *name)
{
    sandbox_data *sb = s->sandboxes;

    sanity(name);

    // Go through the sandboxes linked list
    while (sb) {
        // Is this the sandbox we want?
        if (strcmp(sb->name, name) == 0) {
            // Sandbox found
            return sb->path;
        }
        // Next sandbox
        sb = sb->next;
    }
    // Didn't find the sandbox
    return NULL;
}

/*
 * Get safebox access rights from state
 */
int sandbox_get_access(intend_state *s, char *name)
{
    sandbox_data *sb = s->sandboxes;

    sanity(name);

    // Go through the sandboxes linked list
    while (sb) {
        // Is this the sandbox we want?
        if (strcmp(sb->name, name) == 0) {
            // Sandbox found
            return sb->access;
        }
        // Next sandbox
        sb = sb->next;
    }
    // Didn't find the sandbox
    return SANDBOX_NONE;
}

/*
 * Free sandboxes linked list data from state
 */
void sandbox_free(intend_state *s)
{
    sandbox_data *sb = s->sandboxes;
    sandbox_data *next;

    // Clear the sandboxes linked list
    s->sandboxes = NULL;
    // Go through the sandboxes linked list
    while (sb) {
        // Save the next sandbox
        next = sb->next;
        // Free the sandbox data object
        sandbox_data_free(sb);
        // Next sandbox
        sb = next;
    }
}

/*
 * Helper function to remove trailing slash from paths
 */
static char *remove_trail_slash(char *path)
{
    int pos;

    if (path && (strlen(path) > 1)) {
        pos = strlen(path) - 1;
        if (path[pos] == '/') path[pos] = 0x00;
    }
    return path;
}

/*
 * Check if path has a sandbox (don't validate it)
 */
int sandbox_path_has_sandbox(char *path)
{
    char *found;
    int size;
    int res = 0;

    // Get a pointer to separator
    found = strstr(path, sandbox_separator);

    // Separator found?
    if (found) {
        // Get sandbox name size
        size = (int)(found - path);

        // Check the size of sandbox
        if (size > 0) {
            // Result true
            res = 1;
        }
    }

    // Return result
    return res;
}

/*
 * Get sandbox path access rights
 */
int sandbox_get_path_access(intend_state *s, char *path)
{
    char *sb;
    char *found;
    int res = SANDBOX_NONE;
    int size;

    // Get a pointer to separator
    found = strstr(path, sandbox_separator);

    // Get sandbox name size
    size = (int)(found - path);

    // Separator found?
    if (found) {
        // Get sandbox name size
        size = (int)(found - path);

        // Check the size of sandbox
        if (size > 0) {
            // Create the sandbox buffer
            sb = oom(strndup(path, size));

            // Get the sandbox access rights
            res = sandbox_get_access(s, sb);

            // Free sandbox buffer
            free(sb);
        }
    }

    // Return result
    return res;
}

/*
 * Get sandbox dir from path
 * Don't forget to free the result if not NULL
 */
char *sandbox_get_from_path(intend_state *s, char *path)
{
    char *sb;
    char *found;
    char *sb_path;
    char *res = NULL;
    int size;

    // Get a pointer to separator
    found = strstr(path, sandbox_separator);

    // Get sandbox name size
    size = (int)(found - path);

    // Separator found?
    if (found) {
        // Get sandbox name size
        size = (int)(found - path);

        // Check the size of sandbox
        if (size > 0) {
            // Create the sandbox buffer
            sb = oom(strndup(path, size));

            // Get the sandbox path
            sb_path = sandbox_get(s, sb);

            // Free sandbox buffer
            free(sb);

            // Sandbox valid?
            if (sb_path) {
                // We got the sandbox dir, alloc a copy and remove trail slash if exists
                res = oom(remove_trail_slash(strdup(sb_path)));
            }
        }
    }

    // Return result
    return res;
}

/*
 * Resolve the sandbox path, returning NULL not sandboxed
 * Don't forget to free the result if not NULL
 */
char *sandbox_resolve_path(intend_state *s, char *path)
{
    char *sb_dir;
    char *rel;
    char *full = NULL;

    // Get the sandbox dir from sandboxed path
    sb_dir = sandbox_get_from_path(s, path);

    // Sandbox valid?
    if (sb_dir) {
        // Compute the relative path from sandbox
        rel = strstr(path, sandbox_separator) + strlen(sandbox_separator);

        // Create the full path and check errors
        if (asprintf(&full, "%s/%s", sb_dir, rel) < 0) {
            // An error occurred
            full = NULL;
        } else {
            // Otherwise, remove trail slash if exists
            remove_trail_slash(full);
        }

        // Free the sandbox dir
        free(sb_dir);
    }

    // Return the result
    return full;
}

/*
 * Validate the sandbox path, returning NULL if invalid
 * Don't forget to free the result if not NULL
 */
char *sandbox_validate_path(intend_state *s, char *path)
{
    char *sb_dir;
    char *sand;
    char *full;
    char *real = NULL;

    // Get the sandbox dir from sandboxed path
    sb_dir = remove_trail_slash(sandbox_get_from_path(s, path));

    // Sandbox valid?
    if (sb_dir) {
        // Get real path of sandbox
        sand = path_absolute(sb_dir);

        // Real sandbox path valid?
        if (sand) {
            // Get the resolved full path
            full = sandbox_resolve_path(s, path);

            // Full path valid?
            if (full) {
                // Get the real path of the full path
                real = path_absolute(full);

                // Real path valid?
                if (real) {
                    // Check if real path starts with real sandbox path
                    if (strncmp(real, sand, strlen(sand)) != 0) {
                        // Path is not in sandbox, so free it and make it NULL
                        free(real);
                        real = NULL;
                    }
                }

                // Free full path
                free(full);
            }

            // Free real sandbox path
            free(sand);
        }

        // Free the sandbox dir
        free(sb_dir);
    }

    // Not found or not valid
    return remove_trail_slash(real);
}
