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
 * Intend C Constant expressions
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"

/*
 * Evaluate void constant
 */
value *eval_const_void(void)
{
    return value_make_void();
}

/*
 * Evaluate bool constant
 */
value *eval_const_bool(expr *ex)
{
    sanity(ex);

    return value_make_bool(strcmp(ex->name, "true") == 0);
}

/*
 * Evaluate int constant
 */
value *eval_const_int(expr *ex)
{
    sanity(ex);

    return value_make_int(strtol(ex->name, NULL, 0));
}

/*
 * Evaluate float constant
 */
value *eval_const_float(expr *ex)
{
    sanity(ex);

    return value_make_float(strtod(ex->name, NULL));
}

/*
 * Character code escape
 */
static int charcode(char *orig, char **pos, int base, unsigned int maxlen)
{
    long int res, len;
    char tmp = 0;
    char *endptr = orig;

    if (strlen(orig) > maxlen) {
        tmp = orig[maxlen];
        orig[maxlen] = 0;
    }

    res = strtol(orig, &endptr, base);
    if (endptr != orig) {
        *(*pos) = res & 0xFF;
        (*pos) += 1;
        len = (endptr - orig) - 1;
    } else {
        len = 0;
    }

    if (tmp != 0) {
        orig[maxlen] = tmp;
    }
    return len;
}

/*
 * Unescape special characters
 */
static char *unescape(intend_state *s, const char *orig, int *retlen)
{
    char *raw, *pos;
    int i, len, is_esc = 0, count = 0;

    len = strlen(orig);

    raw = oom(malloc(len + 1));
    pos = raw;

    for (i = 0; i < len + 1; i++) {
        if (is_esc) {
            switch (orig[i]) {
                case 0:
                    *retlen = 0;
                    free(raw);
                    fatal(s, "non-terminated string escape sequence");
                    return NULL;
                    break;
                case '0':
                    i += charcode((char *) & orig[i], &pos, 8, 4);
                    break;
                case 'b':
                    *pos++ = '\b';
                    break;
                case 'd':
                    i += charcode((char *) & orig[i+1], &pos, 10, 3) + 1;
                    break;
                case 'e':
                    *pos++ = 27;
                    break;
                case 'f':
                    *pos++ = '\f';
                    break;
                case 'n':
                    *pos++ = '\n';
                    break;
                case 'o':
                    i += charcode((char *) & orig[i+1], &pos, 8, 3) + 1;
                    break;
                case 'r':
                    *pos++ = '\r';
                    break;
                case 't':
                    *pos++ = '\t';
                    break;
                case 'x':
                    i += charcode((char *) & orig[i+1], &pos, 16, 2) + 1;
                    break;
                default:
                    if (isdigit(orig[i])) {
                        i += charcode((char *) & orig[i], &pos, 8, 3);
                    } else {
                        *pos++ = orig[i];
                    }
            }
            is_esc = 0;
            count++;
        } else {
            switch (orig[i]) {
                case 0:
                    *pos = 0;
                    i = len + 1;
                    break;
                case '\\':
                    is_esc = 1;
                    break;
                default:
                    *pos++ = orig[i];
                    count++;
            }
        }
    }
    *retlen = count;
    return raw;
}

/*
 * Evaluate string constant
 */
value *eval_const_string(intend_state *s, expr *ex)
{
    char *raw;
    value *val;
    int len;

    sanity(ex && ex->name);

    raw = unescape(s, ex->name, &len);
    val = value_make_memstring(raw, len);
    free(raw);
    return val;
}
