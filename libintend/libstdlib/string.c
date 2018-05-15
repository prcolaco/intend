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
 * Intend C String functions
 */

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"

/*
 * String functions to register
 */
static register_func_data str_funcs[] = {
    { "sprintf",    str_sprintf,    1,  "s",    's' },
    { "strlen",		str_length,		1,	"S",	'i'	},
    { "strcat",		str_concat,		0,	"",		's'	},
    { "strchr",		str_leftmost,	2,	"SS",	'?'	},
    { "strrchr",	str_rightmost,	2,	"SS",	'?'	},
    { "strstr",		str_strpos,		2,	"SS",	'?'	},
    { "strspn",		str_span,		2,	"SS",	'i'	},
    { "strcspn",	str_cspan,		2,	"SS",	'i'	},
    { "strpbrk",	str_pbrk,		2,	"SS",	'?'	},
    { "strcoll",	str_coll,		2,	"SS",	'i'	},
    { "tolower",	str_lower,		1,	"S",	's'	},
    { "toupper",	str_upper,		1,	"S",	's'	},
    { "isalnum",	str_is_alnum,	1,	"S",	'b'	},
    { "isalpha",	str_is_alpha,	1,	"S",	'b'	},
    { "iscntrl",	str_is_cntrl,	1,	"S",	'b'	},
    { "isdigit",	str_is_digit,	1,	"S",	'b'	},
    { "isgraph",	str_is_graph,	1,	"S",	'b'	},
    { "islower",	str_is_lower,	1,	"S",	'b'	},
    { "isprint",	str_is_print,	1,	"S",	'b'	},
    { "ispunct",	str_is_punct,	1,	"S",	'b'	},
    { "isspace",	str_is_space,	1,	"S",	'b'	},
    { "isupper",	str_is_upper,	1,	"S",	'b'	},
    { "isxdigit",	str_is_xdigit,	1,	"S",	'b'	},
    { "substr",		str_mid,		2,	"Si*",	's'	},
    { "left",		str_left,		2,	"Si",	's'	},
    { "right",		str_right,		2,	"Si",	's'	},
    { "ord",		str_ord,		1,	"S",	'?'	},
    { "chr",		str_chr,		1,	"i",	's'	},
    { "explode",	str_explode,	1,	"S",	'a'	},
    { "implode",	str_implode,	1,	"a",	's'	},
    { "ltrim",		str_ltrim,		1,	"S",	's'	},
    { "rtrim",		str_rtrim,		1,	"S",	's'	},
    { "trim",		str_trim,		1,	"S",	's'	},

    /* list terminator */
    { NULL,			NULL,			0,	NULL,	0	}
};

/*
 * Initialize the String functions
 */
void str_init(intend_state *s)
{
    // Register functions
    register_function_array(s, str_funcs);
}

/*
 * Turn conversion specifier into type enum
 */
static int spectype(char spec)
{
    if (spec == 'f') {
        return VALUE_TYPE_FLOAT;
    } else if (spec == 's') {
        return VALUE_TYPE_STRING;
    } else {
        return VALUE_TYPE_INT;
    }
}

/*
 * Parse format string
 *
 * This function parses one format specification from a printf format
 * string. It skips over unknown character inside the spec, returns the
 * parsed format spec, and stores the expected type of the argument in
 * the given int pointer.
 */
static char *parse_fmt(char **i, int *min, int *valtype, char *buf)
{
    char *stop, *pos;
    int len;

    *min     = 0;
    *valtype = 0;
    buf[0]   = '%';
    buf[1]   = 0;
    pos      = buf + 1;

    /* escaped percent sign */
    if (**i == '%') {
        *pos++ = '%';
        *pos   = 0;
        (*i)++;
        return buf;
    }

    /* conversion flags */
    len = strspn(*i, "-+#0 ");
    if (len > 0) {
        strncat(buf, *i, len);
        *i  += len;
        pos += len;
    }

    /* display width */
    len = strspn(*i, "0123456789");
    if (len > 0) {
        strncat(buf, *i, len);
        *min = atoi(*i);
        *i  += len;
        pos += len;
    }

    /* display precision */
    if (**i == '.') {
        len = strspn(*i + 1, "0123456789");
        strncat(buf, *i, len + 1);
        *i  += len + 1;
        pos += len + 1;
    }

    /* conversion specifier */
    stop = strpbrk(*i, "fsdioxX");
    if (stop) {
        *valtype = spectype(*stop);
        *pos++   = *stop;
        *pos     = 0;
        *i       = stop + 1;
    } else {
        *i += strlen(*i);
    }

    return buf;
}

