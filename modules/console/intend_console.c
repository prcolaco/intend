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
 * Intend C Console IO functions module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <intend.h>

#include "intend_console.h"

/*
 * Console IO functions to register
 */
static intend_function_data con_funcs[] = {
    { "getc",       con_getc,       0,  "",     's' },
    { "getchar",    con_getc,       0,  "",     's' },
    { "gets",       con_gets,       0,  "",     's' },
    { "input",      con_gets,       0,  "",     's' },
    { "puts",       con_print,      0,  "",     'v' },
    { "print",      con_print,      0,  "",     'v' },
    { "dump",       con_dump,       0,  "",     'v' },
    { "printf",     con_printf,     1,  "s",    'v' },

    /* list terminator */
    { NULL,     NULL,               0,  NULL,   0   }
};

/*
 * Initialize the Console IO functions module
 */
void intend_console_module_initialize(intend_ctx ctx)
{
    // Register functions
    intend_register_function_array(ctx, con_funcs);
}

/*
 * Destroy the Console IO functions module
 */
void intend_console_module_destroy(intend_ctx ctx)
{
    // Unregister functions
    intend_unregister_function_array(ctx, con_funcs);
}

/*
 * Dump parameter values
 *
 * This function prints out a dump of all its parameters.
 */
intend_value con_dump(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    unsigned int i;

    for (i = 0; i < argc; i++) {
        intend_value_dump(ctx, argv[i], 0, 0);
    }

    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Input a char
 *
 * This function reads a character from the standard input.
 */
intend_value con_getc(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    unsigned int i;
    intend_value val;
    FILE *in;
    char ch;
    char buf[2];

    in = intend_stream_get_stdin(ctx);
    ch = (char)fgetc(in);
    if (ch != EOF) {
        buf[0] = ch;
        buf[1] = 0;
        val = intend_create_value(INTEND_TYPE_STRING, &buf, NULL);
    } else {
        val = intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    return val;
}

/*
 * Input a string
 *
 * This function reads a line from the standard input.
 */
intend_value con_gets(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    unsigned int i;
    intend_value val;
    FILE *in;
    char *buf, *res;

    in = intend_stream_get_stdin(ctx);
    buf = intend_oom(malloc(65536));
    res = fgets(buf, 65536, in);
    if (res) {
        val = intend_create_value(INTEND_TYPE_STRING, buf, NULL);
    } else {
        val = intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    free(buf);

    return val;
}

/*
 * Print values as strings
 *
 * This function casts all its parameters to string and then
 * prints them out with no intervening newlines.
 */
intend_value con_print(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    unsigned int i;
    char *str;
    FILE *out;

    out = intend_stream_get_stdout(ctx);
    for (i = 0; i < argc; i++) {
        str = intend_string_value(argv[i]);
        if (strlen(str) > 0) {
            fprintf(out, "%s", str);
        }
        free(str);
    }

    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Print formatted string
 */
intend_value con_printf(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value val;
    char *str = NULL;
    FILE *out;

    out = intend_stream_get_stdout(ctx);
    val = intend_call_function_list(ctx, "sprintf", argc, argv);
    str = intend_string_ptr(val);
    if (str) fprintf(out, "%s", str);
    intend_free_value(val);

    return intend_create_value(INTEND_TYPE_VOID, NULL);
}
