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

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <intend.h>

#include "intend_dyn.h"

/*
 * Dynamic Library functions to register
 */
static intend_function_data dyn_funcs[] = {
    /* foreign function interface */
    { "is_dyn_resource",    dyn_is_resource,    1,  "r",    'b' },
    { "dyn_open",           dyn_open,           1,  "s",    '?' },
    { "dyn_close",      dyn_close,          1,  "r",    'v' },
    { "dyn_fn_pointer", dyn_fn_pointer,     1,  "rs",   '?' },
    { "dyn_call_void",  dyn_call_void,      2,  "rs",   'v' },
    { "dyn_call_int",       dyn_call_int,       2,  "rs",   '?' },
    { "dyn_call_float", dyn_call_float,     2,  "rs",   '?' },
    { "dyn_call_ptr",       dyn_call_ptr,       2,  "rs",   '?' },
    { "cfloat",         dyn_c_float,        1,  "F",    '?' },

    /* list terminator */
    { NULL,     NULL,           0,  NULL,   0   }
};

/*
 * Initialize the Dynamic Library module
 */
void intend_dyn_module_initialize(intend_ctx ctx)
{
    intend_register_function_array(ctx, dyn_funcs);
}

/*
 * Destroy the Dynamic Library module
 */
void intend_dyn_module_destroy(intend_ctx ctx)
{
    intend_unregister_function_array(ctx, dyn_funcs);
}

/*
 * Free C library resource
 */
static void dyn_free(void *data)
{
    if (data) dlclose(data);
}

/*
 * Free Resource Data
 */
static void res_free(void *data)
{
    if (data) free(data);
}

/*
 * Check whether resource is library resource
 */
static int is_lib(intend_value val)
{
    return (((void (*)(void *))intend_resource_release(val)) == dyn_free);
}

/*
 * Check whether resource is library resource
 */
intend_value dyn_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int ret = is_lib(argv[0]);
    return intend_create_value(INTEND_TYPE_BOOL, &ret);
}

/*
 * Call a C function that returns nothing
 */
static void voidcall(void *f, long *a, double *d)
{
    void (*func)() = f;

    (*func)(
        a[ 0], a[ 1], a[ 2], a[ 3], a[ 4], a[ 5], a[ 6], a[ 7], a[ 8], a[ 9],
        a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19],
        a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29],
        a[30], a[31], a[32], a[33], a[34], a[35], a[36], a[37], a[38], a[39],
        d[ 0], d[ 1], d[ 2], d[ 3], d[ 4], d[ 5], d[ 6], d[ 7], d[ 8], d[ 9],
        d[10], d[11], d[12], d[13], d[14], d[15], d[16], d[17], d[18], d[19]
    );
}

/*
 * Call a C function that returns an int
 */
static int intcall(void *f, long *a, double *d)
{
    int (*func)() = f;
    int res = 0;

    res = (*func)(
              a[ 0], a[ 1], a[ 2], a[ 3], a[ 4], a[ 5], a[ 6], a[ 7], a[ 8], a[ 9],
              a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19],
              a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29],
              a[30], a[31], a[32], a[33], a[34], a[35], a[36], a[37], a[38], a[39],
              d[ 0], d[ 1], d[ 2], d[ 3], d[ 4], d[ 5], d[ 6], d[ 7], d[ 8], d[ 9],
              d[10], d[11], d[12], d[13], d[14], d[15], d[16], d[17], d[18], d[19]
          );
    return res;
}

/*
 * Call a C function that returns a double
 */
static double floatcall(void *f, long *a, double *d)
{
    double(*func)() = f;
    double res = 0.0;

    res = (*func)(
              a[ 0], a[ 1], a[ 2], a[ 3], a[ 4], a[ 5], a[ 6], a[ 7], a[ 8], a[ 9],
              a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19],
              a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29],
              a[30], a[31], a[32], a[33], a[34], a[35], a[36], a[37], a[38], a[39],
              d[ 0], d[ 1], d[ 2], d[ 3], d[ 4], d[ 5], d[ 6], d[ 7], d[ 8], d[ 9],
              d[10], d[11], d[12], d[13], d[14], d[15], d[16], d[17], d[18], d[19]
          );
    return res;
}

/*
 * Call a C function that returns a pointer
 */
static void *ptrcall(void *f, long *a, double *d)
{
    void *(*func)() = f;
    void *res = NULL;

    res = (*func)(
              a[ 0], a[ 1], a[ 2], a[ 3], a[ 4], a[ 5], a[ 6], a[ 7], a[ 8], a[ 9],
              a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19],
              a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29],
              a[30], a[31], a[32], a[33], a[34], a[35], a[36], a[37], a[38], a[39],
              d[ 0], d[ 1], d[ 2], d[ 3], d[ 4], d[ 5], d[ 6], d[ 7], d[ 8], d[ 9],
              d[10], d[11], d[12], d[13], d[14], d[15], d[16], d[17], d[18], d[19]
          );
    return res;
}

/*
 * Build int vector out of value array
 */
