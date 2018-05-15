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
 * Intend C Statement lists
 */

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

/*
 * Add statement to list
 */
void stmt_list_push(stmt_list *list, stmt *st)
{
    stmt **elems;

    sanity(list && st);

    if (list->len == 0) {
        elems = oom(malloc(sizeof(stmt *)));
        *elems = st;
        list->list = elems;
        list->len = 1;
    } else {
        elems = oom(realloc(list->list, (list->len + 1) * sizeof(stmt *)));
        list->list = elems;
        elems += list->len++;
        *elems = st;
    }
}

/*
 * Get last statement from list
 */
stmt *stmt_list_pop(stmt_list *list)
{
    sanity(list);

    if (list->len == 0) {
        return NULL;
    }
    list->len--;
    return list->list[list->len];
}