/*
 * Return formatted string
 */
value *str_sprintf(intend_state *s, unsigned int argc, value **argv)
{
    char *fmt = STR_OF(argv[0]);
    char *cfmt, *fmtbuf;
    char *buf = NULL;
    int min = 0;
    int len = 0, valtype, vallen = 0, span = 0;
    unsigned int arg = 1;
    value *val, *curr, *voidval;

    if (!fmt) {
        return value_make_string(NULL);
    }
    voidval = value_make_void();

    fmtbuf = oom(malloc(512));

    while (*fmt) {
        span = strcspn(fmt, "%");
        if (span > 0) {
            buf = oom(realloc(buf, len + span + 1));
            if (len == 0) {
                buf[0] = 0;
            }
            strncat(buf, fmt, span);
            len += span;
            fmt += span;
            buf[len] = 0;
        } else {
            ++fmt;
            cfmt = parse_fmt(&fmt, &min, &valtype, fmtbuf);

            if (valtype > 0) {
                if (arg < argc) {
                    curr = argv[arg];
                } else {
                    curr = voidval;
                }
                value_cast_inplace(s, &curr, valtype);
                if (arg < argc) {
                    argv[arg] = curr;
                } else {
                    voidval = curr;
                }
            }

            switch (valtype) {
                case 0:
                    vallen = strlen(cfmt);
                    if (min > vallen) vallen = min;
                    buf = oom(realloc(buf, len + vallen + 1));
                    vallen = sprintf(buf + len, cfmt);
                    break;
                case VALUE_TYPE_INT:
                    vallen = (min > 32) ? min : 32;
                    buf = oom(realloc(buf, len + vallen + 1));
                    vallen = sprintf(buf + len, cfmt, INT_OF(curr));
                    ++arg;
                    break;
                case VALUE_TYPE_FLOAT:
                    vallen = (min > 64) ? min : 64;
                    buf = oom(realloc(buf, len + vallen + 1));
                    vallen = sprintf(buf + len, cfmt, FLOAT_OF(curr));
                    ++arg;
                    break;
                case VALUE_TYPE_STRING:
                    if (STR_OF(curr))
                        vallen = strlen(STR_OF(curr));
                    else
                        vallen = 0;
                    if (min > vallen) vallen = min;
                    buf = oom(realloc(buf, len + vallen + 1));
                    if (vallen > 0) {
                        vallen = sprintf(buf + len, cfmt, STR_OF(curr));
                    } else {
                        vallen = sprintf(buf + len, cfmt, "");
                    }
                    ++arg;
                    break;
            }
            len += vallen;
        }
    }

    free(fmtbuf);

    buf[len] = 0;

    val = value_make_string(buf);

    free(buf);
    value_free(voidval);

    return val;
}

/*
 * Length of a string
 *
 * This function returns the length of a string as an int.
 */
value *str_length(intend_state *s, unsigned int argc, value **argv)
{
    char *str = STR_OF(argv[0]);
    int len = 0;

    if (str)
        len = strlen(str);

    return value_make_int(len);
}

/*
 * Concatenate strings
 */
value *str_concat(intend_state *s, unsigned int argc, value **argv)
{
    value *result;
    char *buf, *next;
    unsigned int i;
    int catlen = 0, len;

    for (i = 0; i < argc; i++) {
        value_cast_inplace(s, &argv[i], VALUE_TYPE_STRING);
        catlen += STRLEN_OF(argv[i]);
    }

    if (catlen == 0) {
        return value_make_string(NULL);
    }

    buf = oom(malloc(catlen + 1));
    buf[catlen] = 0;

    next = buf;
    for (i = 0; i < argc; i++) {
        len = STRLEN_OF(argv[i]);
        if (len > 0) {
            memcpy(next, STR_OF(argv[i]), len);
            next += len;
        }
    }

    result = value_make_memstring(buf, catlen);
    free(buf);
    return result;
}

/*
 * Position of first occurence of character in string
 *
 * This function uses the first character of the second argument
 * as the character to search for. It returns void if the character
 * is not found.
 */
value *str_leftmost(intend_state *s, unsigned int argc, value **argv)
{
    char *str = STR_OF(argv[0]);
    char search;
    char *pos;

    if (STRLEN_OF(argv[0]) == 0 ||
            STRLEN_OF(argv[1]) == 0) {
        return value_make_void();
    }
    search = STR_OF(argv[1])[0];

    pos = memchr(str, search, STRLEN_OF(argv[0]));
    if (!pos) {
        return value_make_void();
    }
    return value_make_int((ptrdiff_t) pos - (ptrdiff_t) str);
}

