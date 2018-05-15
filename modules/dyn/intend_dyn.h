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
 * Intend C Dynamic Library functions interface module
 */

#ifndef INTEND_MODULE_DYN_H
#define INTEND_MODULE_DYN_H

/*
* double needs to be stored as two separate 32-bit
* stores on 32-bit machines or else SPARC will bus
* error on mis-alignment
*/
#ifndef NOSTACK_FLOATS
#define NOSTACK_FLOATS 0
#endif

/*
 * Initialize the Dynamic Library module
 */
void intend_dyn_module_initialize(intend_ctx ctx);

/*
 * Destroy the Dynamic Library module
 */
void intend_dyn_module_destroy(intend_ctx ctx);

/*
 * Check whether resource is library resource
 */
intend_value dyn_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Call C function that returns nothing
 */
intend_value dyn_call_void(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Call C function that returns an int (or compatible value)
 */
intend_value dyn_call_int(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Call C function that returns a float
 */
intend_value dyn_call_float(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Call C function that returns a pointer
 */
intend_value dyn_call_ptr(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Dynamically load a C library
 */
intend_value dyn_open(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Unload a previously loaded C library
 */
intend_value dyn_close(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get pointer to C function
 */
intend_value dyn_fn_pointer(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Convert float to call-stack representation
 */ 
intend_value dyn_c_float(intend_ctx ctx, unsigned int argc, intend_value *argv);

#endif
