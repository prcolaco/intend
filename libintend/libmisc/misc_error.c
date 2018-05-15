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
 * Intend C Error printing
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

/*
 * Write error message
 */
void errmsg(intend_state *s, int abort, const char *msg, va_list ap)
{
    char *source_file;

    if (!s || !s->source_file) {
        source_file = "intend";
    } else {
        source_file = s->source_file;
    }

    if (s && s->source_col > 0) {
        fprintf(stderr, "%s:%i:%i: ", source_file, s->source_line, s->source_col);
    } else if (s && s->source_line > 0) {
        fprintf(stderr, "%s:%i: ", source_file, s->source_line);
    } else {
        fprintf(stderr, "%s: ", source_file);
    }

    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");

    if (abort) {
        if (s) {
            s->exit_flag  = 1;
            s->exit_value = -1;
        } else {
            exit(1);
        }
    }
}

/*
 * Write fatal error message
 *
 * This function prints a fatal error message. Execution of the
 * running program is aborted.
 */
void fatal(intend_state *s, const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    errmsg(s, 1, msg, ap);
    va_end(ap);
}

/*
 * Write nonfatal error message
 *
 * This function prints an error message. Execution of the
 * running program continues as normal.
 */
void nonfatal(intend_state *s, const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    errmsg(s, 0, msg, ap);
    va_end(ap);
}

/*
 * Report an internal error
 *
 * This function prints an internal error message and is invoked
 * whenever an interpreter core function is passed impossible
 * values.
 */
void internal(const char *file, int line)
{
    fatal(NULL, "internal error at %s:%i", file, line);
}

/*
 * Report out-of-memory conditions
 *
 * This function prints a message about an out-of-memory condition
 * and aborts the running program if the given pointer is NULL.
 */
void *oom(void *ptr)
{
    if (!ptr) {
        fatal(NULL, "out of memory");
    }
    return ptr;
}
