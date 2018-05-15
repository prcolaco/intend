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
 * Intend C Expression parsing machinery
 */

#include <stdlib.h>
#include <string.h>

#include "parser.h"

/*
 * Construct argc/argv pair
 */
static void safe_args(intend_state *s, int *argc, expr ***argv)
{
    expr **next;
    int i;

    *argc = expr_arg_leave(s);
    sanity(*argc != -1);

    *argv = oom(calloc(sizeof(expr *), *argc));

    next = (*argv) + (*argc - 1);
    for (i = 0; i < *argc; i++) {
        *next-- = expr_stack_pop(s);
    }
}

/*
 * End simple expression
 */
static void expr_end_simple(intend_state *s, expr_type type)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = type;
    ex->file = s->source_file;
    ex->line = s->source_line;

    expr_stack_push(s, ex);
}

/*
 * End empty expression
 */
void expr_end_nop(intend_state *s)
{
    expr_end_simple(s, EXPR_NOP);
}

/*
 * End assignment expression
 */
void expr_end_assign(intend_state *s, char *name)
{
    expr *ex, *arg;

    arg = expr_stack_pop(s);

    ex = expr_alloc();
    ex->type = EXPR_ASSIGN;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;
    ex->inner = arg;

    expr_stack_push(s, ex);
}

/*
 * End combined operator and assignment expression
 */
void expr_end_op_assign(intend_state *s, char *name, op_type type)
{
    expr *mod, *ref, *ex;

    mod = expr_stack_pop(s);

    ref = expr_alloc();
    ref->type = EXPR_REF;
    ref->file = s->source_file;
    ref->line = s->source_line;
    ref->name = name;

    expr_stack_push(s, ref);
    expr_stack_push(s, mod);

    expr_end_infix(s, type);

    ex = expr_copy(ref);
    ex->type = EXPR_ASSIGN;
    ex->inner = expr_stack_pop(s);

    expr_stack_push(s, ex);
}

/*
 * End array assignent expression
 */
void expr_end_assign_array(intend_state *s, char *name)
{
    expr *ex, *val;
    expr **argv;
    int argc;

    val = expr_stack_pop(s);

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = EXPR_ASSIGN_ARRAY;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;
    ex->inner = val;
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End combined operator and assignment expression
 */
void expr_end_op_assign_array(intend_state *s, char *name, op_type type)
{
    expr *ex, *mod, *ref;
    expr **argv;
    int argc;

    mod = expr_stack_pop(s);

    safe_args(s, &argc, &argv);

    ref = expr_alloc();
    ref->type = EXPR_REF_ARRAY;
    ref->file = s->source_file;
    ref->line = s->source_line;
    ref->name = name;
    ref->argc = argc;
    ref->argv = argv;

    expr_stack_push(s, ref);
    expr_stack_push(s, mod);

    expr_end_infix(s, type);

    ex = expr_copy(ref);
    ex->type = EXPR_ASSIGN_ARRAY;
    ex->inner = expr_stack_pop(s);

    expr_stack_push(s, ex);
}

/*
 * End cast expression
 */
void expr_end_cast(intend_state *s, char *typespec)
{
    char *tcopy;
    expr *ex;

    tcopy = oom(malloc(strlen(typespec) + 1));
    strcpy(tcopy, typespec);

    ex = expr_alloc();
    ex->type = EXPR_CAST;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = expr_stack_pop(s);
    ex->name = tcopy;

    expr_stack_push(s, ex);
}

/*
 * End void function call expression
 */
void expr_end_call_void(intend_state *s, char *name, int cons)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = cons ? EXPR_NEW : EXPR_CALL;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;

    expr_stack_push(s, ex);
}

/*
 * End void static method call expression
 */
void expr_end_static_void(intend_state *s, char *tname, char *name)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_STATIC;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;
    ex->tname = tname;

    expr_stack_push(s, ex);
}

/*
 * End void method invocation
 */
void expr_end_method_void(intend_state *s, char *name)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_METHOD;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = expr_stack_pop(s);
    ex->name = name;

    expr_stack_push(s, ex);
}

/*
 * End function call expression
 */
void expr_end_call(intend_state *s, char *name, int cons)
{
    int argc;
    expr **argv;
    expr *ex;

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = cons ? EXPR_NEW : EXPR_CALL;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End type initialization expression without elements
 */
void expr_end_typeinit_void(intend_state *s, int type)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_CALL;
    ex->file = s->source_file;
    ex->line = s->source_line;
    switch (type) {
        case 1: // TYPE_ARRAY
            asprintf(&(ex->name), "mkarray");
            break;
        case 2: // TYPE_STRUCT
            asprintf(&(ex->name), "mkstruct");
            break;
        default: // This should never happen
            asprintf(&(ex->name), "invalid_type_initialization");
            break;
    }

    expr_stack_push(s, ex);
}

/*
 * End type initialization expression with elements
 */
