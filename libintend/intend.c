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
 * Top-level Intend C Interpreter API
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// To avoid warnings on fmemopen function
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "libmisc/misc.h"
#include "libruntime/runtime.h"
#include "libparser/parser.h"
#include "libeval/eval.h"
#include "libstdlib/stdlib.h"

#include "intend.h"

/*
 * Create new interpreter context
 */
intend_ctx intend_new_ctx(int argc, char **argv)
{
    intend_state *state = state_alloc();

    symtab_stack_init(state);

    // Set default file callback
    state->get_source = (FILE * (*)(intend_state *, char *))intend_get_file;
    // Set the default streams
    state->stdin  = stdin;
    state->stdout = stdout;
    state->stderr = stderr;
    // Set backup streams to -1 to avoid use prior to execution
    state->stdin_backup = -1;
    state->stdout_backup = -1;
    state->stderr_backup = -1;

    return state;
}

/*
 * Initialize interpreter context
 */
void intend_init_ctx(intend_ctx ctx, int argc, char **argv)
{
    intend_state *state = ctx;

    stdlib_init(state, argc, argv);
    module_preload(state);
}

/*
 * Free interpreter context
 */
void intend_free_ctx(intend_ctx ctx)
{
    intend_state *state = ctx;

    intend_free_script(ctx);
    parser_teardown(state);
    symtab_stack_teardown(state);
    eval_free_floats(state);
    module_preload_teardown(state);
    module_teardown(state);
    sandbox_free(state);
    state_free(state);
}

/*
 * Remove a named entity from the interpreter context
 */
void intend_inhibit_ctx(intend_ctx ctx, const char *name)
{
    intend_state *state = ctx;

    symtab_stack_delete(state, name);
}

/*
 * Set standard input stream
 */
void intend_stream_set_stdin(intend_ctx ctx, FILE *stream)
{
    intend_state *state = ctx;

    sanity(stream);

    state->stdin = stream;
}

/*
 * Set standard output stream
 */
void intend_stream_set_stdout(intend_ctx ctx, FILE *stream)
{
    intend_state *state = ctx;

    sanity(stream);

    state->stdout = stream;
}

/*
 * Set standard error stream
 */
void intend_stream_set_stderr(intend_ctx ctx, FILE *stream)
{
    intend_state *state = ctx;

    sanity(stream);

    state->stderr = stream;
}

/*
 * Get standard input stream
 */
FILE *intend_stream_get_stdin(intend_ctx ctx)
{
    intend_state *state = ctx;

    return state->stdin;
}

/*
 * Get standard output stream
 */
FILE *intend_stream_get_stdout(intend_ctx ctx)
{
    intend_state *state = ctx;

    return state->stdout;
}

/*
 * Get standard error stream
 */
FILE *intend_stream_get_stderr(intend_ctx ctx)
{
    intend_state *state = ctx;

    return state->stderr;
}

/*
 * Reset standard input stream
 */
void intend_stream_reset_stdin(intend_ctx ctx)
{
    intend_state *state = ctx;

    if (state->stdin_backup != -1)
        dup2(state->stdin_backup, STDIN_FILENO);
}

/*
 * Reset standard output stream
 */
void intend_stream_reset_stdout(intend_ctx ctx)
{
    intend_state *state = ctx;

    if (state->stdout_backup != -1)
        dup2(state->stdout_backup, STDOUT_FILENO);
}

/*
 * Reset standard error stream
 */
void intend_stream_reset_stderr(intend_ctx ctx)
{
    intend_state *state = ctx;

    if (state->stderr_backup != -1)
        dup2(state->stderr_backup, STDERR_FILENO);
}

/*
 * Add a module to pre-load list
 */
void intend_module_add_preload(intend_ctx ctx, char *name)
{
    intend_state *state = ctx;

    module_add_preload(state, name);
}

/*
 * Set safe mode on or off and set safe mode path if safe mode on.
 * If safe_mode_path is NULL, the path available to safe mode will be only /tmp
 * if not in strict safe mode (safe_mode = 2).
 * Returns true if all went ok, or false if safe mode path is invalid
 */
void intend_safe_mode_set(intend_ctx ctx, int safe_mode)
{
    intend_state *state = ctx;

    safe_mode_set(state, safe_mode);
}

/*
 * Get safe mode from context
 */
