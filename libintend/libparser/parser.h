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
 * Intend C Parser library
 *
 * Provides function to parse source code into statement and
 * expression structures.
 */

#ifndef INTEND_PARSER_H
#define INTEND_PARSER_H

#include <stdio.h>

#include "../libmisc/misc.h"

#define DEBUG   0

/*
 * -- parser internals --
 */

/*
 * Error reporting
 */
void parser_error(intend_state *state, char *msg);

/*
 * Include file handling in lexer
 */
void parser_enter_include(intend_state *state, char *path);
void parser_include_teardown(intend_state *state);

/*
 * -- statements --
 */

/*
 * Statement types
 */
typedef enum {
    STMT_NOP        = 0,
    STMT_BLOCK      = 1,
    STMT_IF         = 2,
    STMT_IF_ELSE    = 3,
    STMT_WHILE      = 4,
    STMT_DO         = 5,
    STMT_FOR        = 6,
    STMT_CONTINUE   = 7,
    STMT_BREAK      = 8,
    STMT_RETURN     = 9,
    STMT_EXPR       = 10,
    STMT_FUNC       = 11,
    STMT_SWITCH     = 12,
    STMT_CASE       = 13,
    STMT_DEFAULT    = 14,
    STMT_TRY        = 15,
    STMT_THROW      = 16,
    STMT_CLASS      = 17
} stmt_type;

typedef struct stmt STMT;
typedef struct stmt_list STMT_LIST;
typedef struct expr EXPR;

/*
 * Statements
 */
typedef struct stmt {
    stmt_type       type;
    EXPR            *init;
    EXPR            *expr;
    EXPR            *guard;
    STMT            *true_case;
    STMT            *false_case;
    STMT_LIST       *block;
    char            thru;
    unsigned int    args;
    char            *name;
    char            *proto;
    char            rettype;
    char            **names;
} stmt;

/*
 * Statement memory management
 */
stmt *stmt_alloc(void);
void stmt_free(stmt *st);

/*
 * Statement list
 */
typedef struct {
    unsigned int    len;
    stmt            **list;
} stmt_list;

/*
 * Statement list management
 */
stmt_list *stmt_list_alloc();
void stmt_list_free(stmt_list *list);
void stmt_list_push(stmt_list *list, stmt *stmt);
stmt *stmt_list_pop(stmt_list *list);

/*
 * Statement stack management
 */
void stmt_stack_enter(intend_state *s);
stmt_list *stmt_stack_leave(intend_state *s, int free);
void stmt_stack_push(intend_state *s, stmt *stmt);
stmt *stmt_stack_pop(intend_state *s);
void stmt_stack_teardown(intend_state *s);

/*
 * -- expressions --
 */

/*
 * Expression types
 */
typedef enum {
    EXPR_NOP            = 0,
    EXPR_ASSIGN         = 1,
    EXPR_ASSIGN_ARRAY   = 2,
    EXPR_CAST           = 3,
    EXPR_CALL           = 4,
    EXPR_REF            = 5,
    EXPR_REF_ARRAY      = 6,
    EXPR_CONST_BOOL     = 7,
    EXPR_CONST_INT      = 8,
    EXPR_CONST_FLOAT    = 9,
    EXPR_CONST_STRING   = 10,
    EXPR_CONST_VOID     = 11,
    EXPR_INFIX          = 12,
    EXPR_POSTFIX        = 13,
    EXPR_PREFIX         = 14,
    EXPR_FIELD          = 15,
    EXPR_INDEX          = 16,
    EXPR_NEW            = 17,
    EXPR_STATIC         = 18,
    EXPR_METHOD         = 19,
    EXPR_STATIC_REF     = 20,
    EXPR_LAMBDA         = 21,
    EXPR_IF             = 22,
    EXPR_PASS_REF       = 23,
    EXPR_FILE           = 24,
    EXPR_LINE           = 25
} expr_type;

/*
 * Operator types
 */
typedef enum {
    OPTYPE_PLUS         = 1,
    OPTYPE_MINUS        = 2,
    OPTYPE_MUL          = 3,
    OPTYPE_DIV          = 4,
    OPTYPE_MOD          = 5,
    OPTYPE_BOOL_AND     = 6,
    OPTYPE_BOOL_OR      = 7,
    OPTYPE_AND          = 8,
    OPTYPE_OR           = 9,
    OPTYPE_XOR          = 10,
    OPTYPE_POW          = 11,
    OPTYPE_EQUAL        = 12,
    OPTYPE_NOT_EQUAL    = 13,
    OPTYPE_SEQ          = 14,
    OPTYPE_LEQ          = 15,
    OPTYPE_SMALLER      = 16,
    OPTYPE_LARGER       = 17,
    OPTYPE_POSTINC      = 18,
    OPTYPE_POSTDEC      = 19,
    OPTYPE_NOT          = 20,
    OPTYPE_PREINC       = 21,
    OPTYPE_PREDEC       = 22,
    OPTYPE_NEG          = 23,
    OPTYPE_LSHIFT       = 24,
    OPTYPE_RSHIFT       = 25
} op_type;

/*
 * Expressions
 */
typedef struct expr {
    expr_type   type;
    int             line;
    char            *file;
    EXPR            *inner;
    char            *name;
    char            *tname;
    EXPR            *index;
    EXPR            *elif;
    unsigned int    argc;
    EXPR            **argv;
    op_type         op;
    stmt            *lambda;
} expr;

/*
 * Expression memory management
 */
expr *expr_alloc(void);
expr *expr_copy(expr *ex);
void expr_free(expr *ex);

