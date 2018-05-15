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
 * Intend C Runtime environment functions
 */

#ifndef INTEND_STDLIB_RUNTIME_H
#define INTEND_STDLIB_RUNTIME_H

void rt_init(intend_state *s);

value *rt_size_of(intend_state *s, unsigned int, value **);
value *rt_type_of(intend_state *s, unsigned int, value **);
value *rt_class_of(intend_state *s, unsigned int, value **);
value *rt_is_void(intend_state *s, unsigned int, value **);
value *rt_is_bool(intend_state *s, unsigned int, value **);
value *rt_is_int(intend_state *s, unsigned int, value **);
value *rt_is_float(intend_state *s, unsigned int, value **);
value *rt_is_string(intend_state *s, unsigned int, value **);
value *rt_is_array(intend_state *s, unsigned int, value **);
value *rt_is_struct(intend_state *s, unsigned int, value **);
value *rt_is_fn(intend_state *s, unsigned int, value **);
value *rt_is_resource(intend_state *s, unsigned int, value **);
value *rt_is_a(intend_state *s, unsigned int, value **);
value *rt_is_local(intend_state *s, unsigned int, value **);
value *rt_is_global(intend_state *s, unsigned int, value **);
value *rt_is_function(intend_state *s, unsigned int, value **);
value *rt_is_var(intend_state *s, unsigned int, value **);
value *rt_is_class(intend_state *s, unsigned int, value **);
value *rt_set(intend_state *s, unsigned int, value **);
value *rt_get(intend_state *s, unsigned int, value **);
value *rt_get_static(intend_state *s, unsigned int, value **);
value *rt_unset(intend_state *s, unsigned int, value **);
value *rt_assert(intend_state *s, unsigned int, value **);
value *rt_global(intend_state *s, unsigned int, value **);
value *rt_cast_to(intend_state *s, unsigned int, value **);
value *rt_versions(intend_state *s, unsigned int, value **);
value *rt_exit(intend_state *s, unsigned int, value **);

#endif
