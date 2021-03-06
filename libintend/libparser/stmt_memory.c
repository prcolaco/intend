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
 * Intend C Statement memory management
 */

#include <stdlib.h>
#include <string.h>

#include "parser.h"

/*
 * Allocate statement structure
 */
stmt *stmt_alloc(void)
{
    return oom(calloc(sizeof(stmt), 1));
}

/*
 * Free statment structure
 */
void stmt_free(stmt *st)
{
    if (!st) {
        return;
    }

    if (st->init) expr_free(st->init);
    if (st->expr) expr_free(st->expr);
    if (st->guard) expr_free(st->guard);
    if (st->true_case) stmt_free(st->true_case);
    if (st->false_case) stmt_free(st->false_case);
    if (st->block) stmt_list_free((stmt_list *) st->block);

    if (st->args > 0) {
        unsigned int i;

        for (i = 0; i < st->args; i++) {
            free(st->names[i]);
        }
    }

    free(st->name);
    free(st->proto);
    free(st->names);

    free(st);
}

/*
 * Allocate statement list structure
 */
stmt_list *stmt_list_alloc(void)
{
    return oom(calloc(sizeof(stmt_list), 1));
}

/*
 * Free statement list structure
 */
void stmt_list_free(stmt_list *list)
{
    unsigned int i;

    if (!list) {
        return;
    }

    for (i = 0; i < list->len; i++) {
        stmt_free(list->list[i]);
    }
    free(list->list);
    free(list);
}
