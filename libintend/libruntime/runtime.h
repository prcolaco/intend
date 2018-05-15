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
 * Intend C Runtime library
 *
 * This library provides basic typed values, hashed symbol tables,
 * type-safe function calls, and error message printing.
 */

#ifndef INTEND_RUNTIME_H
#define INTEND_RUNTIME_H

#include "../libmisc/misc.h"

#define DEBUG 0


/*
 * --- Basic typed values ---
 */

/*
 * Value types
 */
typedef enum {
    VALUE_TYPE_VOID = 0,
    VALUE_TYPE_BOOL = 1,
    VALUE_TYPE_INT  = 2,
    VALUE_TYPE_FLOAT    = 3,
    VALUE_TYPE_STRING   = 4,
    VALUE_TYPE_ARRAY    = 5,
    VALUE_TYPE_STRUCT   = 6,
    VALUE_TYPE_FN       = 7,
    VALUE_TYPE_RES  = 8
} value_type;

/*
 * Minimum string chars allocated
 */
#define STRING_MIN_CHARS 64

/*
 * String value structure
 */
typedef struct {
    int     len;
    int     size;
    char    *value;
} value_string;

/*
 * Minimum array growth unit
 */
#define ARRAY_GROWTH 32

/*
 * Array value structure
 */
typedef struct symtab SYMTAB;
typedef struct symtab_entry ENTRY;
typedef struct {
    int         len;
    int         size;
    SYMTAB      *keys;
    ENTRY       **values;
} value_array;

/*
 * Resource value structure
 */
typedef struct {
    int     refcount;
    void    (*release)(void *);
    void    *(*get)(void *);
    void    *data;
} value_res;

/*
 * Value union
 */
typedef struct value {
    value_type          type;
    union {
        int             bool_val;
        int             int_val;
        double          float_val;
        value_string    string_val;
        value_array     array_val;
        value_res       *res_val;
        void            *struct_val;
        void            *fn_val;
    } value_u;
} value;

/*
 * Accessor macros
 */
#define TYPE_OF(v) ((v)->type)
#define BOOL_OF(v) ((v)->value_u.bool_val)
#define INT_OF(v) ((v)->value_u.int_val)
#define FLOAT_OF(v) ((v)->value_u.float_val)
#define STR_OF(v) ((v)->value_u.string_val.value)
#define STRLEN_OF(v) ((v)->value_u.string_val.len)
#define STRSIZE_OF(v) ((v)->value_u.string_val.size)
#define ARR_OF(v) ((v)->value_u.array_val.values)
#define ARRLEN_OF(v) ((v)->value_u.array_val.len)
#define ARRSIZE_OF(v) ((v)->value_u.array_val.size)
#define ARRKEYS_OF(v) ((v)->value_u.array_val.keys)
#define STRUCT_OF(v) ((v)->value_u.struct_val)
#define RES_OF(v) ((v)->value_u.res_val->data)
#define RESREF_OF(v) ((v)->value_u.res_val->refcount)
#define RESGET_OF(v) ((v)->value_u.res_val->get)
#define RESRELEASE_OF(v) ((v)->value_u.res_val->release)
#define FNSIG_OF(v) ((v)->value_u.fn_val)

/*
 * Memory management
 */
value *value_alloc(value_type type);
void value_cleanup(value *val);
void value_free(value *val);

/*
 * Resource type management
 */
int reserve_resource_type(intend_state *s);

/*
 * Constructors
 */
value *value_make_void(void);
value *value_make_bool(int val);
value *value_make_int(int val);
value *value_make_float(double val);
value *value_make_string(const char *str);
value *value_make_memstring(const void *buf, int len);
value *value_make_array(void);
value *value_make_struct(void);
value *value_make_fn(void *sig);
value *value_make_resource(void *data, void (*release)(void *), void *(*get)(void *));

/*
 * Value copy
 */
value *value_copy_to(value *copy, value *val);
value *value_copy(value *val);

/*
 * Array management
 */
void value_add_to_array(value *arr, value *val);
void value_set_array(value *arr, int pos, value *val);
value *value_get_array(value *arr, int pos);
void value_delete_array(value *arr, int pos);

