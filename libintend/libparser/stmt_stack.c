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
 * Intend C Statement stack
 */

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

/*
 * Enter new statement list
 */
void stmt_stack_enter(intend_state *s)
{
    parser_data *p;
    stmt_list **copy;

    sanity(s);

    p = s->parser;

    copy = oom(realloc(p->stack, (p->stack_depth + 1) * sizeof(stmt_list *)));
    p->stack = copy;
    p->stack[p->stack_depth++] = stmt_list_alloc();
}

/*
 * Leave last statement list
 */
stmt_list *stmt_stack_leave(intend_state *s, int free)
{
    parser_data *p;
    stmt_list *list = NULL;

    sanity(s);

    p = s->parser;

    sanity(p->stack_depth > 0 || free);


    if (p->stack_depth == 0 && free) {
        return NULL;
    }

    p->stack_depth--;
    if (free) {
        stmt_list_free(p->stack[p->stack_depth]);
    } else {
        list = p->stack[p->stack_depth];
    }
    p->stack[p->stack_depth] = NULL;
    return list;
}

/*
 * Put statement on stack
 */
void stmt_stack_push(intend_state *s, stmt *st)
{
    parser_data *p;

    sanity(s && st);

    p = s->parser;

    sanity(p->stack_depth > 0);

    stmt_list_push(p->stack[p->stack_depth - 1], st);
}

/*
 * Get last statment from stack
 */
stmt *stmt_stack_pop(intend_state *s)
{
    parser_data *p;

    sanity(s);

    p = s->parser;

    if (p->stack_depth == 0) {
        return NULL;
    }
    return stmt_list_pop(p->stack[p->stack_depth - 1]);
}

/*
 * Tear down statement stack
 */
void stmt_stack_teardown(intend_state *s)
{
    parser_data *p;

    sanity(s);

    if (!s->parser)
        return;

    p = s->parser;

    if (p->stack) {
        free(p->stack);
        p->stack = NULL;
        p->stack_depth = 0;
    }
}
