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
 * Expression memory management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/*
 * Allocate expression structure
 */
expr *expr_alloc(void)
{
    expr *ex;

    ex = oom(calloc(sizeof(expr), 1));

    return ex;
}

/*
 * Free expression structure
 */
void expr_free(expr *ex)
{
    unsigned int i;

    if (!ex) {
        return;
    }

    expr_free(ex->inner);
    expr_free(ex->index);
    expr_free(ex->elif);
    for (i = 0; i < ex->argc; i++) {
        expr_free(ex->argv[i]);
    }

    stmt_free(ex->lambda);

    free(ex->argv);
    free(ex->name);
    free(ex->tname);
    free(ex);
}

/*
 * Copy expression
 */
expr *expr_copy(expr *ex)
{
    expr *copy;
    char *scopy;
    unsigned int i;

    if (!ex) {
        return NULL;
    }

    copy = expr_alloc();
    *copy = *ex;

    if (ex->name) {
        scopy = oom(malloc(strlen(ex->name) + 1));
        strcpy(scopy, ex->name);
        copy->name = scopy;
    }
    if (ex->tname) {
        scopy = oom(malloc(strlen(ex->tname) + 1));
        strcpy(scopy, ex->tname);
        copy->tname = scopy;
    }

    copy->inner = expr_copy(ex->inner);
    copy->index = expr_copy(ex->index);

    if (ex->argc > 0) {
        copy->argv = oom(malloc(sizeof(expr *) * ex->argc));
        for (i = 0; i < ex->argc; i++) {
            copy->argv[i] = expr_copy(ex->argv[i]);
        }
    }

    return copy;
}
