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
 * Intend C File operations module, based on standard C facilities
 */

// To define some helper functions
#define _GNU_SOURCE

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>

#include <intend.h>

#include "intend_file.h"

/*
 * file variables to register
 */
static int file_S_IFMT   = S_IFMT;
static int file_S_IFSOCK = S_IFSOCK;
static int file_S_IFLNK  = S_IFLNK;
static int file_S_IFREG  = S_IFREG;
static int file_S_IFBLK  = S_IFBLK;
static int file_S_IFDIR  = S_IFDIR;
static int file_S_IFCHR  = S_IFCHR;
static int file_S_IFIFO  = S_IFIFO;
static int file_S_ISUID  = S_ISUID;
static int file_S_ISGID  = S_ISGID;
static int file_S_ISVTX  = S_ISVTX;
static int file_S_IRWXU  = S_IRWXU;
static int file_S_IRUSR  = S_IRUSR;
static int file_S_IWUSR  = S_IWUSR;
static int file_S_IXUSR  = S_IXUSR;
static int file_S_IRWXG  = S_IRWXG;
static int file_S_IRGRP  = S_IRGRP;
static int file_S_IWGRP  = S_IWGRP;
static int file_S_IXGRP  = S_IXGRP;
static int file_S_IRWXO  = S_IRWXO;
static int file_S_IROTH  = S_IROTH;
static int file_S_IWOTH  = S_IWOTH;
static int file_S_IXOTH  = S_IXOTH;
static int file_R_OK     = R_OK;
static int file_W_OK     = W_OK;
static int file_X_OK     = X_OK;
static int file_F_OK     = F_OK;

static intend_variable_data file_vars[] = {
    { "S_IFMT",         INTEND_TYPE_INT,        &file_S_IFMT        },
    { "S_IFSOCK",       INTEND_TYPE_INT,        &file_S_IFSOCK      },
    { "S_IFLNK",        INTEND_TYPE_INT,        &file_S_IFLNK       },
    { "S_IFREG",        INTEND_TYPE_INT,        &file_S_IFREG       },
    { "S_IFBLK",        INTEND_TYPE_INT,        &file_S_IFBLK       },
    { "S_IFDIR",        INTEND_TYPE_INT,        &file_S_IFDIR       },
    { "S_IFCHR",        INTEND_TYPE_INT,        &file_S_IFCHR       },
    { "S_IFIFO",        INTEND_TYPE_INT,        &file_S_IFIFO       },
    { "S_ISUID",        INTEND_TYPE_INT,        &file_S_ISUID       },
    { "S_ISGID",        INTEND_TYPE_INT,        &file_S_ISGID       },
    { "S_ISVTX",        INTEND_TYPE_INT,        &file_S_ISVTX       },
    { "S_IRWXU",        INTEND_TYPE_INT,        &file_S_IRWXU       },
    { "S_IRUSR",        INTEND_TYPE_INT,        &file_S_IRUSR       },
    { "S_IWUSR",        INTEND_TYPE_INT,        &file_S_IWUSR       },
    { "S_IXUSR",        INTEND_TYPE_INT,        &file_S_IXUSR       },
    { "S_IRWXG",        INTEND_TYPE_INT,        &file_S_IRWXG       },
    { "S_IRGRP",        INTEND_TYPE_INT,        &file_S_IRGRP       },
    { "S_IWGRP",        INTEND_TYPE_INT,        &file_S_IWGRP       },
    { "S_IXGRP",        INTEND_TYPE_INT,        &file_S_IXGRP       },
    { "S_IRWXO",        INTEND_TYPE_INT,        &file_S_IRWXO       },
    { "S_IROTH",        INTEND_TYPE_INT,        &file_S_IROTH       },
    { "S_IWOTH",        INTEND_TYPE_INT,        &file_S_IWOTH       },
    { "S_IXOTH",        INTEND_TYPE_INT,        &file_S_IXOTH       },
    { "R_OK",           INTEND_TYPE_INT,        &file_R_OK          },
    { "W_OK",           INTEND_TYPE_INT,        &file_W_OK          },
    { "X_OK",           INTEND_TYPE_INT,        &file_X_OK          },
    { "F_OK",           INTEND_TYPE_INT,        &file_F_OK          },

    { NULL,             INTEND_TYPE_VOID,       NULL                }
};

/*
 * File functions to register
 */
