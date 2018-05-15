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
 * Intend C Function calls
 */

#include <ctype.h>
#include <stdlib.h>

#include "runtime.h"

/*
 * Call function by signature
 */
value *call_function(intend_state *s, signature *sig, unsigned int argc,
                     value **argv)
{
    value *result = NULL;
    char rettype;
    int realtype;

    sanity(sig && (argc == 0 || argv));

    if (!s->except_flag && !s->exit_flag) {
        call_check(s, sig->name, sig, argc, argv);
    }

    if (!s->except_flag && !s->exit_flag) {
        switch (sig->type) {
            case FUNCTION_TYPE_BUILTIN:
                sanity(sig->call_u.builtin_vector);
                result = sig->call_u.builtin_vector(s, argc, argv);
                break;
            case FUNCTION_TYPE_USERDEF:
                sanity(sig->call_u.userdef_vector);
                result = sig->call_u.userdef_vector(s, sig->data, sig->def,
                                                    sig->args, argc, argv);
                break;
        }
    } else {
        result = value_make_void();
    }

    if (s->except_flag || s->exit_flag) {
        rettype = '?';
    } else {
        rettype = sig->rettype;
    }

    if (rettype != '?' && call_typechar(result) != tolower(rettype)) {
        if (islower(rettype) || rettype == 'P') {
            realtype = result->type;
            fatal(s, "function `%s': return type mismatch (`%s' instead of `%s')",
                  sig->name, call_typename(realtype),
                  call_typename(call_chartype(rettype))
                 );
        } else {
            value_cast_inplace(s, &result, call_chartype(tolower(rettype)));
        }
    }
    return result;
}

/*
 * Call function by name
 */
value *call_named_function(intend_state *s, const char *name,
                           unsigned int argc, value **argv)
{
    symtab_entry *entry;

    sanity(name && (argc == 0 || argv));

    entry = symtab_stack_lookup(s, name);
    if (!entry || entry->type != SYMTAB_ENTRY_FUNCTION) {
        fatal(s, "call to undefined function `%s'", name);
    }
    return call_function(s, &(entry->entry_u.fnc.sigs[0]), argc, argv);
}
