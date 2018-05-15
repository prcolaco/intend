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
 * Intend C Evaluation of switch statement
 */

#include <stdlib.h>

#include "eval.h"

/*
 * Evaluate switch
 */
void eval_stmt_switch(intend_state *s, stmt *st, int cookie)
{
    value *val, *guard, *equal;
    stmt_list *list;
    stmt *label;
    unsigned int i, handled = 0, go = 0;
    int def = -1;

    sanity(st && st->type == STMT_SWITCH && st->block);
    list = (stmt_list *) st->block;

    val = eval_expr(s, st->expr);

    for (i = 0; i < list->len; i++) {
        label = list->list[i];
        sanity(label && (label->type == STMT_CASE || label->type == STMT_DEFAULT));

        if (label->type == STMT_DEFAULT) def = i;

        if (!go && label->type != STMT_CASE) continue;

        if (!go) {
            guard = eval_expr(s, label->expr);
            equal = eval_order_equal(val, guard);
            go = BOOL_OF(equal);
            value_free(guard);
            value_free(equal);
        }

        if (go) {
            handled = 1;
            eval_stmt_list(s, (stmt_list *) label->block, cookie);
        }

        if (go && !label->thru) break;
    }

    if (!handled && def >= 0) {
        eval_stmt_list(s, (stmt_list *) list->list[def]->block, cookie);
    }

    value_free(val);
}
