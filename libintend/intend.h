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

#ifndef INTEND_H
#define INTEND_H

#include <stdio.h>
#include <stdarg.h>

#define INTEND_SAFE_MODE_OFF    0
#define INTEND_SAFE_MODE_ON     1

typedef void *intend_ctx;

intend_ctx intend_new_ctx();
void intend_init_ctx(intend_ctx ctx, int argc, char **argv);
void intend_inhibit_ctx(intend_ctx ctx, const char *name);
void intend_free_ctx(intend_ctx ctx);

void intend_stream_set_stdin(intend_ctx ctx, FILE *stream);
void intend_stream_set_stdout(intend_ctx ctx, FILE *stream);
void intend_stream_set_stderr(intend_ctx ctx, FILE *stream);
FILE *intend_stream_get_stdin(intend_ctx ctx);
FILE *intend_stream_get_stdout(intend_ctx ctx);
FILE *intend_stream_get_stderr(intend_ctx ctx);
void intend_stream_reset_stdin(intend_ctx ctx);
void intend_stream_reset_stdout(intend_ctx ctx);
void intend_stream_reset_stderr(intend_ctx ctx);

void intend_module_add_preload(intend_ctx ctx, char *name);

void intend_safe_mode_set(intend_ctx ctx, int safe_mode);
int intend_safe_mode_get(intend_ctx ctx);

#define INTEND_SANDBOX_NONE 0
#define INTEND_SANDBOX_RO   1
#define INTEND_SANDBOX_RW   2

void intend_sandbox_add(intend_ctx ctx, char *name, char *path, int access);

char *intend_path_resolve(intend_ctx ctx, char *path);
char *intend_path_validate(intend_ctx ctx, char *path);
char *intend_path_validate_parent(intend_ctx ctx, char *path);
int intend_path_access(intend_ctx ctx, char *path);
char *intend_path_parent(char *path);
char *intend_path_file(char *path);
char *intend_path_join(char *parent, char *file);
char *intend_path_real(intend_ctx ctx, char *path);
char *intend_path_absolute(intend_ctx ctx, char *path);

/*
 * Intend pointer type to register a file function
 */
typedef FILE *(*intend_source_callback)(intend_ctx ctx, char *path);
FILE *intend_get_file(intend_ctx ctx, char *path);

void intend_register_source_callback(intend_ctx ctx, intend_source_callback func);

int intend_parse_script_file(intend_ctx ctx, char *name);
int intend_parse_script_buffer(intend_ctx ctx, char *script);
void intend_dump_script(intend_ctx ctx);
void intend_free_script(intend_ctx ctx);

int intend_execute_script(intend_ctx ctx);

typedef void *intend_value;

intend_value intend_execute_function(intend_ctx ctx, const char *name, ...);
intend_value intend_call_function_list(intend_ctx ctx, const char *name, int argc, intend_value *argv);
intend_value intend_call_function(intend_ctx ctx, const char *name, int argc, ...);

void intend_fatal_error(intend_ctx ctx, const char *msg, ...);
void intend_nonfatal_error(intend_ctx ctx, const char *msg, ...);
void *intend_oom(void *ptr);

void intend_except_throw(intend_ctx ctx, intend_value ex);
int intend_except_try(intend_ctx ctx);
intend_value intend_except_catch(intend_ctx ctx);

int intend_value_type(intend_value val);
int intend_bool_value(intend_value val);
int intend_int_value(intend_value val);
double intend_double_value(intend_value val);
char *intend_string_value(intend_value val);
char *intend_string_ptr(intend_value val);
int intend_string_len(intend_value val);

void intend_value_dump(intend_ctx ctx, intend_value val, int depth, int skip_flag);

/*
 * Intend data types
 */
typedef enum {
    INTEND_TYPE_VOID    = 0,
    INTEND_TYPE_BOOL    = 1,
    INTEND_TYPE_INT     = 2,
    INTEND_TYPE_FLOAT   = 3,
    INTEND_TYPE_STRING  = 4,
    INTEND_TYPE_ARRAY   = 5,
    INTEND_TYPE_STRUCT  = 6,
    INTEND_TYPE_FN      = 7,
    INTEND_TYPE_RES     = 8
} intend_type;