/*
 * Position of last occurence of character in string
 *
 * This function uses the first character of the second argument
 * as the character to search for. It returns void if the character
 * cannot be found.
 */
value *str_rightmost(intend_state *s, unsigned int argc, value **argv)
{
    char search;
    char *pos, *next;

    if (STRLEN_OF(argv[0]) == 0 ||
            STRLEN_OF(argv[1]) == 0) {
        return value_make_void();
    }
    search = STR_OF(argv[1])[0];

    pos = STR_OF(argv[0]);
    next = pos + STRLEN_OF(argv[0]) - 1;
    while (next >= pos) {
        if (*next == search) {
            return value_make_int((ptrdiff_t) next - (ptrdiff_t) pos);
        }
        --next;
    }

    return value_make_void();
}

/*
 * Return position of substring
 *
 * This function returns the position of the second argument inside
 * the first argument. It returns void if the substring is not found
 * and 0 if the substring is an empty string.
 */
value *str_strpos(intend_state *s, unsigned int argc, value **argv)
{
    char *str1 = STR_OF(argv[0]);
    int len1   = STRLEN_OF(argv[0]);
    char *str2 = STR_OF(argv[1]);
    int len2   = STRLEN_OF(argv[1]);
    char *pos, *end;

    if (!str1 || len2 > len1) {
        return value_make_void();
    }
    if (!str2) {
        return value_make_int(0);
    }

    pos = str1;
    end = str1 + len1 - len2;
    while (pos <= end) {
        if (memcmp(pos, str2, len2) == 0) {
            return value_make_int((ptrdiff_t) pos - (ptrdiff_t) str1);
        }
        ++pos;
    }

    return value_make_void();
}

/*
 * Extract substring
 *
 * If called with two arguments (string, int), this function returns
 * the substring starting at the indicated position. If called with
 * a third int argument, the third argument specifies the maximum
 * length of the returned substring.
 */
value *str_mid(intend_state *s, unsigned int argc, value **argv)
{
    value *result;
    char *str = STR_OF(argv[0]);
    int max = STRLEN_OF(argv[0]);
    int startpos = INT_OF(argv[1]);
    int count = 0;
    char *start, *buf;

    if (argc > 2) {
        value_cast_inplace(s, &argv[2], VALUE_TYPE_INT);
    }
    if (startpos < 0) {
        startpos = 0;
    }

    if (startpos > max) startpos = max;
    start = str + startpos;

    if (argc > 2) {
        count = INT_OF(argv[2]);
    } else {
        count = max - startpos;
    }
    if (count < 0) count = 0;
    if (startpos + count > max) count = max - startpos;

    buf = oom(calloc(count + 1, 1));
    memcpy(buf, start, count);

    result = value_make_memstring(buf, count);
    free(buf);
    return result;
}

/*
 * Extract left part of string
 *
 * This function returns a given number of characters from the
 * beginning of a string.
 */
value *str_left(intend_state *s, unsigned int argc, value **argv)
{
    const char *str = STR_OF(argv[0]);
    int len         = STRLEN_OF(argv[0]);
    int wanted      = INT_OF(argv[1]);
    char *buf;
    value *val;

    if (!str || len < 1) {
        return value_make_string(NULL);
    }
    if (wanted > len) {
        wanted = len;
    }

    buf = oom(calloc(wanted + 1, 1));
    memcpy(buf, str, wanted);
    val = value_make_memstring(buf, wanted);
    free(buf);

    return val;
}

/*
 * Extract right part of string
 *
 * This function returns a given number of characters from the
 * end of a string.
 */
value *str_right(intend_state *s, unsigned int argc, value **argv)
{
    const char *str = STR_OF(argv[0]);
    int len         = STRLEN_OF(argv[0]);
    int wanted      = INT_OF(argv[1]);
    char *buf;
    value *val;

    if (!str || len < 1) {
        return value_make_string(NULL);
    }
    if (wanted > len) {
        wanted = len;
    }

    buf = oom(calloc(wanted + 1, 1));
    memcpy(buf, str + len - wanted, wanted);
    val = value_make_memstring(buf, wanted);
    free(buf);

    return val;
}

/*
 * Convert string using conversion function
 */
static value *convert(value *arg, int (*conv)(int))
{
    char *pos;
    int i;

    if (STRLEN_OF(arg) > 0) {
        pos = STR_OF(arg);
        for (i = 0; i < STRLEN_OF(arg); i++) {
            *pos = conv(*pos);
            ++pos;
        }
    }
    return value_copy(arg);
}

