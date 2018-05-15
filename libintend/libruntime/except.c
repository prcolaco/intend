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
 * Intend C Exception control in runtime
 */

// To avoid warnings on asprintf function
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Throws an exception with value except
 */
void except_throw(intend_state *s, value *except)
{
    value *str;
    char *ex;

    s->except_value = except;
    if (!s->try_flag) {
        str = value_cast(s, except, VALUE_TYPE_STRING);
        asprintf(&ex, "uncaught exception: %s", STR_OF(str));
        value_free(str);
        fatal(s, ex);
        free(ex);
        value_free(s->except_value);
        s->except_value = NULL;
        return;
    }
    s->except_flag = 1;
}

/*
 * Starts a try section
 */
int except_try(intend_state *s)
{
    int res = 1;

    s->try_flag++;

    if (s->try_flag < 1) {
        fatal(s, "too deep try block nesting");
        res = 0;
    }

    return res;
}

/*
 * Catches any exception thrown in the mean time to a prior
 * except_try. This call ends the try section.
 * The return value must be freed with value_free
 */
value *except_catch(intend_state *s)
{
    value *res = NULL;

    s->try_flag--;

    if (s->except_flag) {
        res = s->except_value;
        s->except_value = NULL;
        s->except_flag = 0;
    }

    return res;
}
