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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <intend.h>

#include "intend_mem.h"

/*
 * Raw Memory Allocation functions to register
 */
static intend_function_data mem_funcs[] = {
    { "is_mem_resource",    mem_is_resource,    1,  "r",    'b' },
    { "malloc",             mem_malloc,         1,  "i",    '?' },
    { "calloc",             mem_calloc,         2,  "ii",   '?' },
    { "realloc",            mem_realloc,        2,  "ri",   'b' },
    { "free",               mem_free,           1,  "r",    'v' },
    { "cnull",              mem_cnull,          0,  "",     'r' },
    { "is_null",            mem_is_null,        1,  "r",    'b' },
    { "cstring",            mem_cstring,        1,  "S",    '?' },
    { "mputchar",           mem_put_char,       3,  "rii",  'b' },
    { "mputshort",          mem_put_short,      3,  "rii",  'b' },
    { "mputint",            mem_put_int,        3,  "rii",  'b' },
    { "mputfloat",          mem_put_float,      3,  "rif",  'b' },
    { "mputdouble",         mem_put_double,     3,  "rif",  'b' },
    { "mputstring",         mem_put_string,     3,  "riS",  'b' },
    { "mputptr",            mem_put_pointer,    3,  "rir",  'b' },
    { "mgetchar",           mem_get_char,       2,  "ri",   '?' },
    { "mgetshort",          mem_get_short,      2,  "ri",   '?' },
    { "mgetint",            mem_get_int,        2,  "ri",   '?' },
    { "mgetfloat",          mem_get_float,      2,  "ri",   '?' },
    { "mgetdouble",         mem_get_double,     2,  "ri",   '?' },
    { "mgetstring",         mem_get_string,     3,  "rii",  '?' },
    { "mgetptr",            mem_get_pointer,    3,  "rib",  '?' },
    { "mstring",            mem_string,         2,  "ri",   '?' },
    { "is_rw",              mem_is_rw,          1,  "r",    'b' },
    { "msize",              mem_size,           1,  "r",    '?' },
    { "memcpy",             mem_cpy,            5,  "ririi", 'b' },
    { "memmove",            mem_move,           5,  "ririi", 'b' },
    { "memcmp",             mem_cmp,            5,  "ririi", '?' },
    { "memchr",             mem_chr,            4,  "riii", '?' },
    { "memset",             mem_set,            4,  "riii", 'b' },

    /* list terminator */
    { NULL,                 NULL,               0,  NULL,   0   }
};

/*
 * Initialize the Raw Memory Allocation module
 */
void intend_mem_module_initialize(intend_ctx ctx)
{
    intend_register_function_array(ctx, mem_funcs);
}

/*
 * Destroy the Raw Memory Allocation module
 */
void intend_mem_module_destroy(intend_ctx ctx)
{
    intend_unregister_function_array(ctx, mem_funcs);
}

/*
 * Initialize memory block structure
 */
static memblock *mem_init(int size, void *data)
{
    memblock *mem;

    mem = intend_oom(malloc(sizeof(memblock)));
    mem->size = size;
    mem->data = data;
    mem->rw   = 1;

    return mem;
}

/*
 * Clean up memory block structure
 */
static void mem_cleanup(void *data)
{
    memblock *mem = data;

    if (mem) free(mem->data);
    free(mem);
}

/*
 * Clean up memory block structure put not contained pointer
 */
static void mem_halfcleanup(void *data)
{
    free(data);
}

/*
 * Check whether resource is memory resource
 */
static int is_mem(intend_value val)
{
    return (intend_resource_release(val) == mem_cleanup ||
            intend_resource_release(val) == mem_halfcleanup);
}

/*
 * Get memory pointer from memory resource
 */
static void *mem_get(void *data)
{
    memblock *mem = data;

    if (!mem) {
        return NULL;
    }
    return mem->data;
}

/*
 * Check whether resource is memory resource
 */
intend_value mem_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int res = is_mem(argv[0]);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Allocate a given number of bytes
 */
intend_value mem_malloc(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int size = intend_int_value(argv[0]);
    void *mem;
    intend_value res;

    mem = malloc(size);
    if (!mem) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    res = intend_create_value(INTEND_TYPE_RES, mem_init(size, mem), mem_cleanup, mem_get);
    return res;
}