int intend_safe_mode_get(intend_ctx ctx)
{
    intend_state *state = ctx;

    return safe_mode_get(state);
}

/*
 * Add a sandbox to context
 */
void intend_sandbox_add(intend_ctx ctx, char *name, char *path, int access)
{
    intend_state *state = ctx;

    sandbox_add(state, name, path, access);
}

/*
 * Resolve a path
 * The returned path is allocated, so don't forget to free it
 */
char *intend_path_resolve(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;

    return path_resolve(state, path);
}

/*
 * Validate a path
 * The returned path is allocated, so don't forget to free it
 */
char *intend_path_validate(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;

    return path_validate(state, path);
}

/*
 * Get a path access rights
 */
int intend_path_access(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;

    return path_access(state, path);
}

/*
 * Get parent path of a path
 * The returned path is allocated, so don't forget to free it
 */
char *intend_path_parent(char *path)
{
    return path_parent(path);
}

/*
 * Get the file of a path
 * The returned path is allocated, so don't forget to free it
 */
char *intend_path_file(char *path)
{
    return path_file(path);
}

/*
 * Join parent path and file to compose a complete path
 * The returned path is allocated, so don't forget to free it
 */
char *intend_path_join(char *parent, char *file)
{
    return path_join(parent, file);
}

/*
 * Validate a parent path, not including the filename
 * The returned path is allocated, so don't forget to free it
 */
char *intend_path_validate_parent(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;

    return path_validate_parent(state, path);
}

/*
 * Get the real path
 */
char *intend_path_real(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;
    char *resolved;
    char *real;

    if (resolved = path_resolve(state, path)) {
        real = path_real(resolved);
        free(resolved);
    }
    return real;
}

/*
 * Get the absolute path
 */
char *intend_path_absolute(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;
    char *resolved;
    char *absolute;

    if (resolved = path_resolve(state, path)) {
        absolute = path_absolute(resolved);
        free(resolved);
    }
    return absolute;
}

/*
 * Get a file pointer to a script
 */
FILE *intend_get_file(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;
    char *filename;
    FILE *file = NULL;

    filename = path_validate(state, path);

    // If filename is validated
    if (filename) {
        // Open it
        file = fopen(filename, "r");
        // Free temp filename
        free(filename);
    }

    return file;
}

/*
 * Register a source callback
 */
void intend_register_source_callback(intend_ctx ctx, intend_source_callback func)
{
    intend_state *state = ctx;

    if (func) {
        state->get_source = (FILE * (*)(intend_state *, char *))func;
    } else {
        state->get_source = (FILE * (*)(intend_state *, char *))intend_get_file;
    }
}

/*
 * Parse a script by reading source file
 */
int intend_parse_script_file(intend_ctx ctx, char *path)
{
    intend_state *state = ctx;
    FILE *file;
    int res = 0;

    // If path is available and not empty
    if (path && strlen(path) > 0) {
        // If path is sandboxed, use normal source get routine,
        // otherwise open path directly, to allow running scripts
        // not sandboxed by the application while in safe mode.
        if (sandbox_path_has_sandbox(path))
            file = (*state->get_source)(state, path);
        else
            file = fopen(path, "r");

        if (!file) {
            state->source_line = 0;
            fatal(state, "could not open input source");
            return 0;
        }

        state->source_file = path;
        state->source_line = 1;
        state->source_col  = 0;

        res = parser_run(state, file);

        fclose(file);
    }

    return res;
}

/*
 * Parse a script by reading it from a memory buffer (string)
 */
int intend_parse_script_buffer(intend_ctx ctx, char *script)
{
    intend_state *state = ctx;
    int size = strlen(script);
    FILE *file;
    int res;

    file = fmemopen(script, size, "r");
    if (!file) {
        state->source_line = 0;
        fatal(state, "out of memory in access to input buffer");
        return 0;
    }

    state->source_file = NULL;
    state->source_line = 1;
    state->source_col  = 0;

    res = parser_run(state, file);

    fclose(file);

    return res;
}

/*
 * Free script data
 */
void intend_free_script(intend_ctx ctx)
{
    intend_state *state = ctx;

    sanity(state);

    stmt_list *list = state->script;

    if (list) stmt_list_free(list);
}

/*
 * Execute a script inside a given interpreter context
 */
