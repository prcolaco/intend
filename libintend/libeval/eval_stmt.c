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
 * Intend C Statements evaluation
 */


#include <stdlib.h>
#include <string.h>

#include "eval.h"

/*
 * Evaluate statement list
 *
 * This function evaluates the contents of a statement list. If
 * a continue statment or an error occurs, processing is halted.
 * On successful evaluation, 0 is returned. On error, -1 is
 * returned.
 */
void eval_stmt_list(intend_state *s, stmt_list *list, int cookie)
{
    unsigned int i;

    sanity(list);

    if (s->return_flag || s->exit_flag) return;

    for (i = 0; i < list->len; i++) {
        eval_stmt(s, list->list[i], cookie);
        if (s->continue_flag || s->return_flag ||
                s->except_flag   || s->exit_flag) break;
    }
}

/*
 * Evaluate boolean test expression
 *
 * This function tries to evaluate the test expression from an if,
 * while, do, or for statement. It returns 0 is the test returned
 * false, 1 if the test returned true, and -1 if an error occured.
 */
static int runtest(intend_state *s, expr *ex)
{
    value *val;
    int res;

    val = eval_expr(s, ex);
    value_cast_inplace(s, &val, VALUE_TYPE_BOOL);
    res = BOOL_OF(val);
    value_free(val);

    return res;
}

/*
 * Put function arguments into symbol table
 *
 * This function adds variables named "argc" and "argv" to the
 * current function's symbol table.
 */
static void varargs(intend_state *s, unsigned int argc, value **argv)
{
    value *count, *vector;
    unsigned int i;

    count = value_make_int(argc);
    vector = value_make_array();
    for (i = 0; i < argc; i++) {
        value_add_to_array(vector, argv[i]);
    }
    symtab_stack_add_variable(s, "argc", count);
    symtab_stack_add_variable(s, "argv", vector);
    value_free(count);
    value_free(vector);
}

/*
 * Run user-defined function
 *
 * This function runs a user-defined function by creating a new
 * symbol table, adding the function parameters as variables, and
 * then executing the function body. This function is implicitly
 * called from eval_call.c::eval_call(), via call_func() from
 * libruntime.
 */
static value *run_func(intend_state *s, void *data, void *def,
                       unsigned int args, unsigned int argc, value **argv)
{
    char **names = (char **) data;
    stmt *st = (stmt *) def;
    unsigned int i;
    int cookie;

    sanity(def && argc >= args && (argc == 0 || argv));

    if (++s->func_flag < 1) {
        fatal(s, "too deep function call nesting");
        return value_make_void();
    }

    varargs(s, argc, argv);

    for (i = 0; i < args; i++) {
        symtab_stack_add_variable(s, names[i], argv[i]);
    }

    s->retval = NULL;
    cookie = ++s->global_cookie;

    eval_stmt(s, st, cookie);

    s->return_flag = s->continue_flag = s->break_flag = 0;

    /*
     * Check whether retval was allocated on the level of this
     * function call -- if not, allocate new void value
     */
    if (s->retval_cookie != cookie || !s->retval) {
        s->retval = value_make_void();
    }

    --s->func_flag;
    return s->retval;
}

/*
 * Create user-defined function symtab entry
 *
 * This function creates a symbol table entry for a user defined
 * function and adds it to the current symbol table.
 */
void define_func(intend_state *s, stmt *st)
{
    signature *sig;
    char *proto_copy;
    char *name_copy;

    sanity(st && st->name);

    sig = call_sig_alloc();

    name_copy = oom(malloc(strlen(st->name) + 1));
    strcpy(name_copy, st->name);

    if (st->proto) {
        proto_copy = oom(malloc(strlen(st->proto) + 1));
        strcpy(proto_copy, st->proto);
    } else {
        proto_copy = NULL;
    }

    sig->type    = FUNCTION_TYPE_USERDEF;
    sig->args    = st->args;
    sig->name    = name_copy;
    sig->proto   = proto_copy;
    sig->rettype = st->rettype;
    sig->data    = st->names;
    sig->def     = st->true_case;
    sig->call_u.userdef_vector = run_func;

    symtab_stack_add_function(s, st->name, sig);
    call_sig_free(sig);
}