static long *makeargs(unsigned int argc, intend_value *argv, double **dargs)
{
    unsigned int i;
    unsigned int size = 0, pos = 0, dsize = 0, dpos = 0, dcount = 0;
    long *args;
    long *dput;
    char **sput;
    void **pput, *resval;

    for (i = 0; i < argc; ++i) {
        switch (intend_value_type(argv[i])) {
            case INTEND_TYPE_VOID:
            case INTEND_TYPE_BOOL:
            case INTEND_TYPE_INT:
                size += sizeof(int);
                break;
            case INTEND_TYPE_FLOAT:
                size += sizeof(double);
                ++dsize;
                break;
            case INTEND_TYPE_STRING:
                size += sizeof(char *);
                break;
            case INTEND_TYPE_RES:
                size += sizeof(void *);
                break;
            default:
                return NULL;
        }
    }

    if (size > 40 * sizeof(int) || dsize > 20) {
        return NULL;
    }

    if (size < 40 * sizeof(int)) {
        size = 40 * sizeof(int);
    }
    args = intend_oom(calloc(size, 1));

    if (dsize < 20) dsize = 20;
    *dargs = intend_oom(calloc(dsize, sizeof(double)));

    for (i = 0; i < argc; ++i) {
        switch (intend_value_type(argv[i])) {
            case INTEND_TYPE_VOID:
                args[pos++] = 0;
                break;
            case INTEND_TYPE_BOOL:
                args[pos++] = intend_bool_value(argv[i]);
                break;
            case INTEND_TYPE_INT:
                args[pos++] = intend_int_value(argv[i]);
                break;
            case INTEND_TYPE_FLOAT:
                ++dcount;
                *dargs[dpos++] = intend_double_value(argv[i]);
                if (dcount > NOSTACK_FLOATS) {
                    /*
                     * double needs to be stored as two separate 32-bit
                     * stores on 32-bit machines or else SPARC will bus
                     * error on mis-alignment
                     */
                    double d = intend_double_value(argv[i]);
                    dput        = (long *) & d;
                    args[pos++] = *dput;
                    if (sizeof(long) < sizeof(double)) {
                        args[pos++] = *(dput + 1);
                    }
                }
                break;
            case INTEND_TYPE_STRING:
                sput  = (char **)(args + pos);
                *sput = intend_string_ptr(argv[i]);
                pos  += sizeof(char *) / sizeof(long);
                break;
            case INTEND_TYPE_RES:
                resval = intend_resource_value(argv[i]);
                pput  = (void **)(args + pos);
                *pput = resval;
                pos  += sizeof(void *) / sizeof(long);
                break;
            default:
                sanity(0);
        }
    }

    return args;
}

/*
 * Call C function by name and argument list
 */
static intend_value *ccall(unsigned int argc, intend_value *argv, char type, int mfree)
{
    void *handle = intend_resource_value(argv[0]);
    char *name = intend_string_ptr(argv[1]);
    long *args;
    double *dargs;
    void *func;
    int ires;
    double fres;
    void *pres;
    void *rel;
    intend_value res;

    if (!is_lib(argv[0]) || !name || !handle) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    func = dlsym(handle, name);
    if (!func) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    args = makeargs(argc - 2, argv + 2, &dargs);
    if (!args) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    switch (type) {
        case 'v':
            voidcall(func, args, dargs);
            res = intend_create_value(INTEND_TYPE_VOID, NULL);
            break;
        case 'i':
            ires = intcall(func, args, dargs);
            res = intend_create_value(INTEND_TYPE_INT, &ires);
            break;
        case 'f':
            fres = floatcall(func, args, dargs);
            res = intend_create_value(INTEND_TYPE_FLOAT, &fres);
            break;
        case 'p':
            pres = ptrcall(func, args, dargs);
            rel = mfree ? res_free : NULL;
            res = intend_create_value(INTEND_TYPE_RES, pres, rel);
            break;
        default:
            res = intend_create_value(INTEND_TYPE_VOID, NULL);
            break;
    }

    free(args);
    free(dargs);
    return res;
}

/*
 * Call C function that returns nothing
 */
intend_value dyn_call_void(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return ccall(argc, argv, 'v', 0);
}

/*
 * Call C function that returns an int (or compatible value)
 */
intend_value dyn_call_int(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return ccall(argc, argv, 'i', 0);
}

/*
 * Call C function that returns a float
 */
intend_value dyn_call_float(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return ccall(argc, argv, 'f', 0);
}

/*
 * Call C function that returns a pointer
 */
intend_value dyn_call_ptr(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value freeval;
    int free = 0, nargc = argc;

    freeval = argv[argc - 1];
    if (intend_value_type(freeval) == INTEND_TYPE_BOOL) {
        free = intend_bool_value(freeval);
        --nargc;
    }

    return ccall(nargc, argv, 'p', free);
}

/*
 * Dynamically load a C library
 */
intend_value dyn_open(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *name = intend_string_ptr(argv[0]);
    void *handle;

    if (!value_str_compat(argv[0])) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    handle = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    return intend_create_value(INTEND_TYPE_RES, handle, dyn_free);
}

/*
 * Unload a previously loaded C library
 */
intend_value dyn_close(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *handle = intend_resource_value(argv[0]);

    if (is_lib(argv[0]) && handle) {
        dlclose(handle);
        intend_resource_value_set(argv[0], NULL);
    }
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Get pointer to C function
 */
intend_value dyn_fn_pointer(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    void *handle = intend_resource_value(argv[0]);
    char *name = intend_string_ptr(argv[1]);
    void *func;

    if (!is_lib(argv[0]) || !value_str_compat(argv[1])) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    func = dlsym(handle, name);
    if (!func) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }
    return intend_create_value(INTEND_TYPE_RES, func, NULL);
}

/*
 * Convert float to call-stack representation
 */
intend_value dyn_c_float(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    double dval = intend_double_value(argv[0]);
    float fval = dval;
    long *lptr = (void *) & fval;

    if (sizeof(float) == sizeof(long)) {
        /* bits in float == bits in long, pass as integer */
        return intend_create_value(INTEND_TYPE_INT, lptr);
    } else {
        /* bits in float != bits in long, pass as double */
        return intend_create_value(INTEND_TYPE_FLOAT, &dval);
    }
}