int intend_execute_script(intend_ctx ctx)
{
    intend_state *state = ctx;
    stmt_list *list = state->script;

    // Return -1 if no parsed script in the context
    if (!list) return -1;

    // Save global streams
    save_std_streams(state);

    // Set global safe mode vars
    safe_mode_vars(state);

    state->source_line = 1;
    state->source_col  = 0;

    eval_stmt_list(state, list, 0);

    return state->exit_value;
}

/*
 * Dump a script to standard output
 */
void intend_dump_script(intend_ctx ctx)
{
    intend_state *state = ctx;

    stmt_list *list = state->script;

    stmt_list_dump(state, list, 0);
}

/*
 * Execute an Intend C function, creating an argument list from suplied arguments
 */
intend_value intend_execute_function(intend_ctx ctx, const char *name, ...)
{
    intend_state *state = ctx;
    signature *sig;
    char *proto;
    symtab_entry *entry;
    va_list ap;
    int i, argc = 0;
    value **argv, *res;

    // Save global streams
    save_std_streams(state);

    // Set global safe mode vars
    safe_mode_vars(state);

    entry = symtab_stack_lookup(state, name);
    if (!entry || entry->type != SYMTAB_ENTRY_FUNCTION) {
        return NULL;
    }
    sig = &(entry->entry_u.fnc.sigs[0]);
    proto = sig->proto;

    if (strpbrk(proto, "?*aAcCpPrR")) {
        return NULL;
    }

    argv = malloc(strlen(proto) * sizeof(value *));
    if (!argv) {
        return NULL;
    }

    va_start(ap, name);
    while (*proto) {
        switch (*proto) {
            case 'v':
            case 'V':
                argv[argc++] = value_make_void();
                break;
            case 'b':
            case 'B':
                argv[argc++] = value_make_bool(va_arg(ap, int));
                break;
            case 'i':
            case 'I':
                argv[argc++] = value_make_int(va_arg(ap, int));
                break;
            case 'f':
            case 'F':
                argv[argc++] = value_make_float(va_arg(ap, double));
                break;
            case 's':
            case 'S':
                argv[argc++] = value_make_string(va_arg(ap, char *));
                break;
        }
        ++proto;
    }
    va_end(ap);

    symtab_stack_enter(state);
    res = call_named_function(state, name, argc, argv);
    symtab_stack_leave(state);

    for (i = 0; i < argc; ++i) {
        value_free(argv[i]);
    }
    free(argv);

    return res;
}

/*
 * Call an Intend C function with the suplied argument list
 */
intend_value intend_call_function_list(intend_ctx ctx, const char *name, int argc, intend_value *argv)
{
    intend_state *state = ctx;
    value *res;

    // Save global streams
    save_std_streams(state);

    // Set global safe mode vars
    safe_mode_vars(state);

    symtab_stack_enter(state);
    res = call_named_function(state, name, argc, (value **)argv);
    symtab_stack_leave(state);

    return res;
}

/*
 * Call an Intend C function with arguments
 */
intend_value intend_call_function(intend_ctx ctx, const char *name, int argc, ...)
{
    intend_state *state = ctx;
    value *res;
    va_list ap;
    value **argv;
    int i;

    // Build argument list
    argv = oom(calloc(argc, sizeof(value *)));
    va_start(ap, argc);
    for (i = 0; i < argc; i++) {
        argv[i] = va_arg(ap, value *);
    }

    // Call the function
    symtab_stack_enter(state);
    res = call_named_function(state, name, argc, argv);
    symtab_stack_leave(state);

    // Free argument list
    free(argv);

    return res;
}

/*
 * Write fatal error message
 *
 * This function prints a fatal error message. Execution of the
 * running program is aborted.
 */
void intend_fatal_error(intend_ctx ctx, const char *msg, ...)
{
    intend_state *state = ctx;
    va_list ap;

    va_start(ap, msg);
    errmsg(state, 1, msg, ap);
    va_end(ap);
}

/*
 * Write nonfatal error message
 *
 * This function prints an error message. Execution of the
 * running program continues as normal.
 */
void intend_nonfatal_error(intend_ctx ctx, const char *msg, ...)
{
    intend_state *state = ctx;
    va_list ap;

    va_start(ap, msg);
    errmsg(state, 0, msg, ap);
    va_end(ap);
}