/*
 * Evaluate single statement
 *
 * This function evaluates a single statment. On success, 0 is returned.
 * If an error occurs during processing, -1 is returned.
 */
void eval_stmt(intend_state *s, stmt *st, int cookie)
{
    value *val;
    int res = 0;

    sanity(st);

    if (s->return_flag || s->except_flag || s->exit_flag) return;

    switch (st->type) {
        case STMT_SWITCH:
            eval_stmt_switch(s, st, cookie);
            break;
        case STMT_NOP:
            /* null statement */
            break;
        case STMT_BLOCK:
            /* block of multiple statements */
            eval_stmt_list(s, (stmt_list *) st->block, cookie);
            break;
        case STMT_IF:
            /* if without else block */
            res = runtest(s, st->expr);
            if (res == 1) {
                eval_stmt(s, st->true_case, cookie);
            }
            break;
        case STMT_IF_ELSE:
            /* if with else block */
            res = runtest(s, st->expr);
            if (res == 1) {
                eval_stmt(s, st->true_case, cookie);
            } else if (res == 0) {
                eval_stmt(s, st->false_case, cookie);
            }
            break;
        case STMT_WHILE:
            /* while loop */
            if (++s->loop_flag < 1) {
                fatal(s, "too deep loop nesting");
                return;
            }
            while ((res = runtest(s, st->expr)) == 1) {
                eval_stmt(s, st->true_case, cookie);
                s->continue_flag = 0;
                if (s->break_flag) break;
            }
            s->break_flag = 0;
            --s->loop_flag;
            break;
        case STMT_DO:
            /* do loop */
            if (++s->loop_flag < 1) {
                fatal(s, "too deep loop nesting");
                return;
            }
            do {
                eval_stmt(s, st->true_case, cookie);
                if (!s->break_flag) {
                    res = runtest(s, st->expr);
                }
                s->continue_flag = 0;
            } while (res == 1 && !s->break_flag);
            s->break_flag = 0;
            --s->loop_flag;
            break;
        case STMT_FOR:
            /* for loop */
            if (++s->loop_flag < 1) {
                fatal(s, "too deep loop nesting");
                return;
            }
            val = eval_expr(s, st->init);
            value_free(val);
            while ((res = runtest(s, st->expr)) == 1) {
                eval_stmt(s, st->true_case, cookie);
                s->continue_flag = 0;
                if (s->break_flag) break;
                val = eval_expr(s, st->guard);
                value_free(val);
            }
            s->break_flag = 0;
            --s->loop_flag;
            break;
        case STMT_CONTINUE:
            /* continue statement */
            if (s->loop_flag) {
                s->continue_flag = 1;
            }
            break;
        case STMT_BREAK:
            /* break statement */
            if (s->loop_flag) {
                s->continue_flag = s->break_flag = 1;
            }
            break;
        case STMT_RETURN:
            /* return statement */
            if (s->func_flag) {
                if (st->expr) {
                    val = eval_expr(s, st->expr);
                } else {
                    val = value_make_void();
                }
                /* store result, for function returns */
                s->retval = val;
                s->retval_cookie = cookie;
                s->return_flag = s->continue_flag = s->break_flag = 1;
            }
            break;
        case STMT_EXPR:
            /* expression statment */
            val = eval_expr(s, st->expr);
            value_free(val);
            break;
        case STMT_FUNC:
            define_func(s, st);
            break;
        case STMT_TRY:
            if(except_try(s)) {
                eval_stmt(s, st->true_case, cookie);
                if (val = except_catch(s)) {
                    symtab_stack_add_variable(s, st->name, val);
                    value_free(val);
                    eval_stmt(s, st->false_case, cookie);
                }
            }
            break;
        case STMT_THROW:
            except_throw(s, eval_expr(s, st->expr));
            break;
        case STMT_CLASS:
            symtab_stack_add_class(s, st->name, st->proto, st->block);
            break;
        case STMT_CASE:
        case STMT_DEFAULT:
            sanity(0);
            break;
    }
}