void value_add_to_key_array(value *arr, char *pos, value *val);
void value_set_key_array(value *arr, char *pos, value *val);
value *value_get_key_array(value *arr, char *pos);
void value_delete_key_array(value *arr, char *pos);

/*
 * Struct management
 */
void value_set_struct(value *st, const char *pos, value *val);
value *value_get_struct(value *st, const char *pos);
void value_delete_struct(value *st, const char *pos);

/*
 * Casting and compatibility
 */
value *value_cast(intend_state *s, value *val, value_type type);
void value_cast_inplace(intend_state *s, value **val, value_type type);
int value_str_compat(const value *val);

/*
 * Dump printing
 */
void value_dump(intend_state *s, const value *val, int depth, int skip_flag);

/*
 * --- function calls ---
 */

/*
 * Function call types
 */
typedef enum {
    FUNCTION_TYPE_BUILTIN = 1,
    FUNCTION_TYPE_USERDEF = 2
} function_type;

/*
 * Pointer type for builtin functions
 */
typedef value *(*call_func)(intend_state *s, unsigned int argc, value **argv);

/*
 * Pointer type for user-defined functions
 */
typedef value *(*user_func)(intend_state *s, void *data, void *def,
                            unsigned int args, unsigned int argc, value **argv);

/*
 * Function call signature
 */
typedef struct {
    function_type   type;
    unsigned int    args;
    char        *name;
    char        *proto;
    char        rettype;
    void        *data;
    void        *def;
    union {
        call_func   builtin_vector;
        user_func   userdef_vector;
    } call_u;
} signature;

/*
 * Type to character conversion and back
 */
char call_typechar(const value *val);
const char *call_typename(value_type type);
int call_chartype(char val);

/*
 * Memory managment
 */
signature *call_sig_alloc(void);
signature *call_sig_copy_to(signature *copy, const signature *sig);
signature *call_sig_copy(const signature *sig);
void call_sig_cleanup(signature *sig);
void call_sig_free(signature *sig);
signature *call_sig_builtin(const char *name, unsigned int args, const char *p, call_func v);

/*
 * Function calls
 */
void call_check(intend_state *s, const char *n, signature *sig,
                unsigned int, value **);
value *call_function(intend_state *s, signature *sig, unsigned int, value **);
value *call_named_function(intend_state *s, const char *n, unsigned int c,
                           value **argv);

/*
 * Struct method calls
 */
void value_set_struct_method(value *st, const char *method, call_func vector, unsigned int args,
                             char *proto, char rettype);
value *value_call_struct_method(intend_state *s, const char *name, const char *method,
                                unsigned int argc, value **argv);


/*
 * --- symbol tables ---
 */

/*
 * Symbol table size
 */
#define SYMTAB_DEFAULT_ORDER    11

/*
 * Node default growth unit
 */
#define SYMTAB_NODE_GROWTH      20

/*
 * Function signature growth
 */
#define FUNCTION_SIG_GROWTH     4

/*
 * Entry types
 */
typedef enum {
    SYMTAB_ENTRY_VAR    = 1,
    SYMTAB_ENTRY_FUNCTION   = 2,
    SYMTAB_ENTRY_CLASS  = 3
} symtab_entry_type;

/*
 * Class types
 */
typedef enum {
    CLASS_TYPE_BUILTIN = 1,
    CLASS_TYPE_USERDEF = 2
} class_type;

/*
 * Function entry
 */
typedef struct {
    int           len;              /* number of registered signatures */
    int           size;             /* total of allocated slots */
    signature     *sigs;            /* signatures allocated array */
} func_decl;

/*
 * Class entry
 */
typedef struct {
    class_type  type;               /* type of class */
    char        *parent;            /* name of parent class */
    void        *definition;        /* definition statement list for user defined classes */
    signature   *constructor;       /* constructor for builtin classes */
} class_decl;

/*
 * Symbol table entry
 */