/*
 * Check if pointer is NULL and raise out off memory error
 * For use with malloc & friends
 */
void *intend_oom(void *ptr)
{
    return oom(ptr);
}

/*
 * Throw an exception
 */
void intend_except_throw(intend_ctx ctx, intend_value ex)
{
    intend_state *state = ctx;
    value *except = ex;

    except_throw(state, except);
}

/*
 * Starts a try section
 */
int intend_except_try(intend_ctx ctx)
{
    intend_state *state = ctx;

    return except_try(state);
}

/*
 * Catches any exception thrown in the mean time to a prior
 * intend_except_try. This call ends the try section.
 * The return value must be freed with value_free
 */
intend_value intend_except_catch(intend_ctx ctx)
{
    intend_state *state = ctx;
    intend_value ex;

    ex = (intend_value)except_catch(state);

    if (!ex)
        ex = intend_create_value(INTEND_TYPE_VOID, NULL);

    return ex;
}

/*
 * Get value type of function argument or result
 */
int intend_value_type(intend_value val)
{
    value *in = val;

    return in->type;
}

/*
 * Dump an intend variable. Used mainly for debug purposes
 */
void intend_value_dump(intend_ctx ctx, intend_value val, int depth, int skip_flag)
{
    intend_state *state = ctx;

    value_dump(state, (value *)val, depth, skip_flag);
}

/*
 * Get bool value of function argument or result
 */
int intend_bool_value(intend_value val)
{
    return intend_int_value(val);
}

/*
 * Get int value of function argument or result
 */
int intend_int_value(intend_value val)
{
    value *cast, *in = val;
    int res;

    cast = value_cast(NULL, in, VALUE_TYPE_INT);
    res = INT_OF(cast);
    value_free(cast);

    return res;
}

/*
 * Get double value of function argument or result
 */
double intend_double_value(intend_value val)
{
    value *cast, *in = val;
    double res;

    cast = value_cast(NULL, in, VALUE_TYPE_FLOAT);
    res = FLOAT_OF(cast);
    value_free(cast);

    return res;
}

/*
 * Get string value of function argument or result
 * Don't forget to free the string pointer.
 */
char *intend_string_value(intend_value val)
{
    value *cast, *in = val;
    char *res;

    cast = value_cast(NULL, in, VALUE_TYPE_STRING);
    res = malloc(STRLEN_OF(cast) + 1);
    if (res) {
        if (STRLEN_OF(cast) > 0) {
            strcpy(res, STR_OF(cast));
        } else {
            res[0] = 0;
        }
    }
    value_free(cast);

    return res;
}

/*
 * Get string pointer of function argument or result
 * This function returns the actual string pointer of
 * the value. Don't free the string pointer.
 */
char *intend_string_ptr(intend_value val)
{
    value *in = val;
    return STR_OF(in);
}

/*
 * Get string length of function argument or result
 */
int intend_string_len(intend_value val)
{
    value *in = val;
    return STRLEN_OF(in);
}

/*
 * Get resource value of function argument or result
 * Be aware that the pointer returned is the actual pointer of
 * the resource, because of the opacity of its content.
 * Do not free it, let the language handle it.
 */
void *intend_resource_value(intend_value val)
{
    value *in = val;

    return RES_OF(in);
}

/*
 * Get resource release of function argument or result
 */
void *intend_resource_release(intend_value val)
{
    value *in = val;

    return RESRELEASE_OF(in);
}

/*
 * Get the array value at index pos of function argument
 * or result
 */
intend_value intend_array_get(intend_value val, const int pos)
{
    value *in = val;

    return (intend_value)value_get_array(in, pos);
}

/*
 * Delete the array value at index pos of function argument
 * or result
 */
intend_value intend_array_delete(intend_value val, const int pos)
{
    value *in = val;

    value_delete_array(in, pos);
}

/*
 * Get the the num of items in array of function argument
 * or result
 */
int intend_array_count(intend_value val)
{
    value *in = val;

    return ARRLEN_OF(in);
}

/*
 * Get the name of the function pointed by argument or result
 */
char *intend_fn_name_ptr(intend_value val)
{
    value *in = val;
    signature *sig = FNSIG_OF(in);

    return sig->name;
}

/*
 * Create an intend value of the desired type and data.
 * Don't forget to free it when not needed anymore.
 * If you use it as a function result, you don't need to free it.
 */