/*
 * Intend pointer type for registered functions
 */
typedef intend_value(*intend_function)(intend_ctx ctx, unsigned int argc, intend_value *argv);

intend_value intend_create_value(intend_type type, void *data, ...);
intend_value intend_copy_value(intend_value val);
void intend_cast_value(intend_value *val, intend_type type);
void intend_free_value(intend_value val);

int intend_array_count(intend_value val);
void intend_array_add(intend_value arr, intend_value val);
intend_value intend_array_get(intend_value val, const int pos);
void intend_array_set(intend_value arr, const int pos, intend_value val);
intend_value intend_array_delete(intend_value val, const int pos);

void intend_resource_value_set(intend_value val, void *data);
void *intend_resource_value(intend_value val);
void *intend_resource_release(intend_value val);

intend_value intend_struct_get(intend_value st, const char *pos);
void intend_struct_set(intend_value st, const char *pos, intend_value val);
void intend_struct_delete(intend_value st, const char *pos);
void intend_struct_set_method(intend_value st, const char *method, intend_function vector,
                              unsigned int args, char *proto, char rettype);
intend_value intend_struct_call(intend_ctx ctx, const char *name, const char *method,
                                unsigned int argc, intend_value *argv);

char *intend_fn_name_ptr(intend_value val);

intend_value intend_get_variable(intend_ctx ctx, char *name);
void intend_set_variable(intend_ctx ctx, char *name, intend_value val);

/*
 * Intend function definition
 */
typedef struct {
    char                *name;
    intend_function     vector;
    unsigned int        args;
    char                *proto;
    char                rettype;
} intend_function_data;

/*
 * Intend variable definition
 */
typedef struct {
    char        *name;
    intend_type type;
    void        *value;
} intend_variable_data;

/*
 * Register a class in the interpreter context
 */
void intend_register_class(intend_ctx ctx, char *name, intend_function vector,
                           unsigned int args, char *proto);
/*
 * Register a function in the interpreter context
 */
void intend_register_function(intend_ctx ctx, char *name, intend_function vector,
                              unsigned int args, char *proto, char rettype);

/*
 * Register a function in the interpreter context from data struct
 */
void intend_register_function_data(intend_ctx ctx, intend_function_data *fdata);

/*
 * Register a function in the interpreter context from array of data structs
 */
void intend_register_function_array(intend_ctx ctx, intend_function_data *fdata);

/*
 * Register a variable in the interpreter context
 */
void intend_register_variable(intend_ctx ctx, char *name, intend_type type, void *value);

/*
 * Register a variable in the interpreter context from data struct
 */
void intend_register_variable_data(intend_ctx ctx, intend_variable_data *vdata);

/*
 * Register a variable in the interpreter context from array of data structs
 */
void intend_register_variable_array(intend_ctx ctx, intend_variable_data *vdata);

/*
 * Unregister a symbol (function or variable) from the interpreter context
 */
void intend_unregister_symbol(intend_ctx ctx, char *symbol);

/*
 * Unregister a function from the interpreter context from array of data structs
 */
void intend_unregister_function_array(intend_ctx ctx, intend_function_data *farray);

/*
 * Unregister a variable from the interpreter context from array of data structs
 */
void intend_unregister_variable_array(intend_ctx ctx, intend_variable_data *varray);

/*
 * Execute a system command, redirecting IO if needed and returning the exit status
 */
int intend_run(intend_ctx ctx, intend_value cmd, intend_value cmdargs,
                  intend_value in, intend_value *out, intend_value *err);

/*
 * Execute a system command in a shell, redirecting IO if needed and returning the exit status
 */
int intend_shell(intend_ctx ctx, intend_value cmdline,
                  intend_value in, intend_value *out, intend_value *err);

/*
 * Execute a system command, substituting the current process with it
 */
int intend_exec(intend_ctx ctx, intend_value cmd, intend_value cmdargs);

#endif
