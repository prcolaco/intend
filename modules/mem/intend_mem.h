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
 * Intend C Raw Memory Allocation module
 */

#ifndef INTEND_MODULE_MEM_H
#define INTEND_MODULE_MEM_H

/*
 * Private wrapper structure for allocated memory
 */
typedef struct {
    int     size;
    int     rw;
    void    *data;
} memblock;

/*
 * Initialize the Raw Memory Allocation module
 */
void intend_mem_module_initialize(intend_ctx ctx);

/*
 * Destroy the Raw Memory Allocation module
 */
void intend_mem_module_destroy(intend_ctx ctx);

/*
 * Check whether resource is memory resource
 */
intend_value mem_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Allocate a given number of bytes
 */
intend_value mem_malloc(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Allocate a given number of zero-filled bytes
 */
intend_value mem_calloc(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Reallocate memory with a new size
 */
intend_value mem_realloc(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Free a memory resource
 */
intend_value mem_free(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Create resource for C NULL pointer
 */
intend_value mem_cnull(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Check for resource containing C NULL pointer
 */
intend_value mem_is_null(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Create resource from string value
 */
intend_value mem_cstring(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Put character at offset in memory resource
 */
intend_value mem_put_char(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Put short integer at offset in memory resource
 */
intend_value mem_put_short(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Put integer at offset in memory resource
 */
intend_value mem_put_int(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Put float at offset in memory resource
 */
intend_value mem_put_float(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Put double at offset in memory resource
 */
intend_value mem_put_double(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Put string at offset into memory resource
 */
intend_value mem_put_string(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Put pointer to another resource at offset in resource
 */
intend_value mem_put_pointer(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get character from offset in memory resource
 */
intend_value mem_get_char(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get short integer from offset in memory resource
 */
intend_value mem_get_short(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get integer from offset in memory resource
 */
intend_value mem_get_int(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get float from offset in memory resource
 */
intend_value mem_get_float(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get double from offset in memory resource
 */
intend_value mem_get_double(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Create memory resource from direct pointer
 */
intend_value mem_make_pointer(void *data, int free);

/*
 * Get memory resource from offset in memory resource
 */
intend_value mem_get_pointer(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get string value from offset in memory resource
 */
intend_value mem_string(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Get fixed-length string value from offset in memory resource
 */
intend_value mem_get_string(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Copy bytes from memory resource to memory resource
 *
 * This routine will corrupt memory if source and destination
 * memory resources overlap.
 */
intend_value mem_cpy(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Copy bytes from memory resource to memory resource
 *
 * This routine will NOT corrupt memory when source and destination
 * memory resources overlap.
 */
intend_value mem_move(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Compare bytes from two memory resources
 */
intend_value mem_cmp(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Look for byte in memory resource
 */
intend_value mem_chr(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Set bytes in memory resource
 */
intend_value mem_set(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Check whether memory resource is writeable
 */
intend_value mem_is_rw(intend_ctx ctx, unsigned int argc, intend_value *argv);

/*
 * Return current allocation size of memory resource (0 = unknown)
 */
intend_value mem_size(intend_ctx ctx, unsigned int argc, intend_value *argv);


#endif

