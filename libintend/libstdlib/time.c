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
 * Intend C Date and time functions
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stdlib.h"

/*
 * Date and time functions to register
 */
static register_func_data time_funcs[] = {
    { "time",		time_time,			0,	"",		'i'	},
    { "gmtime",		time_gmtime,		1,	"i",	'c'	},
    { "localtime",	time_localtime,		1,	"i",	'c'	},
    { "mktime",		time_mktime,		1,	"c",	'i'	},
    { "asctime",	time_asctime,		1,	"c",	's'	},
    { "ctime",		time_ctime,			1,	"i",	's'	},
    { "strftime",	time_strftime,		2,	"sc",	's'	},

    /* list terminator */
    { NULL,			NULL,				0,	NULL,	0	}
};

/*
 * Initialize the Date and time functions
 */
void time_init(intend_state *s)
{
    // Register functions
    register_function_array(s, time_funcs);
}

/*
 * Get current time
 *
 * Returns the time in seconds since 1970-01-01 00:00:00.
 */
value *time_time(intend_state *s, unsigned int argc, value **argv)
{
    return value_make_int(time(NULL));
}

/*
 * Create date and time structure
 */
static value *mktimestruct(struct tm *when)
{
    value *st, *val;

    st = value_make_struct();

    val = value_make_int(when->tm_sec);
    value_set_struct(st, "tm_sec", val); value_free(val);

    val = value_make_int(when->tm_min);
    value_set_struct(st, "tm_min", val); value_free(val);

    val = value_make_int(when->tm_hour);
    value_set_struct(st, "tm_hour", val); value_free(val);

    val = value_make_int(when->tm_mday);
    value_set_struct(st, "tm_mday", val); value_free(val);

    val = value_make_int(when->tm_mon);
    value_set_struct(st, "tm_mon", val); value_free(val);

    val = value_make_int(when->tm_year);
    value_set_struct(st, "tm_year", val); value_free(val);

    val = value_make_int(when->tm_wday);
    value_set_struct(st, "tm_wday", val); value_free(val);

    val = value_make_int(when->tm_yday);
    value_set_struct(st, "tm_yday", val); value_free(val);

    val = value_make_int(when->tm_isdst);
    value_set_struct(st, "tm_isdst", val); value_free(val);

    return st;
}

/*
 * Make UTC time structure
 *
 * Takes an time in seconds since the epoch and creates a
 * structure containing broken-out time values. It assumes UTC
 * as the local timezone.
 */
value *time_gmtime(intend_state *s, unsigned int argc, value **argv)
{
    return mktimestruct(gmtime((time_t *) &INT_OF(argv[0])));
}

/*
 * Make local time structure
 *
 * Takes an time in seconds since the epoch and creates a
 * structure containing broken-out time values. It assumes
 * the default local timezone of the system.
 */
value *time_localtime(intend_state *s, unsigned int argc, value **argv)
{
    return mktimestruct(localtime((time_t *) &INT_OF(argv[0])));
}

/*
 * Helper function to get an int from a struct
 */
static int getint(symtab *sym, const char *name)
{
    symtab_entry *entry;

    entry = symtab_lookup(sym, name);
    if (!entry || entry->type != SYMTAB_ENTRY_VAR ||
            TYPE_OF(&entry->entry_u.var) != VALUE_TYPE_INT) {
        return 0;
    } else {
        return INT_OF(&entry->entry_u.var);
    }
}

/*
 * Make C struct tm from struct value
 */
static struct tm mkstructtm(value *st) {
    symtab *sym = st->value_u.struct_val;
    struct tm t;

    memset(&t, 0, sizeof(struct tm));

    t.tm_sec   = getint(sym, "tm_sec");
    t.tm_min   = getint(sym, "tm_min");
    t.tm_hour  = getint(sym, "tm_hour");
    t.tm_mday  = getint(sym, "tm_mday");
    t.tm_mon   = getint(sym, "tm_mon");
    t.tm_year  = getint(sym, "tm_year");
    t.tm_wday  = getint(sym, "tm_wday");
    t.tm_yday  = getint(sym, "tm_yday");
    t.tm_isdst = getint(sym, "tm_isdst");

    return t;
}

/*
 * Convert time structure to time int
 */
value *time_mktime(intend_state *s, unsigned int argc, value **argv)
{
    struct tm t = mkstructtm(argv[0]);

    return value_make_int(mktime(&t));
}

/*
 * String representation of time structure
 */
value *time_asctime(intend_state *s, unsigned int argc, value **argv)
{
    struct tm t = mkstructtm(argv[0]);

    return value_make_string(asctime(&t));
}

/*
 * String representation of time value
 */
value *time_ctime(intend_state *s, unsigned int argc, value **argv)
{
    return value_make_string(ctime((time_t *) &INT_OF(argv[0])));
}

/*
 * Format time
 *
 * This function returns a string containing a formatted time
 * value.
 */
value *time_strftime(intend_state *s, unsigned int argc, value **argv)
{
    char *fmt   = STR_OF(argv[0]);
    struct tm t = mkstructtm(argv[1]);
    char buf[1024];

    if (!fmt) {
        return value_make_string(NULL);
    }

    buf[1023] = 0;
    strftime(buf, 1023, fmt, &t);

    return value_make_string(buf);
}
