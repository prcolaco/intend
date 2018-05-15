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
 * Intend C Cast evaluation
 */

#include <stdlib.h>
#include <string.h>

#include "eval.h"

/*
 * Map type name to type enum
 */
static value_type typeval(const char *typespec)
{
    sanity(typespec);

    if (strcmp(typespec, "void") == 0) {
        return VALUE_TYPE_VOID;
    } else if (strcmp(typespec, "bool") == 0) {
        return VALUE_TYPE_BOOL;
    } else if (strcmp(typespec, "int") == 0) {
        return VALUE_TYPE_INT;
    } else if (strcmp(typespec, "float") == 0) {
        return VALUE_TYPE_FLOAT;
    } else if (strcmp(typespec, "string") == 0) {
        return VALUE_TYPE_STRING;
    } else if (strcmp(typespec, "array") == 0) {
        return VALUE_TYPE_ARRAY;
    } else if (strcmp(typespec, "struct") == 0) {
        return VALUE_TYPE_STRUCT;
    }
    return VALUE_TYPE_VOID;
}

/*
 * Evaluate cast
 */
value *eval_cast(intend_state *s, expr *ex)
{
    value *val;

    sanity(ex);

    val = eval_expr(s, ex->inner);
    value_cast_inplace(s, &val, typeval(ex->name));
    return val;
}