void expr_end_typeinit(intend_state *s, int type)
{
    int argc;
    expr **argv;
    expr *ex;

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = EXPR_CALL;
    ex->file = s->source_file;
    ex->line = s->source_line;
    switch (type) {
        case 1: // TYPE_ARRAY
            asprintf(&(ex->name), "mkkeyarray");
            break;
        case 2: // TYPE_STRUCT
            asprintf(&(ex->name), "mkstruct");
            break;
        default: // This should never happen
            asprintf(&(ex->name), "invalid_type_initialization");
            break;
    }
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End key array initialization expression without key
 *
 * This function inserts a void expression before the last expression in the stack,
 * so that the function mkkeyarray receives the correct argument list.
 */
void expr_end_keyarray_single(intend_state *s)
{
    expr *ex;

    ex = expr_stack_pop(s);
    expr_end_simple(s, EXPR_CONST_VOID);
    expr_stack_push(s, ex);
}

/*
 * End function call expression
 */
void expr_end_use(intend_state *s)
{
    int argc;
    expr **argv;
    expr *ex;

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = EXPR_CALL;
    ex->file = s->source_file;
    ex->line = s->source_line;
    asprintf(&(ex->name), "module_load");
    oom(ex->name);
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End static method call expression
 */
void expr_end_static(intend_state *s, char *tname, char *name)
{
    int argc;
    expr **argv;
    expr *ex;

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = EXPR_STATIC;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;
    ex->tname = tname;
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End method invocation
 */
void expr_end_method(intend_state *s, char *name)
{
    int argc;
    expr **argv;
    expr *ex;

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = EXPR_METHOD;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = expr_stack_pop(s);
    ex->name = name;
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End variable reference expression
 */
void expr_end_ref(intend_state *s, char *name)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_REF;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;

    expr_stack_push(s, ex);
}

/*
 * End index reference expression
 */
void expr_end_index(intend_state *s)
{
    expr *ex;
    expr **argv;
    int argc;

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = EXPR_INDEX;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = expr_stack_pop(s);
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End array reference expression
 */
void expr_end_ref_array(intend_state *s, char *name)
{
    expr *ex;
    expr **argv;
    int argc;

    safe_args(s, &argc, &argv);

    ex = expr_alloc();
    ex->type = EXPR_REF_ARRAY;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = name;
    ex->argc = argc;
    ex->argv = argv;

    expr_stack_push(s, ex);
}

/*
 * End specified constant expression
 */
static void expr_end_const(intend_state *s, expr_type type, char *val)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = type;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->name = val;

    expr_stack_push(s, ex);
}

/*
 * End constant bool expression
 */
void expr_end_const_bool(intend_state *s, char *val)
{
    expr_end_const(s, EXPR_CONST_BOOL, val);
}

/*
 * End true bool expression
 */
void expr_end_true(intend_state *s)
{
    char *str;

    str = oom(malloc(5));
    strcpy(str, "true");

    expr_end_const(s, EXPR_CONST_BOOL, str);
}

/*
 * End constant int expression
 */
void expr_end_const_int(intend_state *s, char *val)
{
    expr_end_const(s, EXPR_CONST_INT, val);
}

/*
 * End constant float expression
 */
void expr_end_const_float(intend_state *s, char *val)
{
    expr_end_const(s, EXPR_CONST_FLOAT, val);
}

/*
 * End constant string expression
 */
void expr_end_const_string(intend_state *s, char *val)
{
    expr_end_const(s, EXPR_CONST_STRING, val);
}

/*
 * End constant void expression
 */
void expr_end_const_void(intend_state *s)
{
    expr_end_simple(s, EXPR_CONST_VOID);
}

/*
 * End field name
 */
void expr_end_field(intend_state *s, char *val)
{
    expr_end_const(s, EXPR_FIELD, val);
}

/*
 * End infix operator expression
 */
void expr_end_infix(intend_state *s, op_type type)
{
    expr *one, *two, *ex;

    two = expr_stack_pop(s);
    one = expr_stack_pop(s);

    ex = expr_alloc();
    ex->type = EXPR_INFIX;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = one;
    ex->index = two;
    ex->op = type;

    expr_stack_push(s, ex);
}

/*
 * End postfix operator expression
 */
void expr_end_postfix(intend_state *s, op_type type)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_POSTFIX;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = expr_stack_pop(s);
    ex->op = type;

    expr_stack_push(s, ex);
}

/*
 * End prefix operator expression
 */
void expr_end_prefix(intend_state *s, op_type type)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_PREFIX;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = expr_stack_pop(s);
    ex->op = type;

    expr_stack_push(s, ex);
}

/*
 * End first argument to function call
 */
void expr_end_first(intend_state *s)
{
    expr_arg_enter(s);
}

/*
 * End non-first argument to function call
 */
void expr_end_next(intend_state *s)
{
    expr_arg_push(s);
}

/*
 * End static class member reference
 */
void expr_end_static_ref(intend_state *s, char *tname, char *name)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_STATIC_REF;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->tname = tname;
    ex->name = name;

    expr_stack_push(s, ex);
}

/*
 * End conditional expression
 */
void expr_end_if(intend_state *s)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_IF;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->elif = expr_stack_pop(s);
    ex->index = expr_stack_pop(s);
    ex->inner = expr_stack_pop(s);

    expr_stack_push(s, ex);
}

/*
 * End reference passing expression
 */
void expr_end_pass_ref(intend_state *s)
{
    expr *ex;

    ex = expr_alloc();
    ex->type = EXPR_PASS_REF;
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->inner = expr_stack_pop(s);

    expr_stack_push(s, ex);
}

/*
 * End file name reference
 */
void expr_end_file(intend_state *s)
{
    expr_end_simple(s, EXPR_FILE);
}

/*
 * End source code line reference
 */
void expr_end_line(intend_state *s)
{
    expr_end_simple(s, EXPR_LINE);
}
