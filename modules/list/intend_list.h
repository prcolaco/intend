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
 * Intend C List functions module
 */

#ifndef INTEND_MODULE_LIST_H
#define INTEND_MODULE_LIST_H

/*
 * Initialize the List module
 */
void intend_list_module_initialize(intend_ctx ctx);

/*
 * Destroy the List module
 */
void intend_list_module_destroy(intend_ctx ctx);

/*
 * Create empty list
 */
intend_value list_nil(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Prepend element to list
 */
intend_value list_cons(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get first element of list
 */
intend_value list_head(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get all elements of list except the first
 */
intend_value list_tail(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get last element of list
 */
intend_value list_last(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get all elements of list except the last
 */
intend_value list_initial(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Take a number of elements from the front of list
 */
intend_value list_take(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Drop a number of elements from the front of list
 */
intend_value list_drop(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Length of list
 */
intend_value list_length(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Check for empty list
 */
intend_value list_null(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Check for element in list
 */
intend_value list_elem(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Intersperse value between list elements
 */
intend_value list_intersperse(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Replicate element to create list
 */
intend_value list_replicate(intend_ctx ctx, unsigned int argc, intend_value *argv);

#endif
