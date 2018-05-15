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
 * Intend C Expression stack handling
 */

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

/*
 * Push expression onto stack
 *
 * This function pushed an expression onto the stack. It returns 0
 * on success and -1 on failure.
 */
void expr_stack_push(intend_state *s, expr *ex)
{
    parser_data *p;
    expr **copy;

    sanity(s && ex);

    p = s->parser;

    copy = oom(realloc(p->expr_stack, (p->expr_depth + 1) * sizeof(expr *)));
    p->expr_stack = copy;
    p->expr_stack[p->expr_depth++] = ex;
}

/*
 * Pop expression from stack
 *
 * This function returns the last expression that was pushed onto the
 * stack. It returns NULL if no such expression exists.
 */
expr *expr_stack_pop(intend_state *s)
{
    parser_data *p;

    sanity(s);

    p = s->parser;

    sanity(p->expr_depth > 0);

    p->expr_depth--;
    return p->expr_stack[p->expr_depth];
}

/*
 * Tear down expression stack
 */
void expr_stack_teardown(intend_state *s)
{
    parser_data *p;

    sanity(s);

    if (!s->parser)
        return;

    p = s->parser;

    if (p->expr_stack) {
        free(p->expr_stack);
        p->expr_stack = NULL;
        p->expr_depth = 0;
    }
}

/*
 * Enter new argument count
 *
 * This function extends the argument count stack one level. It is
 * called whenever the parser enters the scope of a new function
 * call with arguments.
 */
void expr_arg_enter(intend_state *s)
{
    parser_data *p;
    unsigned int *count;

    sanity(s);

    p = s->parser;

    count = oom(realloc(p->arg_count, (p->arg_depth + 1) * sizeof(int *)));
    p->arg_count = count;
    p->arg_count[p->arg_depth++] = 1;
}

/*
 * Push argument count
 *
 * This function increases the argument count stored in the top level
 * of the argument count stack by one. It is called whenever the
 * parser encounters a new argument to a function call.
 */
void expr_arg_push(intend_state *s)
{
    parser_data *p;

    sanity(s);

    p = s->parser;

    sanity(p->arg_depth > 0);

    p->arg_count[p->arg_depth - 1]++;
}

/*
 * Leave argument count
 *
 * This function reduces the argument count stack by one level and
 * returns the count of arguments stored in the now-discarded level.
 * It is called by the parser when it is finished with parsing
 * function arguments and needs to know their number. This function
 * returns -1 if there was no argument count on the stack.
 */
int expr_arg_leave(intend_state *s)
{
    parser_data *p;

    sanity(s);

    p = s->parser;

    sanity(p->arg_depth > 0);

    p->arg_depth--;
    return p->arg_count[p->arg_depth];
}

/*
 * Tear down argument count stack
 */
void expr_arg_teardown(intend_state *s)
{
    parser_data *p;

    sanity(s);

    if (!s->parser)
        return;

    p = s->parser;

    if (p->arg_count) {
        free(p->arg_count);
        p->arg_count = NULL;
        p->arg_depth = 0;
    }
}