/*
 * Allocate a given number of zero-filled bytes
 */
intend_value mem_calloc(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int size = intend_int_value(argv[0]) * intend_int_value(argv[1]);
    void *mem;
    intend_value res;

    mem = calloc(intend_int_value(argv[0]), intend_int_value(argv[1]));
    if (!mem) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    res = intend_create_value(INTEND_TYPE_RES, mem_init(size, mem), mem_cleanup, mem_get);
    return res;
}

/*
 * Reallocate memory with a new size
 */
intend_value mem_realloc(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    int size = intend_int_value(argv[1]);
    void *data;
    int res = 0;

    if (!is_mem(argv[0]) || !mem || size < 0) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    data = realloc(mem->data, size);
    if (!data) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    mem->data = data;
    mem->size = size;
    res = 1;
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Free a memory resource
 */
intend_value mem_free(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);

    if (is_mem(argv[0]) && mem) {
        mem_cleanup(mem);
        intend_resource_value_set(argv[0], NULL);
    }
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Create resource for C NULL pointer
 */
intend_value mem_cnull(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return intend_create_value(INTEND_TYPE_RES, NULL, mem_cleanup, mem_get);
}

/*
 * Check for resource containing C NULL pointer
 */
intend_value mem_is_null(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);

    int res = is_mem(argv[0]) && (!mem || !mem->data);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Create resource from string value
 */
intend_value mem_cstring(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *str = intend_string_ptr(argv[0]);
    int len;
    char *mem;
    intend_value res;

    if (str && !value_str_compat(argv[0])) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    if (!str) {
        str = "";
    }

    len = strlen(str) + 1;
    mem = intend_oom(malloc(len));
    strcpy(mem, str);

    res = intend_create_value(INTEND_TYPE_RES, mem_init(len, mem), mem_cleanup, mem_get);
    return res;
}

/*
 * Put character at offset in memory resource
 */
intend_value mem_put_char(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *put;
    int offset = intend_int_value(argv[1]);
    char val = intend_int_value(argv[2]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (mem->size < offset + sizeof(char)) {
        mem->data = intend_oom(realloc(mem->data, offset + sizeof(char)));
        mem->size = offset + sizeof(char);
    }

    put = mem->data;
    put += offset;
    *put = val;
    res = 1;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Put short integer at offset in memory resource
 */
intend_value mem_put_short(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    short *put;
    int offset = intend_int_value(argv[1]);
    short val = intend_int_value(argv[2]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (mem->size < offset + sizeof(short)) {
        mem->data = intend_oom(realloc(mem->data, offset + sizeof(short)));
        mem->size = offset + sizeof(short);
    }

    pos = mem->data;
    pos += offset;
    put = (short *) pos;
    *put = val;
    res = 1;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Put integer at offset in memory resource
 */
intend_value mem_put_int(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    int *put;
    int offset = intend_int_value(argv[1]);
    int val = intend_int_value(argv[2]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (mem->size < offset + sizeof(int)) {
        mem->data = intend_oom(realloc(mem->data, offset + sizeof(int)));
        mem->size = offset + sizeof(int);
    }

    pos = mem->data;
    pos += offset;
    put = (int *) pos;
    *put = val;
    res = 1;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Put float at offset in memory resource
 */
intend_value mem_put_float(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    float *put;
    int offset = intend_int_value(argv[1]);
    float val = intend_double_value(argv[2]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (mem->size < offset + sizeof(float)) {
        mem->data = intend_oom(realloc(mem->data, offset + sizeof(float)));
        mem->size = offset + sizeof(float);
    }

    pos = mem->data;
    pos += offset;
    put = (float *) pos;
    *put = val;
    res = 1;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Put double at offset in memory resource
 */
intend_value mem_put_double(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    double *put;
    int offset = intend_int_value(argv[1]);
    double val = intend_double_value(argv[2]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (mem->size < offset + sizeof(double)) {
        mem->data = intend_oom(realloc(mem->data, offset + sizeof(double)));
        mem->size = offset + sizeof(double);
    }

    pos = mem->data;
    pos += offset;
    put = (double *) pos;
    *put = val;
    res = 1;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Put string at offset into memory resource
 */
intend_value mem_put_string(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    int offset = intend_int_value(argv[1]);
    int len    = intend_string_len(argv[2]);
    char *str  = intend_string_ptr(argv[2]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (mem->size < offset + len) {
        mem->data = intend_oom(realloc(mem->data, offset + len));
        mem->size = offset + len;
    }

    pos = mem->data;
    pos += offset;
    memcpy(pos, str, len);
    res = 1;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Put pointer to another resource at offset in resource
 */
intend_value mem_put_pointer(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    memblock *ptr = intend_resource_value(argv[2]);
    char *pos;
    void **put;
    int offset = intend_int_value(argv[1]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw || !is_mem(argv[2])) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (mem->size < offset + sizeof(void *)) {
        mem->data = intend_oom(realloc(mem->data, offset + sizeof(void *)));
        mem->size = offset + sizeof(void *);
    }

    pos = mem->data;
    pos += offset;
    put = (void **) pos;
    if (ptr) {
        *put = ptr->data;
    } else {
        *put = NULL;
    }
    res = 1;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Get character from offset in memory resource
 */
intend_value mem_get_char(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *get;
    int offset = intend_int_value(argv[1]);

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + sizeof(char) > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    get = mem->data;
    get += offset;

    return intend_create_value(INTEND_TYPE_INT, get);
}

/*
 * Get short integer from offset in memory resource
 */
intend_value mem_get_short(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    short *get;
    int offset = intend_int_value(argv[1]);

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + sizeof(short) > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    pos = mem->data;
    pos += offset;
    get = (short *) pos;

    return intend_create_value(INTEND_TYPE_INT, get);
}

/*
 * Get integer from offset in memory resource
 */
intend_value mem_get_int(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    int *get;
    int offset = intend_int_value(argv[1]);

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + sizeof(int) > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    pos = mem->data;
    pos += offset;
    get = (int *) pos;

    return intend_create_value(INTEND_TYPE_INT, get);
}

/*
 * Get float from offset in memory resource
 */
intend_value mem_get_float(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    float *get;
    int offset = intend_int_value(argv[1]);

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + sizeof(float) > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    pos = mem->data;
    pos += offset;
    get = (float *) pos;

    return intend_create_value(INTEND_TYPE_FLOAT, get);
}

/*
 * Get double from offset in memory resource
 */
intend_value mem_get_double(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *pos;
    double *get;
    int offset = intend_int_value(argv[1]);

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + sizeof(double) > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    pos = mem->data;
    pos += offset;
    get = (double *) pos;

    return intend_create_value(INTEND_TYPE_FLOAT, get);
}

/*
 * Create memory resource from direct pointer
 */
intend_value mem_make_pointer(void *data, int free)
{
    memblock *mem;
    intend_value res;

    res = intend_create_value(INTEND_TYPE_RES, mem_init(0, data),
                              free ? mem_cleanup : mem_halfcleanup, mem_get);
    mem = intend_resource_value(res);
    mem->rw = 0;

    return res;
}

/*
 * Get memory resource from offset in memory resource
 */
intend_value mem_get_pointer(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    memblock *resmem;
    char *pos;
    void **get;
    int offset = intend_int_value(argv[1]);
    int cleanup = intend_bool_value(argv[2]);
    intend_value res;

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + sizeof(void *) > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    pos = mem->data;
    pos += offset;
    get = (void **) pos;

    res = intend_create_value(INTEND_TYPE_RES, mem_init(0, *get),
                              cleanup ? mem_cleanup : mem_halfcleanup, mem_get);
    resmem = intend_resource_value(res);
    resmem->rw = 0;

    return res;
}

/*
 * Get string value from offset in memory resource
 */
intend_value mem_string(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    int offset = intend_int_value(argv[1]);
    char *get;

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + sizeof(char) > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    get = mem->data;
    get += offset;

    return intend_create_value(INTEND_TYPE_STRING, get, NULL);
}

/*
 * Get fixed-length string value from offset in memory resource
 */
intend_value mem_get_string(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    char *get;
    int offset = intend_int_value(argv[1]);
    int len    = intend_int_value(argv[2]);

    if (!is_mem(argv[0]) || !mem ||
            (mem->size && offset + len > mem->size)) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    get = mem->data;
    get += offset;

    return intend_create_value(INTEND_TYPE_STRING, get, &len);
}

/*
 * Copy bytes from memory resource to memory resource
 *
 * This routine will corrupt memory if source and destination
 * memory resources overlap.
 */
intend_value mem_cpy(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *dst = intend_resource_value(argv[0]);
    int dst_off   = intend_int_value(argv[1]);
    memblock *src = intend_resource_value(argv[2]);
    int src_off   = intend_int_value(argv[3]);
    int count     = intend_int_value(argv[4]);
    int res = 0;

    if (!is_mem(argv[0]) || !dst || !is_mem(argv[2]) || !src) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }
    res = 1;
    if (count == 0) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (count > src->size - src_off) {
        count = src->size - src_off;
    }
    if (dst_off + count > dst->size) {
        dst->data = intend_oom(realloc(dst->data, dst_off + count));
        dst->size = dst_off + count;
    }

    memcpy(dst->data + dst_off, src->data + src_off, count);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Copy bytes from memory resource to memory resource
 *
 * This routine will NOT corrupt memory when source and destination
 * memory resources overlap.
 */
intend_value mem_move(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *dst = intend_resource_value(argv[0]);
    int dst_off   = intend_int_value(argv[1]);
    memblock *src = intend_resource_value(argv[2]);
    int src_off   = intend_int_value(argv[3]);
    int count     = intend_int_value(argv[4]);
    int res = 0;

    if (!is_mem(argv[0]) || !dst || !is_mem(argv[2]) || !src) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }
    res = 1;
    if (count == 0) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }

    if (count > src->size - src_off) {
        count = src->size - src_off;
    }
    if (dst_off + count > dst->size) {
        dst->data = intend_oom(realloc(dst->data, dst_off + count));
        dst->size = dst_off + count;
    }

    memmove(dst->data + dst_off, src->data + src_off, count);
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Compare bytes from two memory resources
 */
intend_value mem_cmp(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *one = intend_resource_value(argv[0]);
    int one_off   = intend_int_value(argv[1]);
    memblock *two = intend_resource_value(argv[2]);
    int two_off   = intend_int_value(argv[3]);
    int count     = intend_int_value(argv[4]);
    int res;

    if (!is_mem(argv[0]) || !one || !is_mem(argv[2]) || !two ||
            one_off + count > one->size || two_off + count > two->size) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    res = memcmp(one->data + one_off, two->data + two_off, count);
    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Look for byte in memory resource
 */
intend_value mem_chr(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    int offset    = intend_int_value(argv[1]);
    int search    = intend_int_value(argv[2]);
    int count     = intend_int_value(argv[3]);
    void *where   = NULL;
    int res;

    if (!is_mem(argv[0]) || !mem || offset + count > mem->size) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    where = memchr(mem->data + offset, search, count);
    if (!where) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    res = where - mem->data;
    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Set bytes in memory resource
 */
intend_value mem_set(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    int offset    = intend_int_value(argv[1]);
    int data      = intend_int_value(argv[2]);
    int count     = intend_int_value(argv[3]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }
    if (offset + count > mem->size) {
        mem->data = intend_oom(realloc(mem->data, offset + count));
        mem->size = offset + count;
    }
    memset(mem->data + offset, data, count);
    res = 1;
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Check whether memory resource is writeable
 */
intend_value mem_is_rw(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    int res = 0;

    if (!is_mem(argv[0]) || !mem || !mem->rw) {
        return intend_create_value(INTEND_TYPE_BOOL, &res);
    }
    res = 1;
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Return current allocation size of memory resource (0 = unknown)
 */
intend_value mem_size(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    memblock *mem = intend_resource_value(argv[0]);
    int res = 0;

    if (!is_mem(argv[0])) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    if (!mem) {
        return intend_create_value(INTEND_TYPE_INT, &res);
    }
    res = mem->size;
    return intend_create_value(INTEND_TYPE_INT, &res);
}