/*
 * Convert string to lower case
 */
value *str_lower(intend_state *s, unsigned int argc, value **argv)
{
    return convert(argv[0], tolower);
}

/*
 * Convert string to upper case
 */
value *str_upper(intend_state *s, unsigned int argc, value **argv)
{
    return convert(argv[0], toupper);
}

/*
 * Check string for specified property
 */
static value *checkstr(value *arg, int (*check)(int))
{
    int ok = 0, i;
    char *pos;

    if (STRLEN_OF(arg) > 0) {
        pos = STR_OF(arg);
        for (i = 0; i < STRLEN_OF(arg); i++) {
            if (!check(*pos)) break;
            pos++;
        }
        if (i == STRLEN_OF(arg)) ok = 1;
    }
    return value_make_bool(ok);
}

/*
 * Test string for being alpha-numeric
 */
value *str_is_alnum(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isalnum);
}

/*
 * Test string for being alphabetical
 */
value *str_is_alpha(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isalpha);
}

/*
 * Test string for being control characters only
 */
value *str_is_cntrl(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], iscntrl);
}

/*
 * Test string for being digits only
 */
value *str_is_digit(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isdigit);
}

/*
 * Test string for being non-space only
 */
value *str_is_graph(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isgraph);
}

/*
 * Test string for being lower-case only
 */
value *str_is_lower(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], islower);
}

/*
 * Test string for being printable characters only
 */
value *str_is_print(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isprint);
}

/*
 * Test string for being punctuation only
 */
value *str_is_punct(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], ispunct);
}

/*
 * Test string for being whitespace only
 */
value *str_is_space(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isspace);
}

/*
 * Test string for being upper-case only
 */
value *str_is_upper(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isupper);
}

/*
 * Test string for being hexadecimal digits only
 */
value *str_is_xdigit(intend_state *s, unsigned int argc, value **argv)
{
    return checkstr(argv[0], isxdigit);
}

/*
 * Get numerical character value of first character of string
 *
 * Returns void if handed an empty string.
 */
value *str_ord(intend_state *s, unsigned int argc, value **argv)
{
    if (STRLEN_OF(argv[0]) > 0) {
        return value_make_int(STR_OF(argv[0])[0]);
    } else {
        return value_make_void();
    }
}

/*
 * Create single-character string from numerical character value
 */
value *str_chr(intend_state *s, unsigned int argc, value **argv)
{
    char buf[2];

    buf[0] = (char) INT_OF(argv[0]);
    buf[1] = 0;
    return value_make_memstring(buf, 1);
}

/*
 * Number of characters leading first string from set in second string
 */
value *str_span(intend_state *s, unsigned int argc, value **argv)
{
    char *str1 = STR_OF(argv[0]);
    int   len1 = STRLEN_OF(argv[0]);
    char *str2 = STR_OF(argv[1]);
    int   len2 = STRLEN_OF(argv[1]);
    int i, count = 0;

    if (!str1 || !str2) {
        return value_make_int(0);
    }

    for (i = 0; i < len1; i++) {
        if (memchr(str2, str1[i], len2) == NULL) break;
        ++count;
    }
    return value_make_int(count);
}

/*
 * Number of characters leading first string not from set in second string
 */
value *str_cspan(intend_state *s, unsigned int argc, value **argv)
{
    char *str1 = STR_OF(argv[0]);
    int   len1 = STRLEN_OF(argv[0]);
    char *str2 = STR_OF(argv[1]);
    int   len2 = STRLEN_OF(argv[1]);
    int i, count = 0;

    if (!str1) {
        return value_make_int(0);
    }
    if (!str2) {
        return value_make_int(len1);
    }

    for (i = 0; i < len1; i++) {
        if (memchr(str2, str1[i], len2) != NULL) break;
        ++count;
    }
    return value_make_int(count);
}

/*
 * Position of first character from set in second string
 */
value *str_pbrk(intend_state *s, unsigned int argc, value **argv)
{
    char *str1 = STR_OF(argv[0]);
    int   len1 = STRLEN_OF(argv[0]);
    char *str2 = STR_OF(argv[1]);
    int   len2 = STRLEN_OF(argv[1]);
    int i;

    if (!str1 || !str2) {
        return value_make_void();
    }

    for (i = 0; i < len1 ; i++) {
        if (memchr(str2, str1[i], len2) != NULL) {
            return value_make_int(i);
        }
    }
    return value_make_void();
}

/*
 * Compare strings based on locale setting
 */