static intend_function_data file_funcs[] = {
    { "is_file_resource",   file_is_resource,       1,  "r",    'b' },
    { "basename",           file_basename,          1,  "s",    's' },
    { "dirname",            file_dirname,           1,  "s",    's' },
    { "realpath",           file_realpath,          1,  "s",    's' },
    { "getcwd",             file_getcwd,            0,  "",     's' },
    { "chdir",              file_chdir,             1,  "s",    'b' },
    { "mkdir",              file_mkdir,             1,  "s",    'b' },
    { "rmdir",              file_rmdir,             1,  "s",    'b' },
    { "stat",               file_stat,              1,  "s",    '?' },
    { "access",             file_access,            2,  "si",   'b' },
    { "eaccess",            file_eaccess,           2,  "si",   'b' },
    { "umask",              file_umask,             1,  "i",    'i' },
    { "chown",              file_chown,             2,  "si",   'b' },
    { "chmod",              file_chmod,             2,  "si",   'b' },
    { "fopen",              file_open,              2,  "ss",   '?' },
    { "freopen",            file_reopen,            3,  "ssr",  '?' },
    { "fseek",              file_seek,              2,  "ri",   'b' },
    { "ftell",              file_tell,              1,  "r",    '?' },
    { "fread",              file_read,              2,  "ri",   '?' },
    { "fgetc",              file_getc,              1,  "r",    '?' },
    { "fgets",              file_gets,              1,  "r",    '?' },
    { "fwrite",             file_write,             2,  "rS",   '?' },
    { "setbuf",             file_setbuf,            2,  "rb",   'b' },
    { "fflush",             file_flush,             1,  "r",    'b' },
    { "feof",               file_is_eof,            1,  "r",    'b' },
    { "ferror",             file_is_error,          1,  "r",    'b' },
    { "clearerr",           file_clearerr,          1,  "r",    'v' },
    { "fclose",             file_close,             1,  "r",    'b' },
    { "link",               file_link,              2,  "ss",   'b' },
    { "symlink",            file_symlink,           2,  "ss",   'b' },
    { "unlink",             file_unlink,            1,  "s",    'b' },
    { "remove",             file_remove,            1,  "s",    'b' },
    { "rename",             file_rename,            2,  "ss",   'b' },
    { "copy",               file_copy,              2,  "ss",   'b' },
    { "move",               file_rename,            2,  "ss",   'b' },
    { "scandir",            file_scandir,           1,  "s",    '?' },
    { "scandirs",           file_scandirs,          1,  "s",    '?' },
    { "scanfiles",          file_scanfiles,         1,  "s",    '?' },
    { "fileread",           file_fileread,          1,  "s",    '?' },
    { "filewrite",          file_filewrite,         2,  "ss",   'b' },
    { "filereadarray",      file_filereadarray,     1,  "s",    '?' },
    { "filewritearray",     file_filewritearray,    2,  "as",   'b' },
    { "pipe",               file_pipe,              2,  "??",   'b' },

    /* list terminator */
    { NULL,     NULL,           0,  NULL,   0   }
};

/*
 * Callback function for freeing file handles
 */
static void file_free(void *data)
{
    if (data) fclose(data);
}

/*
 * Get file pointer from file resource
 */
static void *file_get(void *data)
{
    return data;
}

/*
 * Initialize the File functions module
 */
void intend_file_module_initialize(intend_ctx ctx)
{
    intend_value val;

    // Init file handles
    val = intend_create_value(INTEND_TYPE_RES, stdin, file_free, file_get);
    intend_register_variable(ctx, "stdin", INTEND_TYPE_RES, val);
    intend_free_value(val);

    val = intend_create_value(INTEND_TYPE_RES, stdout, file_free, file_get);
    intend_register_variable(ctx, "stdout", INTEND_TYPE_RES, val);
    intend_free_value(val);

    val = intend_create_value(INTEND_TYPE_RES, stderr, file_free, file_get);
    intend_register_variable(ctx, "stderr", INTEND_TYPE_RES, val);
    intend_free_value(val);

    // Register variables
    intend_register_variable_array(ctx, file_vars);

    // Register functions
    intend_register_function_array(ctx, file_funcs);
}

/*
 * Destroy the File functions module
 */
