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

#ifndef INTEND_MODULE_PREG_H
#define INTEND_MODULE_PREG_H

/*
 * Internal data for a compiled regular expression
 */
typedef struct {
    pcre            *data;
    unsigned char   *table;
} pregdata;

/*
 * Initialize the PREG Regular Expression module
 */
void intend_preg_module_initialize(intend_ctx ctx);

/*
 * Destroy the PREG Regular Expression module
 */
void intend_preg_module_destroy(intend_ctx ctx);

/*
 * Check whether resourse is PREG resource
 */
intend_value preg_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compile regular expression into PREG resource
 */
intend_value preg_compile(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Run compiled regular expression on some string
 *
 * This function tries to match an input string with a previously
 * compiled regular expression. Depending on the matches parameter,
 * either a bool or an array of matching substrings is returned.
 */
static intend_value preg_run(intend_ctx ctx, unsigned int argc, intend_value *argv,
  int matches);

/*
 * Try to match regular expression
 *
 * This function returns true if a precompiled regular expression
 * matches a given string. It returns false otherwise.
 */
intend_value preg_match(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Return matching substrings
 *
 * This function returns an array of matching substring for the
 * given precompiled regular expression and input string. The
 * result is an empty array if the regexp does not match.
 */
intend_value preg_exec(intend_ctx ctx, unsigned int argc, intend_value *argv);

/* 
 * Free data of a precompiled regular expression
 */
intend_value preg_free(intend_ctx ctx, unsigned int argc, intend_value *argv);


#endif