intend_value intend_create_value(intend_type type, void *data, ...)
{
    va_list ap;
    void *arg;
    void *arg2;
    value *v;
    signature *sig;

    switch (type) {
        case VALUE_TYPE_BOOL:
            v = value_make_bool(*((int *)data));
            break;
        case VALUE_TYPE_INT:
            v = value_make_int(*((int *)data));
            break;
        case VALUE_TYPE_FLOAT:
            v = value_make_float(*((double *)data));
            break;
        case VALUE_TYPE_STRING:
            va_start(ap, data);
            arg = va_arg(ap, int *);
            va_end(ap);
            if (arg) {
                v = value_make_memstring((char *)data, *((int *)arg));
            } else {
                v = value_make_string((char *)data);
            }
            break;
        case VALUE_TYPE_ARRAY:
            v = value_make_array();
            if (data) {
                value_add_to_array(v, (value *)data);

                va_start(ap, data);
                while (arg = va_arg(ap, void *)) {
                    value_add_to_array(v, (value *)arg);
                }
                va_end(ap);
            }
            break;
        case VALUE_TYPE_RES:
            va_start(ap, data);
            arg = va_arg(ap, void (*)(void *));
            arg2 = va_arg(ap, void * (*)(void *));
            va_end(ap);
            v = value_make_resource(data, arg, arg2);
            break;
        case VALUE_TYPE_STRUCT:
            // Create the struct
            v = value_make_struct();
            // Add the fields and methods
            if (data) {
                va_start(ap, data);
                arg = data;
                arg2 = va_arg(ap, void *);
                while (arg) {
                    value_set_struct(v, (char *)arg, (value *)arg2);
                    arg = va_arg(ap, void *);
                    if (arg) arg2 = va_arg(ap, void *);
                }
                va_end(ap);
            }
            break;
        case VALUE_TYPE_FN:
            va_start(ap, data);
            arg = va_arg(ap, void *);
            // Setup signature
            sig = symtab_stack_get_function((intend_state *)arg, (char *)data);
            // Create fn
            v = value_make_fn(sig);
            break;
        case VALUE_TYPE_VOID:
        default:
            v = value_make_void();
    }
    return (intend_value)v;
}

/*
 * Copy an intend value returning a new copy equal to the received.
 * Don't forget to free it when not needed anymore.
 * If you use it as a function result, you don't need to free it.
 */
intend_value intend_copy_value(intend_value val)
{
    value *v = (value *)val;

    return value_copy(v);
}

/*
 * Cast an intend value to a determined type.
 */
void intend_cast_value(intend_value *val, intend_type type)
{
    value **v = (value **)val;

    value_cast_inplace(NULL, v, type);
}

/*
 * Set resource value of function argument or result
 */
void intend_resource_value_set(intend_value val, void *data)
{
    value *in = val;

    RES_OF(in) = data;
}

/*
 * Add a value to an array
 */
void intend_array_add(intend_value arr, intend_value val)
{
    value *array = arr;
    value *elem = val;

    value_add_to_array(array, elem);
}

/*
 * Set an array element to the given value
 */
void intend_array_set(intend_value arr, const int pos, intend_value val)
{
    value *array = arr;
    value *elem = val;

    value_set_array(array, pos, elem);
}

/*
 * Get a structure field
 */
intend_value intend_struct_get(intend_value st, const char *pos)
{
    return (intend_value)value_get_struct((value *)st, pos);
}

/*
 * Set a structure field
 */
void intend_struct_set(intend_value st, const char *pos, intend_value val)
{
    value_set_struct((value *)st, pos, (value *)val);
}

/*
 * Delete struct field
 */
void intend_struct_delete(intend_value st, const char *pos)
{
    value_delete_struct((value *)st, pos);
}

/*
 * Set a struct method field (or class instance method)
 */
void intend_struct_set_method(intend_value st, const char *method, intend_function vector,
                              unsigned int args, char *proto, char rettype)
{
    value_set_struct_method((value *)st, method, (call_func)vector, args, proto, rettype);
}

/*
 * Call a struct method (or class instance method)
 */
intend_value intend_struct_call(intend_ctx ctx, const char *name, const char *method,
                                unsigned int argc, intend_value *argv)
{
    intend_state *state = ctx;

    return value_call_struct_method(state, name, method, argc, (value **)argv);
}

