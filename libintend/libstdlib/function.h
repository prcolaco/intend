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
 * Intend C Functions on functions
 */

#ifndef INTEND_STDLIB_FUNCTION_H
#define INTEND_STDLIB_FUNCTION_H

void fn_init(intend_state *s);

value *fn_is_builtin(intend_state *s, unsigned int, value **);
value *fn_is_userdef(intend_state *s, unsigned int, value **);
value *fn_name(intend_state *s, unsigned int, value **);
value *fn_call(intend_state *s, unsigned int, value **);
value *fn_call_array(intend_state *s, unsigned int, value **);
value *fn_call_method(intend_state *s, unsigned int, value **);
value *fn_call_method_array(intend_state *s, unsigned int, value **);
value *fn_prototype(intend_state *s, unsigned int, value **);
value *fn_map(intend_state *s, unsigned int, value **);
value *fn_filter(intend_state *s, unsigned int, value **);
value *fn_foldl(intend_state *s, unsigned int, value **);
value *fn_foldr(intend_state *s, unsigned int, value **);
value *fn_take_while(intend_state *s, unsigned int, value **);
value *fn_drop_while(intend_state *s, unsigned int, value **);

#endif