/*
 * Expression stack
 */
void expr_stack_push(intend_state *s, expr *ex);
expr *expr_stack_pop(intend_state *s);
void expr_stack_teardown(intend_state *s);
void expr_arg_enter(intend_state *s);
void expr_arg_push(intend_state *s);
int expr_arg_leave(intend_state *s);
void expr_arg_teardown(intend_state *s);

/*
 * -- parsing machinery --
 */

/*
 * Statement parsing
 */
void stmt_begin_block(intend_state *s);
void stmt_end_block(intend_state *s);
void stmt_begin_if(intend_state *s);
void stmt_end_if(intend_state *s);
void stmt_begin_while(intend_state *s);
void stmt_end_while(intend_state *s);
void stmt_begin_do(intend_state *s);
void stmt_end_do(intend_state *s);
void stmt_begin_for(intend_state *s);
void stmt_end_for(intend_state *s);
void stmt_end_continue(intend_state *s);
void stmt_end_break(intend_state *s);
void stmt_end_return(intend_state *s);
void stmt_end_return_value(intend_state *s);
void stmt_end_nop(intend_state *s);
void stmt_end_expr(intend_state *s);
void stmt_begin_func(intend_state *s);
void stmt_end_rettype(intend_state *s, char type);
void stmt_end_func(intend_state *s, char *name);
void stmt_end_arg(intend_state *s, char typespec, char *name);
void stmt_begin_switch(intend_state *s);
void stmt_end_switch(intend_state *s);
void stmt_begin_case(intend_state *s);
void stmt_end_case(intend_state *s, char thru);
void stmt_end_default(intend_state *s);
void stmt_begin_try(intend_state *s);
void stmt_end_try(intend_state *s, char *name);
void stmt_end_throw(intend_state *s);
void stmt_begin_class(intend_state *s);
void stmt_end_extends(intend_state *s, char *parent);
void stmt_end_class(intend_state *s, char *name);
void expr_begin_lambda(intend_state *s);
void expr_end_lambda(intend_state *s);
void stmt_arg_teardown(intend_state *s);

/*
 * Expression parsing
 */
void expr_end_nop(intend_state *s);
void expr_end_assign(intend_state *s, char *name);
void expr_end_assign_array(intend_state *s, char *name);
void expr_end_op_assign(intend_state *s, char *name, op_type type);
void expr_end_op_assign_array(intend_state *s, char *name, op_type type);
void expr_end_cast(intend_state *s, char *typespec);
void expr_end_call_void(intend_state *s, char *name, int cons);
void expr_end_call(intend_state *s, char *name, int cons);
void expr_end_static_void(intend_state *s, char *tname, char *name);
void expr_end_static(intend_state *s, char *tname, char *name);
void expr_end_method_void(intend_state *s, char *name);
void expr_end_method(intend_state *s, char *name);
void expr_end_typeinit_void(intend_state *s, int type);
void expr_end_typeinit(intend_state *s, int type);
void expr_end_keyarray_single(intend_state *s);
void expr_end_ref(intend_state *s, char *name);
void expr_end_ref_array(intend_state *s, char *name);
void expr_end_index(intend_state *s);
void expr_end_const_bool(intend_state *s, char *val);
void expr_end_true(intend_state *s);
void expr_end_const_int(intend_state *s, char *val);
void expr_end_const_float(intend_state *s, char *val);
void expr_end_const_string(intend_state *s, char *val);
void expr_end_const_void(intend_state *s);
void expr_end_field(intend_state *s, char *val);
void expr_end_infix(intend_state *s, op_type type);
void expr_end_postfix(intend_state *s, op_type type);
void expr_end_prefix(intend_state *s, op_type type);
void expr_end_first(intend_state *s);
void expr_end_next(intend_state *s);
void expr_end_static_ref(intend_state *s, char *tname, char *name);
void expr_end_if(intend_state *s);
void expr_end_pass_ref(intend_state *s);
void expr_end_file(intend_state *s);
void expr_end_line(intend_state *s);

/*
 * Dumping
 */
void expr_dump(intend_state *s, expr *ex);
void stmt_dump_mode(intend_state *s, int mode);
void stmt_dump(intend_state *s, stmt *st);
void stmt_list_dump(intend_state *s, stmt_list *list, int internal);

/*
 * -- parser data --
 */
typedef struct parser_data {
    // scanner state
    void            *scanner;               /* flex scanner data */
    // general
    int             parser_error;           /* parser error count */
    // include file stack
    int             include_depth;
    char            **include_files;
    FILE            **include_fps;
    int             *include_lines;
    int             *include_cols;
    // side copy of all allocated filenames
    int include_all;
    char            **include_all_files;
    // block comment flag
    int             in_block;
    // statement stack
    unsigned int    stack_depth;            /* depth of statement list stack */
    stmt_list       **stack;                /* parsed script statement list stack */
    // expression stack
    int             expr_depth;             /* depth of expression stack */
    expr            **expr_stack;           /* expression stack */
    // argument count stack
    int             arg_depth;              /* depth of argument count stack */
    unsigned int    *arg_count;             /* argument count stack */
    // function argument definitions
    unsigned int    fn_arg_depth;
    unsigned int    *fn_arg_count;
    char            **fn_arg_types;
    char            ***fn_arg_names;
    char            *fn_rettype;
    // name of parent class
    char            *class_parent;
} parser_data;

/*
 * -- high-level interface --
 */

int parser_run(intend_state *s, FILE *file);

#endif
