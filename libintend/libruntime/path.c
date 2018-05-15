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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <stddef.h>

#include "runtime.h"

/*
 * Resolve a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_resolve(intend_state *s, char *path)
{
    sanity(path);

    // Check if running in safe mode
    if (safe_mode_get(s) == SAFE_MODE_ON) {
        // Return the resolved path or NULL if not sandboxed or invalid sandbox
        return sandbox_resolve_path(s, path);
    } else {
        // Check is path is sandboxed
        if (sandbox_path_has_sandbox(path)) {
            // Return the resolved path or NULL if sandbox not valid
            return sandbox_resolve_path(s, path);
        } else {
            // Return a copy of original
            return oom(strdup(path));
        }
    }
}

/*
 * Validate a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_validate(intend_state *s, char *path)
{
    char *full;
    char *res = NULL;

    sanity(path);

    // Check if running in safe mode
    if (safe_mode_get(s) == SAFE_MODE_ON) {
        // Get the validated path or NULL if not sandboxed or invalid sandbox
        res = sandbox_validate_path(s, path);
    } else {
        // Check is path is sandboxed
        if (sandbox_path_has_sandbox(path)) {
            // Get resolved path
            full = sandbox_resolve_path(s, path);

            // Valid sandbox?
            if (full) {
                // Get the real valid path out of it
                res = path_absolute(full);

                // Free full
                free(full);
            }
        } else {
            // Get the real path of the original
            res = path_absolute(path);
        }
    }

    // Return result
    return res;
}

/*
 * Get a path access rights
 */
int path_access(intend_state *s, char *path)
{
    sanity(path);

    // Check if running in safe mode
    if (safe_mode_get(s) == SAFE_MODE_ON) {
        // Return the access rights of path or SANDBOX_NONE if not sandboxed or invalid sandbox
        return sandbox_get_path_access(s, path);
    } else {
        // Check is path is sandboxed
        if (sandbox_path_has_sandbox(path)) {
            // Return the access rights of path or SANDBOX_NONE if sandbox not valid
            return sandbox_get_path_access(s, path);
        } else {
            // Return RW if not sandboxed
            return SANDBOX_RW;
        }
    }
}

/*
 * Get parent path of a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_parent(char *path)
{
    char *work;
    char *par;
    char *p;
    char *res = NULL;

    sanity(path);

    // Create a copy of path to work with
    work = oom(strdup(path));

    // Get the parent pointer
    par = dirname(work);

    // Is parent valid?
    if (par) {
        // If parent length greater than 1 ('.' or '/')
        if (strlen(par) > 1) {
            // Add a trailing slash if last char is ':' (sandbox :/)
            p = par + strlen(par) - 1;
            if (*p == ':') {
                *(++p) = '/';
                *(++p) = 0x00;
            }
        }

        // Create a return copy
        res = oom(strdup(par));
    }

    // Free work copy
    free(work);

    // Return it
    return res;
}

/*
 * Get the file of a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_file(char *path)
{
    char *work;
    char *file;
    char *res = NULL;

    sanity(path);

    // Create a copy of path to work with
    work = oom(strdup(path));

    // Get the parent pointer
    file = basename(work);

    // Is file valid?
    if (file) {
        // Create a return copy
        res = oom(strdup(file));
    }

    // Free work copy
    free(work);

    // Return it
    return res;
}

/*
 * Join parent path and file to compose a complete path
 * The returned path is allocated, so don't forget to free it
 */
char *path_join(char *parent, char *file)
{
    char *work;
    char *path = NULL;

    sanity(parent && file);

    // Compose the valid name
    if (asprintf(&work, "%s/%s", parent, file) > 0) {
        path = work;
    }

    // Return it
    return path;
}

/*
 * Validate a parent path, not including the filename
 * The returned path is allocated, so don't forget to free it
 */
char *path_validate_parent(intend_state *s, char *path)
{
    char *full = NULL;
    char *parent;
    char *file;
    char *validpath;

    sanity(path);

    // Get parent path and file from filename
    parent = path_parent(path);
    file = path_file(path);

    // Validate the parent path
    validpath = path_validate(s, parent);

    // Valid path?
    if (validpath) {
        // Compose the valid name
        full = path_join(validpath, file);

        // Free valid path
        free(validpath);
    }

    // Free parent path and file
    free(parent);
    free(file);

    // Return the full validated path
    return full;
}

/*
 * Get real path of path (use realpath function)
 * The returned path is allocated, so don't forget to free it
 */
char *path_real(char *path)
{
    return realpath(path, NULL);
}

/*
 * Get the absolute path like realpath() but without resolving symlinks,
 * so that it is possible to have symlinks inside sandboxes... :D
 * (This function is based on the glibc realpath one with the symlink part
 * cut out and some minor changes...)
 * The returned path is allocated, so don't forget to free it
 */
char *path_absolute(char *path)
{
    char *rpath, *dest, *extra_buf = NULL;
    const char *start, *end, *rpath_limit;
    long int path_max = 1024;
    int num_links = 0;

    sanity(path);

    // Alloc the buffer and set a limit pointer to the end of it
    rpath = oom(malloc(path_max));
    rpath_limit = rpath + path_max;


    if (path[0] != '/') {
        if (!getcwd(rpath, path_max)) {
            rpath[0] = '\0';
            goto error;
        }
        dest = (char *)__rawmemchr(rpath, '\0');
    } else {
        rpath[0] = '/';
        dest = rpath + 1;
    }

    for (start = end = path; *start; start = end) {
        int n;

        /* Skip sequence of multiple path-separators.  */
        while (*start == '/')
            ++start;

        /* Find end of path component.  */
        for (end = start; *end && *end != '/'; ++end)
            /* Nothing.  */;

        if (end - start == 0)
            break;
        else if (end - start == 1 && start[0] == '.')
            /* nothing */;
        else if (end - start == 2 && start[0] == '.' && start[1] == '.') {
            /* Back up to previous component, ignore if at root already.  */
            if (dest > rpath + 1)
                while ((--dest)[-1] != '/');
        } else {
            size_t new_size;

            if (dest[-1] != '/')
                *dest++ = '/';

            if (dest + (end - start) >= rpath_limit) {
                ptrdiff_t dest_offset = dest - rpath;
                char *new_rpath;

                new_size = rpath_limit - rpath;
                if (end - start + 1 > path_max)
                    new_size += end - start + 1;
                else
                    new_size += path_max;
                new_rpath = (char *) realloc(rpath, new_size);
                if (new_rpath == NULL)
                    goto error;
                rpath = new_rpath;
                rpath_limit = rpath + new_size;

                dest = rpath + dest_offset;
            }

            dest = (char *)__mempcpy(dest, start, end - start);
            *dest = '\0';
        }
    }
    if (dest > rpath + 1 && dest[-1] == '/')
        --dest;
    *dest = '\0';

    return rpath;

error:
    free(rpath);
    return NULL;
}