typedef struct symtab_entry {
    char                *symbol;    /* symbol name */
    symtab_entry_type   type;       /* symbol type */
    union {
        value           var;        /* variable data */
        func_decl       fnc;        /* function data */
        class_decl      cls;        /* class data */
    } entry_u;
} symtab_entry;

/*
 * Symbol table node
 */
typedef struct {
    unsigned int        len;
    unsigned int        size;
    symtab_entry        *entries;
} symtab_node;

/*
 * Symbol table
 */
typedef struct symtab {
    unsigned int        order;
    symtab_node         **nodes;
} symtab;

/*
 * Memory management
 */
symtab_entry *symtab_entry_alloc(void);
void symtab_entry_free(symtab_entry *entry);
void symtab_entry_recycle(symtab_entry *entry);
void symtab_entry_cleanup(symtab_entry *entry);
symtab_node *symtab_node_alloc(void);
void symtab_node_free(symtab_node *node);
symtab *symtab_alloc(unsigned int order);
symtab *symtab_copy(symtab *sym);
void symtab_free(symtab *symtab);

/*
 * Symbol table entry manipulation
 */
symtab_entry *symtab_add(symtab *symtab, symtab_entry entry);
symtab_entry *symtab_add_variable(symtab *symtab, const char *name, value *val);
symtab_entry *symtab_add_function(symtab *symtab, const char *name, signature *s);
symtab_entry *symtab_add_class(symtab *, const char *n, const char *p, void *def);
symtab_entry *symtab_add_builtin_class(symtab *symtab, const char *name, const char *parent,
                                       signature *con);
symtab_entry *symtab_lookup(symtab *symtab, const char *symbol);
symtab_entry *symtab_get(symtab *symtab, const int index);
value *symtab_get_variable(symtab *symtab, const char *name);
signature *symtab_get_function(symtab *symtab, const char *name);
void symtab_delete(symtab *symtab, const char *symbol);
int symtab_num_entries(symtab *sym);

/*
 * Symbol table stack
 */
void symtab_stack_init(intend_state *s);
void symtab_stack_teardown(intend_state *s);
void symtab_stack_enter(intend_state *s);
void symtab_stack_leave(intend_state *s);
symtab *symtab_stack_pop(intend_state *s);
unsigned int symtab_stack_depth(intend_state *s);
void symtab_stack_add(intend_state *s, symtab_entry entry);
void symtab_stack_add_global_variable(intend_state *s, const char *name, value *val);
void symtab_stack_add_global_function(intend_state *s, const char *name,
                                      signature *sig);
void symtab_stack_add_variable(intend_state *s, const char *name, value *val);
void symtab_stack_add_function(intend_state *s, const char *name,
                               signature *sig);
void symtab_stack_add_class(intend_state *s, const char *name,
                            const char *parent, void *d);
symtab_entry *symtab_stack_lookup(intend_state *s, const char *symbol);
value *symtab_stack_get_variable(intend_state *s, const char *name);
signature *symtab_stack_get_function(intend_state *s, const char *name);
int symtab_stack_local(intend_state *s, const char *symbol);
void symtab_stack_delete(intend_state *s, const char *symbol);

/*
 * Standard Stream control
 */
void save_std_streams(intend_state *state);

/*
 * Exception handling
 */
void except_throw(intend_state *s, value *except);
int except_try(intend_state *s);
value *except_catch(intend_state *s);

/*
 * Safe mode definitions
 */

#define SAFE_MODE_OFF       0
#define SAFE_MODE_ON        1

/*
 * Set global safe mode vars
 */
void safe_mode_vars(intend_state *state);

/*
 * Set safe mode on or off
 */
void safe_mode_set(intend_state *state, int safe_mode);

/*
 * Get safe mode from state
 */
int safe_mode_get(intend_state *state);

/*
 * Sandbox access rights
 */
#define SANDBOX_NONE    0
#define SANDBOX_RO      1
#define SANDBOX_RW      2

/*
 * Add a sandbox to state
 */
void sandbox_add(intend_state *state, char *name, char *path, int access);

/*
 * Free sandbox data from state
 */
void sandbox_free(intend_state *state);