void intend_file_module_destroy(intend_ctx ctx)
{
    // Register functions
    intend_unregister_function_array(ctx, file_funcs);
    // Init file handles
    intend_unregister_symbol(ctx, "stdin");
    intend_unregister_symbol(ctx, "stdout");
    intend_unregister_symbol(ctx, "stderr");
}

/*
 * Return the filename part of path
 */
intend_value file_basename(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char buf[PATH_MAX] = "";
    char *res = NULL;

    if (!path || strlen(path) == 0)
        return intend_create_value(INTEND_TYPE_STRING, buf, NULL);

    strcpy(buf, path);
    res = basename(buf);
    if (!res) {
        buf[0] = '\0';
        return intend_create_value(INTEND_TYPE_STRING, buf, NULL);
    }

    return intend_create_value(INTEND_TYPE_STRING, res, NULL);
}

/*
 * Return the directory part of path
 */
intend_value file_dirname(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char buf[PATH_MAX] = "";
    char *res = NULL;

    if (!path || strlen(path) == 0)
        return intend_create_value(INTEND_TYPE_STRING, buf, NULL);

    strcpy(buf, path);
    res = dirname(buf);
    if (!res) {
        buf[0] = '\0';
        return intend_create_value(INTEND_TYPE_STRING, buf, NULL);
    }

    return intend_create_value(INTEND_TYPE_STRING, res, NULL);
}

/*
 * Return the absolute path the path refers to
 */
intend_value file_realpath(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char buf[PATH_MAX] = "";
    char *real = NULL;
    intend_value res = NULL;

    if (!path || strlen(path) == 0)
        return intend_create_value(INTEND_TYPE_STRING, buf, NULL);

    real = intend_path_absolute(ctx, path);
    if (!real) {
        buf[0] = '\0';
        return intend_create_value(INTEND_TYPE_STRING, buf, NULL);
    }

    res = intend_create_value(INTEND_TYPE_STRING, real, NULL);

    free(real);

    return res;
}

/*
 * Return the current working directory
 */
intend_value file_getcwd(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *buf = NULL;
    char empty[] = "";
    intend_value res = NULL;

    // If running in safe mode, don't allow to get current dir
    if (intend_safe_mode_get(ctx) == INTEND_SAFE_MODE_ON) {
        return intend_create_value(INTEND_TYPE_STRING, empty, NULL);
    }

    buf = get_current_dir_name();
    if (!buf)
        return intend_create_value(INTEND_TYPE_STRING, empty, NULL);

    res = intend_create_value(INTEND_TYPE_STRING, buf, NULL);

    free(buf);

    return res;
}

/*
 * Change the current working directory
 */
