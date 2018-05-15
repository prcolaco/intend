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
 * Intend C Statement parsing machinery
 */

#include <stdlib.h>
#include <string.h>

#include "parser.h"

/*
 * Pop statement from stack or die
 */
static stmt *safe_pop(intend_state *s)
{
    stmt *st;

    st = stmt_stack_pop(s);
    sanity(st);

    return st;
}

/*
 * Start of statement block
 */
void stmt_begin_block(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End of statement block
 */
void stmt_end_block(intend_state *s)
{
    stmt *st;

    st = stmt_alloc();
    st->type = STMT_BLOCK;
    st->block = (STMT_LIST *) stmt_stack_leave(s, 0);

    stmt_stack_push(s, st);
}

/*
 * Start of if control statement
 */
void stmt_begin_if(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End of if control statement
 */
void stmt_end_if(intend_state *s)
{
    stmt *one, *two, *st;

    one = safe_pop(s);
    two = stmt_stack_pop(s);
    stmt_stack_leave(s, 1);

    st = stmt_alloc();
    if (two) {
        st->type = STMT_IF_ELSE;
        st->true_case = two;
        st->false_case = one;
    } else {
        st->type = STMT_IF;
        st->true_case = one;
    }
    st->expr = expr_stack_pop(s);

    stmt_stack_push(s, st);
}

/*
 * Start of while control statement
 */
void stmt_begin_while(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End of while control statement
 */
void stmt_end_while(intend_state *s)
{
    stmt *st;

    st = stmt_alloc();
    st->type = STMT_WHILE;
    st->expr = expr_stack_pop(s);
    st->true_case = safe_pop(s);

    stmt_stack_leave(s, 1);

    stmt_stack_push(s, st);
}

/*
 * Start of do control statement
 */
void stmt_begin_do(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End of do control statement
 */
void stmt_end_do(intend_state *s)
{
    stmt *st;

    st = stmt_alloc();
    st->type = STMT_DO;
    st->expr = expr_stack_pop(s);
    st->true_case = safe_pop(s);

    stmt_stack_leave(s, 1);

    stmt_stack_push(s, st);
}

/*
 * Start of for control statement
 */
void stmt_begin_for(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End of for control statement
 */
void stmt_end_for(intend_state *s)
{
    expr *init, *expr, *guard;
    stmt *st;

    guard = expr_stack_pop(s);
    expr = expr_stack_pop(s);
    init = expr_stack_pop(s);

    st = stmt_alloc();
    st->type = STMT_FOR;
    st->init = init;
    st->expr = expr;
    st->guard = guard;
    st->true_case = safe_pop(s);

    stmt_stack_leave(s, 1);

    stmt_stack_push(s, st);
}

/*
 * End simple statement
 */
static void end_simple(intend_state *s, stmt_type type)
{
    stmt *st;

    st = stmt_alloc();
    st->type = type;

    stmt_stack_push(s, st);
}

/*
 * End simple statement with expression parameter
 */
static void end_simple_expr(intend_state *s, stmt_type type)
{
    stmt *st;

    st = stmt_alloc();
    st->type = type;
    st->expr = expr_stack_pop(s);

    stmt_stack_push(s, st);
}

/*
 * End continue statement
 */
void stmt_end_continue(intend_state *s)
{
    end_simple(s, STMT_CONTINUE);
}

/*
 * End break statement
 */
void stmt_end_break(intend_state *s)
{
    end_simple(s, STMT_BREAK);
}

/*
 * End return statement
 */
void stmt_end_return(intend_state *s)
{
    end_simple(s, STMT_RETURN);
}

/*
 * End return statement with expression
 */
void stmt_end_return_value(intend_state *s)
{
    end_simple_expr(s, STMT_RETURN);
}

/*
 * End empty statement
 */
void stmt_end_nop(intend_state *s)
{
    end_simple(s, STMT_NOP);
}

/*
 * End expression statement
 */
void stmt_end_expr(intend_state *s)
{
    end_simple_expr(s, STMT_EXPR);
}

/*
 * Add argument definition to list
 */
void stmt_end_arg(intend_state *s, char typespec, char *name)
{
    parser_data *p;
    char *list;
    char **names;
    unsigned int count;

    sanity(s && name);

    p = (parser_data *)s->parser;
    count = p->fn_arg_count[p->fn_arg_depth - 1];

    list = p->fn_arg_types[p->fn_arg_depth - 1];
    list = oom(realloc(list, (count + 2) * sizeof(char)));

    names = p->fn_arg_names[p->fn_arg_depth - 1];
    names = oom(realloc(names, (count + 1) * sizeof(char *)));

    names[count++] = name;
    list[count-1] = typespec;
    list[count] = 0;

    p->fn_arg_count[p->fn_arg_depth - 1] = count;
    p->fn_arg_types[p->fn_arg_depth - 1] = list;
    p->fn_arg_names[p->fn_arg_depth - 1] = names;
}

/*
 * Start function declaration
 */
void stmt_begin_func(intend_state *s)
{
    parser_data *p;

    sanity(s);

    p = (parser_data *)s->parser;

    ++p->fn_arg_depth;

    p->fn_arg_types = oom(realloc(p->fn_arg_types, p->fn_arg_depth * sizeof(char *)));
    p->fn_arg_names = oom(realloc(p->fn_arg_names, p->fn_arg_depth * sizeof(char **)));
    p->fn_arg_count = oom(realloc(p->fn_arg_count, p->fn_arg_depth * sizeof(int)));
    p->fn_rettype   = oom(realloc(p->fn_rettype, p->fn_arg_depth * sizeof(char)));

    p->fn_arg_types[p->fn_arg_depth - 1] = NULL;
    p->fn_arg_names[p->fn_arg_depth - 1] = NULL;
    p->fn_arg_count[p->fn_arg_depth - 1] = 0;
    p->fn_rettype[p->fn_arg_depth - 1]   = '?';

    stmt_stack_enter(s);
}

/*
 * End function return type
 */
void stmt_end_rettype(intend_state *s, char type)
{
    parser_data *p;

    sanity(s);

    p = (parser_data *)s->parser;

    p->fn_rettype[p->fn_arg_depth - 1] = type;
}

/*
 * End function declaration
 */
void stmt_end_func(intend_state *s, char *name)
{
    parser_data *p;
    stmt *st;

    sanity(s);

    p = (parser_data *)s->parser;

    sanity(p->fn_arg_depth > 0);

    --p->fn_arg_depth;

    st = stmt_alloc();
    st->type = STMT_FUNC;
    st->true_case = safe_pop(s);
    st->name = name;
    st->args = p->fn_arg_count[p->fn_arg_depth];
    st->proto = p->fn_arg_types[p->fn_arg_depth];
    st->rettype = p->fn_rettype[p->fn_arg_depth];
    st->names = p->fn_arg_names[p->fn_arg_depth];

    stmt_stack_leave(s, 1);

    stmt_stack_push(s, st);
}

/*
 * Begin switch statement
 */
void stmt_begin_switch(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End switch statement
 */
void stmt_end_switch(intend_state *s)
{
    stmt *st;

    st = stmt_alloc();
    st->type = STMT_SWITCH;
    st->expr = expr_stack_pop(s);
    st->block = (STMT_LIST *) stmt_stack_leave(s, 0);

    stmt_stack_push(s, st);
}

/*
 * Begin case block
 */
void stmt_begin_case(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End case block
 */
void stmt_end_case(intend_state *s, char thru)
{
    stmt *st;
    stmt_list *list;

    list = stmt_stack_leave(s, 0);
    if (list->len == 1 && list->list[0]->type == STMT_BREAK) {
        list->list[0]->type = STMT_NOP;
        thru = 0;
    }

    st = stmt_alloc();
    st->type = STMT_CASE;
    st->expr = expr_stack_pop(s);
    st->block = (STMT_LIST *) list;
    st->thru = thru;

    stmt_stack_push(s, st);
}

/*
 * End default block
 */
void stmt_end_default(intend_state *s)
{
    stmt *st;

    st = stmt_alloc();
    st->type = STMT_DEFAULT;
    st->block = (STMT_LIST *) stmt_stack_leave(s, 0);
    st->thru = 0;

    stmt_stack_push(s, st);
}

/*
 * Begin try-catch block
 */
void stmt_begin_try(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End try-catch block
 */
void stmt_end_try(intend_state *s, char *name)
{
    stmt *st;

    st = stmt_alloc();
    st->type = STMT_TRY;
    st->false_case = safe_pop(s);
    st->true_case = safe_pop(s);
    st->name = name;

    stmt_stack_leave(s, 1);

    stmt_stack_push(s, st);
}

/*
 * End throw statement
 */
void stmt_end_throw(intend_state *s)
{
    end_simple_expr(s, STMT_THROW);
}

/*
 * Begin class definition
 */
void stmt_begin_class(intend_state *s)
{
    stmt_stack_enter(s);
}

/*
 * End extends declaration
 */
void stmt_end_extends(intend_state *s, char *parent)
{
    parser_data *p;

    sanity(s);

    p = (parser_data *)s->parser;

    p->class_parent = parent;
}

/*
 * End class definition
 */
void stmt_end_class(intend_state *s, char *name)
{
    parser_data *p;
    stmt *st;

    sanity(s);

    p = (parser_data *)s->parser;

    st = stmt_alloc();
    st->type = STMT_CLASS;
    st->name = name;
    st->proto = p->class_parent;
    st->block = (STMT_LIST *) stmt_stack_leave(s, 0);

    stmt_stack_push(s, st);
}

/*
 * Begin lambda expression
 */
void expr_begin_lambda(intend_state *s)
{
    stmt_begin_func(s);
}

/*
 * End lambda expression
 */
void expr_end_lambda(intend_state *s)
{
    parser_data *p;
    char *name;
    stmt *block, *st;
    expr *ex;

    sanity(s);

    p = (parser_data *)s->parser;

    sanity(p->fn_arg_depth > 0);

    --p->fn_arg_depth;

    name = oom(malloc(strlen("\\lambda") + 1));
    strcpy(name, "\\lambda");

    block = stmt_alloc();
    block->type = STMT_BLOCK;
    block->block = (STMT_LIST *) stmt_stack_leave(s, 0);

    st = stmt_alloc();
    st->type = STMT_FUNC;
    st->true_case = block;
    st->name = name;
    st->args = p->fn_arg_count[p->fn_arg_depth];
    st->proto = p->fn_arg_types[p->fn_arg_depth];
    st->rettype = '?';
    st->names = p->fn_arg_names[p->fn_arg_depth];

    ex = expr_alloc();
    ex->file = s->source_file;
    ex->line = s->source_line;
    ex->type = EXPR_LAMBDA;
    ex->lambda = st;

    expr_stack_push(s, ex);
}

/*
 * Tear down memory used by function argument types
 */
void stmt_arg_teardown(intend_state *s)
{
    parser_data *p;

    sanity(s);

    if (!s->parser)
        return;

    p = (parser_data *)s->parser;

    free(p->fn_arg_count);
    free(p->fn_arg_types);
    free(p->fn_arg_names);
    free(p->fn_rettype);
}