/*
 * Get safe path from state
 */
char *sandbox_get(intend_state *state, char *name);

/*
 * Get safebox access rights from state
 */
int sandbox_get_access(intend_state *s, char *name);

/*
 * Check if path has a sandbox (don't validate it)
 */
int sandbox_path_has_sandbox(char *path);

/*
 * Get sandbox path access rights
 */
int sandbox_get_path_access(intend_state *s, char *path);

/*
 * Get sandbox dir from path
 * Don't forget to free the result if not NULL
 */
char *sandbox_get_from_path(intend_state *s, char *path);

/*
 * Resolve the sandbox path, returning NULL not sandboxed
 * Don't forget to free the result if not NULL
 */
char *sandbox_resolve_path(intend_state *s, char *path);

/*
 * Validate the sandbox path, returning NULL if invalid
 * Don't forget to free the result if not NULL
 */
char *sandbox_validate_path(intend_state *s, char *path);

/*
 * Resolve a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_resolve(intend_state *s, char *path);

/*
 * Validate a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_validate(intend_state *s, char *path);

/*
 * Get a path access rights
 */
int path_access(intend_state *s, char *path);

/*
 * Get parent path of a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_parent(char *path);

/*
 * Get the file of a path
 * The returned path is allocated, so don't forget to free it
 */
char *path_file(char *path);

/*
 * Join parent path and file to compose a complete path
 * The returned path is allocated, so don't forget to free it
 */
char *path_join(char *parent, char *file);

/*
 * Validate a parent path, not including the filename
 * The returned path is allocated, so don't forget to free it
 */
char *path_validate_parent(intend_state *s, char *path);

/*
 * Get real path of path (use realpath function)
 * The returned path is allocated, so don't forget to free it
 */
char *path_real(char *path);

/*
 * Get the absolute path like realpath() but without resolving symlinks,
 * so that it is possible to have symlinks inside sandboxes... :D
 * (This function is based on the glibc one with the symlink part
 * commented and some minor changes...)
 * The returned path is allocated, so don't forget to free it
 */
char *path_absolute(char *path);

/*
 * register function structure definition
 */
typedef struct {
    char            *name;
    call_func       vector;
    unsigned int    args;
    char            *proto;
    char            rettype;
} register_func_data;

/*
 * register variable structure definition
 */
typedef struct {
    char            *name;
    value_type  type;
    void            *value;
} register_var_data;

/*
 * Register functions
 */
void register_function(intend_state *s, char *name, call_func vector, unsigned int args,
                       char *proto, char rettype);
void register_function_data(intend_state *s, register_func_data *fdata);
void register_function_array(intend_state *s, register_func_data *farray);

/*
 * Register variables
 */
void register_variable(intend_state *s, char *name, value_type type, void *value);
void register_variable_data(intend_state *s, register_var_data *vdata);
void register_variable_array(intend_state *s, register_var_data *varray);

/*
 * Unregister symbols (functions or variables)
 */
void unregister_symbol(intend_state *s, char *symbol);
void unregister_symbol_array(intend_state *s, register_var_data *varray);

/*
 * Add a module to pro-load list
 */
void module_add_preload(intend_state *s, char *name);

/*
 * Load all pre-load modules
 */
void module_preload(intend_state *s);

/*
 * Load a module
 */
int module_load(intend_state *s, char *name);

/*
 * Check if a module is loaded
 */
int module_is_loaded(intend_state *s, char *name);

/*
 * Unload a module
 */
int module_unload(intend_state *s, char *name);

/*
 * Module teardown
 */
void module_teardown(intend_state *s);

/*
 * Execute a system command, redirecting IO if needed and returning the exit status
 */
int system_run(intend_state *s, value *cmd, value *cmdargs,
               value *in, value **out, value **err);

/*
 * Execute a system command in a shell, redirecting IO if needed and returning the exit status
 */
int system_shell(intend_state *s, value *cmdline,
                 value *in, value **out, value **err);

/*
 * Execute a system command, substituting the current process with it
 */
int system_exec(intend_state *s, value *cmd, value *cmdargs);

#endif
