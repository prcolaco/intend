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
 * Intend C Function call memory
 */

#include <stdlib.h>
#include <string.h>

#include "runtime.h"

/*
 * Allocate function signature
 *
 * This function tries to allocate a new function signature structure
 * filled with zeros.
 */
signature *call_sig_alloc(void)
{
    signature *sig;

    sig = oom(calloc(sizeof(signature), 1));
    sig->rettype = '?';

    return sig;
}

/*
 * Copy signature to pre-allocated signature
 *
 * This function makes a complete copy of the given function signature.
 */
signature *call_sig_copy_to(signature *copy, const signature *sig)
{
    char *proto = NULL;
    char *name = NULL;

    // Avoid null signatures
    if (!sig) return NULL;

    proto = xstrdup(sig->proto);
    name = xstrdup(sig->name);

    memcpy(copy, sig, sizeof(signature));
    copy->proto = proto;
    copy->name  = name;
    return copy;
}

/*
 * Copy signature
 *
 * This function makes a complete copy of the given function signature.
 * The prototype string contained will be a newly allocated copy.
 */
signature *call_sig_copy(const signature *sig)
{
    signature *copy;

    // Avoid null signatures
    if (!sig) return NULL;

    copy = call_sig_alloc();

    return call_sig_copy_to(copy, sig);
}

/*
 * Cleans function signature
 *
 * This function frees the contained prototype strings.
 */
void call_sig_cleanup(signature *sig)
{
    // Avoid null signatures
    if (!sig) return;

    if (sig->name) free(sig->name);
    if (sig->proto) free(sig->proto);
    memset(sig, 0, sizeof(signature));
}

/*
 * Free function signature
 *
 * This function frees the given function signature after freeing the
 * contained prototype string.
 */
void call_sig_free(signature *sig)
{
    // Avoid null signatures
    if (!sig) return;

    // Cleanup signature insiders
    call_sig_cleanup(sig);

    // Free self
    free(sig);
}

/*
 * Create signature for builtin function
 *
 * This function creates a function signature for a builtin function.
 * The number of arguments, function prototype, and pointer to the
 * builtin function need to be given.
 */
signature *call_sig_builtin(const char *name, unsigned int args,
                            const char *proto, call_func vector)
{
    signature *sig;
    int proto_len;
    char *proto_copy;
    int name_len;
    char *name_copy;

    sig = call_sig_alloc();

    proto_len = strlen(proto);
    proto_copy = oom(malloc(proto_len + 1));
    strcpy(proto_copy, proto);

    name_len = strlen(name);
    name_copy = oom(malloc(name_len + 1));
    strcpy(name_copy, name);

    sig->type = FUNCTION_TYPE_BUILTIN;
    sig->args = args;
    sig->name = name_copy;
    sig->proto = proto_copy;
    sig->call_u.builtin_vector = vector;
    return sig;
}