value *str_coll(intend_state *s, unsigned int argc, value **argv)
{
    char *str1 = STR_OF(argv[0]);
    char *str2 = STR_OF(argv[1]);

    if (!str1 && !str2) {
        return value_make_int(0);
    }
    if (!str1) {
        return value_make_int(-1);
    }
    if (!str2) {
        return value_make_int(1);
    }
    return value_make_int(strcoll(str1, str2));
}

/*
 * Explode string into array of characters
 */
value *str_explode(intend_state *s, unsigned int argc, value **argv)
{
    value *res, *elem;
    char *str   = STR_OF(argv[0]);
    int   len   = STRLEN_OF(argv[0]);
    char *sep   = NULL;
    int   slen  = 0;
    char *buf, *next, *end;
    int i;

    // Create an empty result array
    res = value_make_array();

    // Check if we received a separator
    if (argc == 2) {
        sep = STR_OF(argv[1]);
        slen = STRLEN_OF(argv[1]);
    }

    // If no separator, just cast the string to array
    if (!sep) {
        return value_cast(s, argv[0], VALUE_TYPE_ARRAY);
    }

    // Make a buffer with a working copy of the string
    buf = oom(malloc(len + 1));
    strcpy(buf, str);

    // Explode the string using sep as separator
    next = buf;
    while ((next < (buf + len)) && (end = strstr(next, sep))) {
        // Mark the end of this bit
        *end = '\0';

        // Create a new element and add it to array
        elem = value_make_string(next);
        value_add_to_array(res, elem);
        value_free(elem);

        // Move to next
        next = end + slen;
    }

    // Check if there is a last element to add
    if (next < (buf + len)) {
        // Create last element and add it to array
        elem = value_make_string(next);
        value_add_to_array(res, elem);
        value_free(elem);
    }

    // Free buffer
    free(buf);

    // Return the result array
    return res;
}

/*
 * Implode array into single string
 */
value *str_implode(intend_state *s, unsigned int argc, value **argv)
{
    int max = ARRLEN_OF(argv[0]);
    int i, len, curr = 0;
    char *sep   = NULL;
    int   slen  = 0;
    char *buf = NULL;
    value *res, *elem;

    // Check if we received a separator
    if (argc == 2) {
        sep = STR_OF(argv[1]);
        slen = STRLEN_OF(argv[1]);
    }

    // If no separator, just cast the array to string
    if (!sep) {
        return value_cast(s, argv[0], VALUE_TYPE_STRING);
    }

    // Implode the array using sep as separator
    for (i = 0; i < max; i++) {
        // Get temporary element and cast it to string
        elem = value_get_array(argv[0], i);
        value_cast_inplace(s, &elem, VALUE_TYPE_STRING);

        // Get the len of element
        len = STRLEN_OF(elem);

        // Is this the first element
        if (!buf) {
            // Allocate space
            curr += len;
            buf = oom(malloc(curr + 1));
            buf[0] = 0;
        } else {
            // Allocate more space and add separator
            curr += slen + len;
            buf = oom(realloc(buf, curr + 1));
            strcat(buf, sep);
        }

        // If element not empty add it to buffer
        if (len > 0) {
            strcat(buf, STR_OF(elem));
        }

        // Free the temporary element
        value_free(elem);
    }

    // Create result string value
    res = value_make_string(buf);

    // Free the buffer
    free(buf);

    // Return the result string
    return res;
}

/*
 * Remove leading whitespace from string
 */
value *str_ltrim(intend_state *s, unsigned int argc, value **argv)
{
    char *str = STR_OF(argv[0]);

    if (!str) {
        return value_make_string(NULL);
    }

    while (isspace(*str)) ++str;
    return value_make_string(str);
}

/*
 * Remove trailing whitespace from string
 */
value *str_rtrim(intend_state *s, unsigned int argc, value **argv)
{
    char *str = STR_OF(argv[0]);
    int   len = STRLEN_OF(argv[0]);

    if (!str) {
        return value_make_string(NULL);
    }

    while (len > 1 && isspace(*(str + len - 1))) --len;
    return value_make_memstring(str, len);
}

/*
 * Remove leading and trailing whitespace from string
 */
value *str_trim(intend_state *s, unsigned int argc, value **argv)
{
    char *str = STR_OF(argv[0]);
    int   len = STRLEN_OF(argv[0]);

    if (!str) {
        return value_make_string(NULL);
    }

    while (isspace(*str)) {
        ++str;
        --len;
    }
    while (len > 1 && isspace(*(str + len - 1))) --len;
    return value_make_memstring(str, len);
}