intend_value file_chdir(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    int res = 0;

    // If running in safe mode, don't allow to change current dir
    if (intend_safe_mode_get(ctx) == INTEND_SAFE_MODE_ON) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (path && strlen(path) > 0) {
        res = (chdir(path) != -1) ? 1 : 0;
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Create the supplied directory
 */
intend_value file_mkdir(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    mode_t mode = 0755;
    int res = 0;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to parent path
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the parent path (because we are creating the directory)
        if (!(validpath = intend_path_validate_parent(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // If we get a second argument, it is the mode
        if (argc == 2) {
            mode = intend_int_value(argv[1]);
        }

        // Create the directory
        res = (mkdir(validpath, mode) != -1) ? 1 : 0;

        // Free validated filename
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Remove the supplied directory
 */
intend_value file_rmdir(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    int res = 0;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to parent path
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Perform the directory removal
        res = (rmdir(validpath) != -1) ? 1 : 0;

        // Free validated filename
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Return stat information about the file or directory
 */
intend_value file_stat(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    struct stat data;
    intend_value res;
    intend_value item;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_VOID, NULL);
        }

        // Get the data
        if (stat(validpath, &data) == 0) {
            res = intend_create_value(INTEND_TYPE_STRUCT, NULL);
            // st_dev
            item = intend_create_value(INTEND_TYPE_INT, &data.st_dev);
            intend_struct_set(res, "st_dev", item);
            free(item);
            // st_ino
            item = intend_create_value(INTEND_TYPE_INT, &data.st_ino);
            intend_struct_set(res, "st_ino", item);
            free(item);
            // st_mode
            item = intend_create_value(INTEND_TYPE_INT, &data.st_mode);
            intend_struct_set(res, "st_mode", item);
            free(item);
            // st_nlink
            item = intend_create_value(INTEND_TYPE_INT, &data.st_nlink);
            intend_struct_set(res, "st_nlink", item);
            free(item);
            // st_uid
            item = intend_create_value(INTEND_TYPE_INT, &data.st_uid);
            intend_struct_set(res, "st_uid", item);
            free(item);
            // st_gid
            item = intend_create_value(INTEND_TYPE_INT, &data.st_gid);
            intend_struct_set(res, "st_gid", item);
            free(item);
            // st_rdev
            item = intend_create_value(INTEND_TYPE_INT, &data.st_rdev);
            intend_struct_set(res, "st_rdev", item);
            free(item);
            // st_size
            item = intend_create_value(INTEND_TYPE_INT, &data.st_size);
            intend_struct_set(res, "st_size", item);
            free(item);
            // st_blksize
            item = intend_create_value(INTEND_TYPE_INT, &data.st_blksize);
            intend_struct_set(res, "st_blksize", item);
            free(item);
            // st_blocks
            item = intend_create_value(INTEND_TYPE_INT, &data.st_blocks);
            intend_struct_set(res, "st_blocks", item);
            free(item);
            // st_atime
            item = intend_create_value(INTEND_TYPE_INT, &data.st_atime);
            intend_struct_set(res, "st_atime", item);
            free(item);
            // st_mtime
            item = intend_create_value(INTEND_TYPE_INT, &data.st_mtime);
            intend_struct_set(res, "st_mtime", item);
            free(item);
            // st_ctime
            item = intend_create_value(INTEND_TYPE_INT, &data.st_ctime);
            intend_struct_set(res, "st_ctime", item);
            free(item);
        } else {
            res = intend_create_value(INTEND_TYPE_VOID, NULL);
        }

        // Free validated filename
        free(validpath);
    }

    return res;
}

/*
 * Verify the access rights over a path
 */
intend_value file_access(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    int mode = intend_int_value(argv[1]);
    char *validpath;
    int res = 0;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to parent path
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Perform the access check
        res = (access(validpath, mode) != -1) ? 1 : 0;

        // Free validated filename
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Verify the access rights over a path
 */
intend_value file_eaccess(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    int mode = intend_int_value(argv[1]);
    char *validpath;
    int res = 0;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to parent path
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Perform the access check
        res = (eaccess(validpath, mode) != -1) ? 1 : 0;

        // Free validated filename
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Set process file or directory creation mask
 */
intend_value file_umask(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    mode_t mode = intend_int_value(argv[0]);
    mode_t res = 0;

    // If running in safe mode, don't allow to set umask
    if (intend_safe_mode_get(ctx) == INTEND_SAFE_MODE_ON) {
        return intend_create_value(INTEND_TYPE_INT, &res);
    }

    res = umask(mode);

    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Change the owner of the path
 */
intend_value file_chown(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    uid_t owner = intend_int_value(argv[1]);
    uid_t group = -1;
    char *validpath;
    int res = 0;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to parent path
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Check if we got a group argument
        if (argc == 3) {
            group = intend_int_value(argv[2]);
        }

        // Perform the directory removal
        res = (chown(validpath, owner, group) != -1) ? 1 : 0;

        // Free validated filename
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Change the access mode of the path
 */
intend_value file_chmod(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    mode_t mode = intend_int_value(argv[1]);
    char *validpath;
    int res = 0;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to parent path
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Perform the directory removal
        res = (chmod(validpath, mode) != -1) ? 1 : 0;

        // Free validated filename
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Open file with given mode
 *
 * Opens the given filename with the given mode and returns a file
 * handle. Modes are as in C. On error, void is returned.
 */
intend_value file_open(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *filename = intend_string_ptr(argv[0]);
    char *filemode = intend_string_ptr(argv[1]);
    char *validname;
    FILE *fp;
    intend_value res;

    // Check access rights to parent path
    if (intend_path_access(ctx, filename) != INTEND_SANDBOX_RW && strpbrk(filemode, "wa+")) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Validate the parent path (because file may not exist yet)
    if (!(validname = intend_path_validate_parent(ctx, filename))) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    fp = fopen(validname, filemode);

    // Free validated filename
    free(validname);

    if (!fp) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    res = intend_create_value(INTEND_TYPE_RES, fp, file_free, file_get);
    return res;
}

/*
 * Reopen file stream with given filename and mode
 *
 * Reopens the given stream to filename with the given mode. Modes are as in C.
 * On success returns true, otherwise false is returned.
 */
intend_value file_reopen(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *filename = intend_string_ptr(argv[0]);
    char *filemode = intend_string_ptr(argv[1]);
    FILE *fp = (FILE *)intend_resource_value(argv[2]);
    char *validname = NULL;
    int res;

    // Verify if in safe mode and if standard stream passed
    if ((intend_safe_mode_get(ctx) == INTEND_SAFE_MODE_ON) && (fileno(fp) < 3)) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    // If empty path ignore validations
    if (filename) {
        // Check access rights to parent path
        if (intend_path_access(ctx, filename) != INTEND_SANDBOX_RW && strpbrk(filemode, "wa+")) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the parent path (because file may not exist yet)
        if (!(validname = intend_path_validate_parent(ctx, filename))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        fp = freopen(validname, filemode, fp);

        // Free validated filename
        free(validname);
    } else {
        fflush(fp);
        switch (fileno(fp)) {
            case 0:     //stdin
                intend_stream_reset_stdin(ctx);
                break;
            case 1:     //stdout
                intend_stream_reset_stdout(ctx);
                break;
            case 2:     //stderr
                intend_stream_reset_stderr(ctx);
                break;
            default:     //invalid
                fp = NULL;
                break;
        }
    }

    if (fp) {
        res = 1;
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Check whether resource is file resource
 */
intend_value file_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    int res = 0;

    res = (type == file_free);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Set file position
 *
 * This function sets the file position for the given file handle.
 * Positive values are offsets from the start of the file, negative
 * offsets are from the end of the file. Returns true on success,
 * false on failure.
 */
intend_value file_seek(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int pos    = intend_int_value(argv[0]);
    int res = 0;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    res = !fseek(fp, pos, pos >= 0 ? SEEK_SET : SEEK_END);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Get file position
 *
 * Gets the current file position for the given file handle. On
 * error, void is returned.
 */
intend_value file_tell(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    long pos;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    pos = ftell(fp);
    if (pos == -1) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    return intend_create_value(INTEND_TYPE_INT, &pos);
}

/*
 * Read from file
 *
 * Reads at most n bytes from the given file descriptor. The result
 * is a string of at most n characters. On error, void is returned.
 * On read errors, a short string is returned.
 */
intend_value file_read(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int max    = intend_int_value(argv[0]);
    char *buf;
    int units;
    intend_value res;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    buf = intend_oom(calloc(max, 1));
    units = fread(buf, 1, max, fp);
    res = intend_create_value(INTEND_TYPE_STRING, buf, units);
    free(buf);

    return res;
}

/*
 * Get character from file
 */
intend_value file_getc(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    char buf[2];
    int got;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    got = fgetc(fp);
    if (got == EOF) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    buf[0] = got;
    buf[1] = 0;
    return intend_create_value(INTEND_TYPE_STRING, buf, NULL);
}

/*
 * Get line from file
 */
intend_value file_gets(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    char buf[65536];
    char *res;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    res = fgets(buf, 65536, fp);
    if (!res) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    return intend_create_value(INTEND_TYPE_STRING, buf, NULL);
}

/*
 * Write string to file
 *
 * This function writes a string to the given file handle. The number
 * of bytes written is returned. On error, void is returned.
 */
intend_value file_write(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int len    = intend_string_len(argv[1]);
    const char *data = intend_string_ptr(argv[1]);
    size_t written = 0;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    if (!len) {
        return intend_create_value(INTEND_TYPE_INT, &written);
    }

    written = fwrite(data, 1, len, fp);
    return intend_create_value(INTEND_TYPE_INT, &written);
}

/*
 * Set buffering
 *
 * Enables or disables I/O buffering for the given file handle.
 * Returns true on success or false on failure.
 */
intend_value file_setbuf(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int res    = -1;

    if (type == file_free && fp) {
        if (intend_bool_value(argv[1])) {
            res = setvbuf(fp, NULL, _IOFBF, BUFSIZ);
        } else {
            res = setvbuf(fp, NULL, _IONBF, BUFSIZ);
        }
    }

    res = !res;
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Flush file buffer
 *
 * Flushes the I/O buffer for the given file handle. Returns true
 * on success and false on failure.
 */
intend_value file_flush(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int res = 0;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    res = (fflush(fp) == 0);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Check end-of-file condition
 *
 * Returns true if the given file handle points to the end of the
 * corresponding file. The result is only meaninful if the file
 * has been read before. On error, EOF is assumed to be reached.
 */
intend_value file_is_eof(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int res = 1;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    res = feof(fp);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Check error condition
 *
 * Returns true if the given file handle had I/O errors during one
 * of the operations executed before. If the status cannot be
 * determined, it is assumed that no errors were encountered.
 */
intend_value file_is_error(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int res = 0;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    res = ferror(fp);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Clear status indicators
 *
 * This function clears the end-of-file and error indicators for
 * the given file handle.
 */
intend_value file_clearerr(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);

    if (type == file_free && fp) {
        clearerr(fp);
    }
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Close file
 *
 * This function closes the given file handle. It returns true on
 * success and false if an error occurs during closing. In any
 * case, the file handle is closed when the function returns. If
 * the given file handle does not exit, success is reported.
 */
intend_value file_close(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *type = intend_resource_release(argv[0]);
    FILE *fp   = intend_resource_value(argv[0]);
    int res = 1;

    if (type != file_free || !fp) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    intend_resource_value_set(argv[0], NULL);

    res = !fclose(fp);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Create an hard link
 */
intend_value file_link(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *lpath = intend_string_ptr(argv[1]);
    char *validpath;
    char *validlpath;
    int res = 0;

    // If path and lpath are available and not empty
    if ((path && strlen(path) > 0) && (lpath && strlen(lpath) > 0)) {
        // Check access rights to parent lpath
        if (intend_path_access(ctx, lpath) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the parent lpath (because we are creating the link)
        if (!(validlpath = intend_path_validate_parent(ctx, lpath))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Create the link
        res = (link(validpath, validlpath) != -1) ? 1 : 0;

        // Free validated filenames
        free(validpath);
        free(validlpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Create a symbolic link
 */
intend_value file_symlink(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *lpath = intend_string_ptr(argv[1]);
    char *validpath;
    char *validlpath;
    int res = 0;

    // If path and lpath are available and not empty
    if ((path && strlen(path) > 0) && (lpath && strlen(lpath) > 0)) {
        // Check access rights to parent lpath
        if (intend_path_access(ctx, lpath) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the parent lpath (because we are creating the symlink)
        if (!(validlpath = intend_path_validate_parent(ctx, lpath))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Create the symlink
        res = (symlink(validpath, validlpath) != -1) ? 1 : 0;

        // Free validated filenames
        free(validpath);
        free(validlpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Remove a link to a file, and the file if last link to it
 */
intend_value file_unlink(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    int res = 0;

    // If path and lpath are available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to parent path
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Create the directory
        res = (unlink(validpath) != -1) ? 1 : 0;

        // Free validated filenames
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Remove file
 *
 * This function tries to remove the given filename from the system.
 * It returns true on success and false on failure.
 */
intend_value file_remove(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    int res = 0;

    // If path and lpath are available and not empty
    if (path && strlen(path) > 0) {
        // Check access rights to dpath
        if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Remove the file
        res = !remove(validpath);

        // Free validated filename
        free(validpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Rename file
 *
 * This functions tries to rename the file given as the first
 * argument to the name given as the second argument. It returns
 * true on success and false on failure.
 */
intend_value file_rename(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *dpath = intend_string_ptr(argv[1]);
    char *validpath;
    char *validdpath;
    int res = 0;

    // If path and dpath are available and not empty
    if ((path && strlen(path) > 0) && (dpath && strlen(dpath) > 0)) {
        // Check access rights to dpath
        if (intend_path_access(ctx, dpath) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the parent dpath (because we may be moving the file)
        if (!(validdpath = intend_path_validate_parent(ctx, dpath))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Rename the file
        res = !rename(validpath, validdpath);

        // Free validated filenames
        free(validpath);
        free(validdpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Copy a file
 */
intend_value file_copy(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *dpath = intend_string_ptr(argv[1]);
    char *validpath;
    char *validdpath;
    FILE *in, *out;
    int page, read, wrote;
    char *buf;
    struct stat st;
    int res = 0;

    // If path and dpath are available and not empty
    if ((path && strlen(path) > 0) && (dpath && strlen(dpath) > 0)) {
        // Check access rights to parent dpath
        if (intend_path_access(ctx, dpath) != INTEND_SANDBOX_RW) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the parent dpath (because we are creating the file)
        if (!(validdpath = intend_path_validate_parent(ctx, dpath))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_BOOL, &res);
        }

        // Copy the file
        if (in = fopen(validpath, "r")) {
            if (out = fopen(validdpath, "w")) {
                res = 1;
                page = getpagesize();
                if (page < 512) page = 512;
                read = wrote = 0;
                buf = intend_oom(malloc(page));
                while ((read = fread(buf, 1, page, in)) > 0) {
                    wrote = fwrite(buf, 1, read, out);
                    if (read != wrote) {
                        res = 0;
                    }
                }
                free(buf);
                fclose(out);
            }
            fclose(in);
        }

        // Set the correct mode
        if (stat(validpath, &st) == 0) {
            chmod(validdpath, st.st_mode & 07777);
        }

        // Free validated filenames
        free(validpath);
        free(validdpath);
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Internal scandir filter to return directory entries
 */
static int file_scanfilter_dirs(const struct dirent *dir)
{
    return (dir->d_type & (DT_DIR | DT_LNK)) > 0 ? 1 : 0;
}

/*
 * Internal scandir filter to return file entries
 */
static int file_scanfilter_files(const struct dirent *dir)
{
    return (dir->d_type & (DT_REG | DT_LNK)) > 0 ? 1 : 0;
}

/*
 * Internal scandir helper that receives the filter as last argument
 */
static intend_value file_scandir_helper(intend_ctx ctx, unsigned int argc, intend_value *argv,
                                        int(*filter)(const struct dirent *))
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    struct dirent **dir;
    int count, i;
    mode_t lnk_mask = 0;
    struct stat st;
    char *full;
    int inc = 0;
    intend_value res, val;

    // If path and lpath are available and not empty
    if (path && strlen(path) > 0) {
        // Validate the path
        if (!(validpath = intend_path_validate(ctx, path))) {
            return intend_create_value(INTEND_TYPE_VOID, NULL);
        }

        // Check if filter is passed
        if (filter) {
            if (filter == file_scanfilter_dirs) {
                lnk_mask = S_IFDIR;
            } else if (filter == file_scanfilter_files) {
                lnk_mask = S_IFREG;
            }
        }

        // Scan the directory
        if ((count = scandir(validpath, &dir, filter, alphasort)) >= 0) {
            // Create the return empty array
            res = intend_create_value(INTEND_TYPE_ARRAY, NULL);
            // Go through all the returned elements
            for (i = 0; i < count; i++) {
                // Default inc to false
                inc = 1;
                // If this is a symlink, check the pointed element
                if (dir[i]->d_type | DT_LNK) {
                    // Default inc to false
                    inc = 0;
                    // Create the full path
                    full = intend_path_join(validpath, dir[i]->d_name);
                    // Check what symlink points to
                    if (stat(full, &st) == 0) {
                        inc = (st.st_mode & lnk_mask) > 0 ? 1 : 0;
                    }
                    // Free the full path
                    free(full);
                }
                // If inc is still false, ignore this element
                if (inc) {
                    // Add this element to the array
                    val = intend_create_value(INTEND_TYPE_STRING, dir[i]->d_name, NULL);
                    intend_array_add(res, val);
                    intend_free_value(val);
                }
                // Free this element in the dirent array
                free(dir[i]);
            }
        } else {
            res = intend_create_value(INTEND_TYPE_VOID, NULL);
        }

        // Free validated path
        free(validpath);

        // Free the dirent array
        free(dir);
    }

    return res;
}

/*
 * Scan the directory supplied and return all elements in an array
 */
intend_value file_scandir(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return file_scandir_helper(ctx, argc, argv, NULL);
}

/*
 * Scan the directory supplied and return all directory elements in an array
 */
intend_value file_scandirs(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return file_scandir_helper(ctx, argc, argv, file_scanfilter_dirs);
}

/*
 * Scan the directory supplied and return all file elements in an array
 */
intend_value file_scanfiles(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return file_scandir_helper(ctx, argc, argv, file_scanfilter_files);
}

/*
 * Read a file contents to a string
 */
intend_value file_fileread(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[0]);
    char *validpath;
    FILE *in;
    int page, read, len;
    char *buf, *str;
    intend_value res;

    // If path and dpath are available and not empty
    if (!(path && strlen(path) > 0)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Validate the path
    if (!(validpath = intend_path_validate(ctx, path))) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Open the file
    in = fopen(validpath, "r");

    // Check if opened ok
    if (!in) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Define the page size and buffer for read operations
    page = getpagesize();
    if (page < 512) page = 512;
    buf = intend_oom(malloc(page));

    // Alloc the minimum result buffer
    str = intend_oom(malloc(1));
    str[0] = '\0';

    // Actually read the file contents
    read = len = 0;
    while ((read = fread(buf, 1, page, in)) > 0) {
        // Realloc result str buffer
        str = intend_oom(realloc(str, len + read + 1));

        // Copy the buffer to result str
        memcpy(str + len, buf, read);

        // Trailing '\0'
        str[len + read] = '\0';

        // Increment len
        len += read;
    }

    // Free the buffer
    free(buf);

    // Close the file
    fclose(in);

    // Free validated filenames
    free(validpath);

    // Create result value
    res = intend_create_value(INTEND_TYPE_STRING, str, NULL);

    // Free result str
    free(str);

    return res;
}

/*
 * Write the string contents to a file
 */
intend_value file_filewrite(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *str  = intend_string_ptr(argv[0]);
    int len = intend_string_len(argv[0]);
    char *path = intend_string_ptr(argv[1]);
    char *validpath;
    FILE *out;
    int wrote;
    int res = 0;

    // If path and dpath are available and not empty
    if (!(path && strlen(path) > 0)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Check access rights to path
    if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Validate the parent path
    if (!(validpath = intend_path_validate_parent(ctx, path))) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Open the file
    out = fopen(validpath, "w");

    // Check if opened ok
    if (!out) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Actually write the file contents
    wrote = fwrite(str, 1, len, out);

    // Close the file
    fclose(out);

    // Remove the file if not wrote completely
    if (wrote == len) {
        res = 1;
    } else {
        remove(validpath);
    }

    // Free validated filenames
    free(validpath);

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Read a file contents to an array
 */
intend_value file_filereadarray(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value res, val;
    intend_value sep;
    char dsep[2] = "\n";

    // Read the file
    val = file_fileread(ctx, argc, argv);

    // If read was not successful, return the read result
    if (intend_value_type(val) == INTEND_TYPE_VOID) {
        return val;
    }

    // Construct separator argument
    if (argc == 2) {
        sep = intend_copy_value(argv[1]);
    } else {
        sep = intend_create_value(INTEND_TYPE_STRING, dsep, NULL);
    }

    // Call explode stdlib function
    res = intend_call_function(ctx, "explode", 2, val, sep);

    // Free arguments
    intend_free_value(val);
    intend_free_value(sep);

    // Return result array
    return res;
}

/*
 * Write the array contents to a file
 */
intend_value file_filewritearray(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *path = intend_string_ptr(argv[1]);
    intend_value res, val, sep;
    char dsep[2] = "\n";
    int false = 0;

    // Check access rights to path
    if (intend_path_access(ctx, path) != INTEND_SANDBOX_RW) {
        return intend_create_value(INTEND_TYPE_BOOL, &false);
    }

    // Construct separator argument
    if (argc == 3) {
        sep = intend_copy_value(argv[2]);
    } else {
        sep = intend_create_value(INTEND_TYPE_STRING, dsep, NULL);
    }

    // Call explode stdlib function
    val = intend_call_function(ctx, "implode", 2, argv[0], sep);

    // Free separator argument
    intend_free_value(sep);

    // Write the file
    res = intend_call_function(ctx, "filewrite", 2, val, argv[1]);

    // Free string value
    intend_free_value(val);

    // Return result
    return res;
}

/*
 * Creates a pipe and returns input and output ends
 */
intend_value file_pipe(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int ends[2];
    FILE *in, *out;

    // Attempt to create the pipe
    int res = (pipe(ends) == 0) ? 1 : 0;

    // If pipe created
    if (res) {
        // Open both ends
        in = fdopen(ends[0], "r");
        out = fdopen(ends[1], "w");
        // Verify if ends opened
        res = (in && out) ? 1 : 0;
        // If pipe ends opened
        if (res) {
            // Set the reference arguments
            intend_free_value(argv[0]);
            argv[0] = intend_create_value(INTEND_TYPE_RES, in, file_free, file_get);
            intend_free_value(argv[1]);
            argv[1] = intend_create_value(INTEND_TYPE_RES, out, file_free, file_get);
        }
    }

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}