/*
 * Free created value
 */
void intend_free_value(intend_value val)
{
    value *v = val;

    value_free(v);
}

/*
 * Get a variable from the interpreter context
 */
intend_value intend_get_variable(intend_ctx ctx, char *name)
{
    intend_state *state = ctx;

    return (intend_value)symtab_stack_get_variable(state, name);
}

/*
 * Set a variable to the interpreter context
 */
void intend_set_variable(intend_ctx ctx, char *name, intend_value val)
{
    intend_state *state = ctx;

    symtab_stack_add_variable(state, name, (value *)val);
}

/*
 * Register a class in the interpreter context
 */
void intend_register_class(intend_ctx ctx, char *name, intend_function vector,
                           unsigned int args, char *proto)
{
    intend_state *state = ctx;

    register_class(state, name, vector, args, proto);
}

/*
 * Register a function in the interpreter context
 */
void intend_register_function(intend_ctx ctx, char *name, intend_function vector,
                              unsigned int args, char *proto, char rettype)
{
    intend_state *state = ctx;
    call_func func = (call_func) vector;

    register_function(state, name, func, args, proto, rettype);
}

/*
 * Register a function in the interpreter context from data struct
 */
void intend_register_function_data(intend_ctx ctx, intend_function_data *fdata)
{
    intend_state *state = ctx;

    register_function_data(state, (register_func_data *)fdata);
}

/*
 * Register a function in the interpreter context from array of data structs
 */
void intend_register_function_array(intend_ctx ctx, intend_function_data *farray)
{
    intend_state *state = ctx;

    register_function_array(state, (register_func_data *)farray);
}

/*
 * Register a variable in the interpreter context
 */
void intend_register_variable(intend_ctx ctx, char *name, intend_type type, void *value)
{
    intend_state *state = ctx;

    register_variable(state, name, (value_type)type, value);
}

/*
 * Register a variable in the interpreter context from data struct
 */
void intend_register_variable_data(intend_ctx ctx, intend_variable_data *vdata)
{
    intend_state *state = ctx;

    register_variable_data(state, (register_var_data *)vdata);
}

/*
 * Register a variable in the interpreter context from array of data structs
 */
void intend_register_variable_array(intend_ctx ctx, intend_variable_data *varray)
{
    intend_state *state = ctx;

    register_variable_array(state, (register_var_data *)varray);
}

/*
 * Unregister a symbol (function or variable) from the interpreter context
 */
void intend_unregister_symbol(intend_ctx ctx, char *symbol)
{
    intend_state *state = ctx;

    unregister_symbol(state, symbol);
}

/*
 * Unregister a function from the interpreter context from array of data structs
 */
void intend_unregister_function_array(intend_ctx ctx, intend_function_data *farray)
{
    intend_state *state = ctx;

    unregister_function_array(state, (register_func_data *)farray);
}

/*
 * Unregister a variable from the interpreter context from array of data structs
 */
void intend_unregister_variable_array(intend_ctx ctx, intend_variable_data *varray)
{
    intend_state *state = ctx;

    unregister_variable_array(state, (register_var_data *)varray);
}

/*
 * Execute a system command, redirecting IO if needed and returning the exit status
 */
int intend_run(intend_ctx ctx, intend_value cmd, intend_value cmdargs,
               intend_value in, intend_value *out, intend_value *err)
{
    intend_state *s = (intend_state *)ctx;
    int res;

    res = system_run(s, (value *)cmd, (value *)cmdargs, (value *)in, (value **)out, (value **)err);

    // Return result
    return res;
}

/*
 * Execute a system command in a shell, redirecting IO if needed and returning the exit status
 */
int intend_shell(intend_ctx ctx, intend_value cmdline,
                 intend_value in, intend_value *out, intend_value *err)
{
    intend_state *s = (intend_state *)ctx;
    int res;

    res = system_shell(s, (value *)cmdline, (value *)in, (value **)out, (value **)err);

    // Return result
    return res;
}

/*
 * Execute a system command, substituting the current process with it
 */
int intend_exec(intend_ctx ctx, intend_value cmd, intend_value cmdargs)
{
    intend_state *s = (intend_state *)ctx;
    int res;

    res = system_exec(s, (value*)cmd, (value *)cmdargs);

    // Return error (this should never be reached)
    return res;
}
