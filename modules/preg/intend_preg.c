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
 * Intend C PREG Regular Expression module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

#include <intend.h>

#include "intend_preg.h"

/*
 * Table of supported PREG constants
 */
int preg_ANCHORED         = PCRE_ANCHORED;
int preg_CASELESS         = PCRE_CASELESS;
int preg_DOLLAR_ENDONLY   = PCRE_DOLLAR_ENDONLY;
int preg_DOTALL           = PCRE_DOTALL;
int preg_EXTENDED         = PCRE_EXTENDED;
int preg_MULTILINE        = PCRE_MULTILINE;
int preg_UNGREEDY         = PCRE_UNGREEDY;
int preg_NOTBOL           = PCRE_NOTBOL;
int preg_NOTEOL           = PCRE_NOTEOL;
int preg_NOTEMPTY         = PCRE_NOTEMPTY;
static intend_variable_data preg_vars[] = {
    { "PREG_ANCHORED",          INTEND_TYPE_INT,    &preg_ANCHORED          },
    { "PREG_CASELESS",          INTEND_TYPE_INT,    &preg_CASELESS          },
    { "PREG_DOLLAR_ENDONLY",    INTEND_TYPE_INT,    &preg_DOLLAR_ENDONLY    },
    { "PREG_DOTALL",            INTEND_TYPE_INT,    &preg_DOTALL            },
    { "PREG_EXTENDED",          INTEND_TYPE_INT,    &preg_EXTENDED          },
    { "PREG_MULTILINE",         INTEND_TYPE_INT,    &preg_MULTILINE         },
    { "PREG_UNGREEDY",          INTEND_TYPE_INT,    &preg_UNGREEDY          },
    { "PREG_NOTBOL",            INTEND_TYPE_INT,    &preg_NOTBOL            },
    { "PREG_NOTEOL",            INTEND_TYPE_INT,    &preg_NOTEOL            },
    { "PREG_NOTEMPTY",          INTEND_TYPE_INT,    &preg_NOTEMPTY          },

    /* list terminator */
    { NULL,                     0,                  NULL                    }
};

/*
 * Intend C PREG functions to register
 */
static intend_function_data preg_funcs[] = {
    { "is_preg_resource",   preg_is_resource,   1,  "r",    'b' },
    { "preg_compile",       preg_compile,       2,  "si",   '?' },
    { "preg_match",         preg_match,         2,  "rsi",  '?' },
    { "preg_exec",          preg_exec,          2,  "rsi",  '?' },
    { "preg_free",          preg_free,          1,  "r",    'v' },

    /* list terminator */
    { NULL,                 NULL,               0,  NULL,   0   }
};

/*
 * Initialize the PREG Regular Expression module
 */
void intend_preg_module_initialize(intend_ctx ctx)
{
    intend_register_variable_array(ctx, preg_vars);
    intend_register_function_array(ctx, preg_funcs);
}

/*
 * Destroy the PREG Regular Expression module
 */
void intend_preg_module_destroy(intend_ctx ctx)
{
    intend_unregister_variable_array(ctx, preg_vars);
    intend_unregister_function_array(ctx, preg_funcs);
}

/*
 * Free internal data of a compiled regular expression
 */
static void pregdata_free(void *data)
{
    pregdata *pd = data;

    if (pd) free(pd->data);
    if (pd) free(pd->table);
    free(pd);
}

/*
 * Initialize internal data for a compiled regular expression
 */
static pregdata *pregdata_init(pcre *data, const unsigned char *table)
{
    pregdata *res;

    res = intend_oom(malloc(sizeof(pregdata)));
    res->data = data;
    res->table = (unsigned char *) table;

    return res;
}

/*
 * Get compiled regexp pointer from resource
 */
static void *pregdata_get(void *data)
{
    pregdata *pd = data;

    if (!pd) {
    return NULL;
    }
    return pd->data;
}

/*
 * Check whether a value is a regular expression resource
 */
static int is_preg(intend_value val)
{
    return (intend_resource_release(val) == pregdata_free);
}

/*
 * Check whether resourse is PREG resource
 */
intend_value preg_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int res = is_mem(argv[0]);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Compile regular expression into PREG resource
 */
intend_value preg_compile(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *pat = intend_string_ptr(argv[0]);
    int opts  = intend_int_value(argv[1]);
    const unsigned char *table;
    pcre *comp;
    const char *err;
    int erroffset;
    intend_value res;

    table = pcre_maketables();
    if (!table) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    comp = pcre_compile(pat, opts, &err, &erroffset, table);
    if (!comp) {
        intend_nonfatal_error(ctx, "preg error: %s (offset %i)", err, erroffset);
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    res = intend_create_value(INTEND_TYPE_RES, pregdata_init(comp, table), pregdata_free, pregdata_get);
    return res;
}

/*
 * Run compiled regular expression on some string
 *
 * This function tries to match an input string with a previously
 * compiled regular expression. Depending on the matches parameter,
 * either a bool or an array of matching substrings is returned.
 */
static intend_value preg_run(intend_ctx ctx, unsigned int argc, intend_value *argv,
  int matches)
{
    pregdata *pd = intend_resource_value(argv[0]);
    int len      = intend_string_len(argv[1]);
    char *str    = intend_string_ptr(argv[1]);
    int opts     = intend_int_value(argv[2]);
    int i, res, subs = 0;
    int *ovector;
    char *start;
    int elemlen;
    intend_value info, *elem;

    if (!is_preg(argv[0]) || !pd) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    pcre_fullinfo(pd->data, NULL, PCRE_INFO_CAPTURECOUNT, &subs);
    ++subs;

    ovector = malloc(subs * 3 * sizeof(int));
    if (!ovector) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    res = pcre_exec(pd->data, NULL, str, len, 0, opts, ovector, subs * 3);
    if (res < -1) {
        free(ovector);
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    if (!matches) {
        int ret = (res > 0);
        free(ovector);
        return intend_create_value(INTEND_TYPE_BOOL, &ret);
    }

    info = intend_create_value(INTEND_TYPE_ARRAY, NULL);
    for (i = 0; i < res; ++i) {
        if (ovector[2 * i] == -1) {
            elem = intend_create_value(INTEND_TYPE_VOID, NULL);
        } else {
            start = str + ovector[2 * i];
            elemlen = ovector[2 * i + 1] - ovector[2 * i];
            elem = intend_create_value(INTEND_TYPE_STRING, start, &elemlen);
        }
        intend_array_add(info, elem);
        intend_free_value(elem);
    }

    free(ovector);
    return info;
}

/*
 * Try to match regular expression
 *
 * This function returns true if a precompiled regular expression
 * matches a given string. It returns false otherwise.
 */
intend_value preg_match(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return preg_run(ctx, argc, argv, 0);
}

/*
 * Return matching substrings
 *
 * This function returns an array of matching substring for the
 * given precompiled regular expression and input string. The
 * result is an empty array if the regexp does not match.
 */
intend_value preg_exec(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return preg_run(ctx, argc, argv, 1);
}

/* 
 * Free data of a precompiled regular expression
 */
intend_value preg_free(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    pregdata *pd = intend_resource_value(argv[0]);

    if (is_preg(argv[0]) && pd) {
        pregdata_free(pd);
        intend_resource_value_set(argv[0], NULL);
    }
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

