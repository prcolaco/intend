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
 * Intend C Locale functions
 */

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#include "stdlib.h"

/*
 * Locale functions to register
 */
static register_func_data lc_funcs[] = {
    { "getlocale",	lc_getlocale,		0,	"",		's'	},
    { "setlocale",	lc_setlocale,		1,	"s",	'?'	},
    { "localeconv",	lc_localeconv,		0,	"",		'?'	},

    /* list terminator */
    { NULL,			NULL,				0,	NULL,	0	}
};

/*
 * Initialize the Locale functions
 */
void lc_init(intend_state *s)
{
    // Register functions
    register_function_array(s, lc_funcs);
}

/*
 * Get current locale
 */
value *lc_getlocale(intend_state *s, unsigned int argc, value **argv)
{
    return value_make_string(setlocale(LC_ALL, NULL));
}

/*
 * Set current locale
 */
value *lc_setlocale(intend_state *s, unsigned int argc, value **argv)
{
    char *name = STR_OF(argv[0]);
    char *res = NULL;

    if (!name) {
        res = setlocale(LC_ALL, "");
    } else {
        res = setlocale(LC_ALL, name);
    }
    if (!res) {
        return value_make_void();
    }
    return value_make_string(res);
}

/*
 * Add one-character string to struct
 */
static void addchr(value *s, const char *name, char what)
{
    value *elem;
    char buf[2];

    buf[0] = what;
    buf[1] = 0;

    elem = value_make_string(buf);
    value_set_struct(s, name, elem);
    value_free(elem);
}

/*
 * Add string to struct
 */
static void addstr(value *s, const char *name, char *what)
{
    value *elem;

    elem = value_make_string(what);
    value_set_struct(s, name, elem);
    value_free(elem);
}

/*
 * Get current locale conventions
 */
value *lc_localeconv(intend_state *s, unsigned int argc, value **argv)
{
    struct lconv *lc;
    value *ret;

    lc = localeconv();
    if (!lc) {
        return value_make_void();
    }

    ret = value_make_struct();

    addstr(ret, "decimal_point", lc->decimal_point);
    addstr(ret, "thousands_sep", lc->thousands_sep);
    addstr(ret, "grouping", lc->grouping);
    addstr(ret, "int_curr_symbol", lc->int_curr_symbol);
    addstr(ret, "currency_symbol", lc->currency_symbol);
    addstr(ret, "mon_decimal_point", lc->mon_decimal_point);
    addstr(ret, "mon_thousands_sep", lc->mon_thousands_sep);
    addstr(ret, "mon_grouping", lc->mon_grouping);
    addstr(ret, "positive_sign", lc->positive_sign);
    addstr(ret, "negative_sign", lc->negative_sign);
    addchr(ret, "int_frac_digits", lc->int_frac_digits);
    addchr(ret, "frac_digits", lc->frac_digits);
    addchr(ret, "p_cs_precedes", lc->p_cs_precedes);
    addchr(ret, "p_sep_by_space", lc->p_sep_by_space);
    addchr(ret, "n_cs_precedes", lc->n_cs_precedes);
    addchr(ret, "n_sep_by_space", lc->n_sep_by_space);
    addchr(ret, "p_sign_posn", lc->p_sign_posn);
    addchr(ret, "n_sign_posn", lc->n_sign_posn);

    return ret;
}
