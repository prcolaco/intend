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
 * Intend C Library of helper functions
 *
 * Provides simple helper functions needed by the other libraries.
 */

#ifndef INTEND_MISC_H
#define INTEND_MISC_H

#include <stdio.h>

/*
 * Interpreter state
 */
typedef struct intend_state_s {
    // function pointer to registered function that opens source files
    FILE    *(*get_source)(struct intend_state_s *s, char *path);
    // io streams
    FILE    *stdin;             /* standard input stream */
    FILE    *stdout;            /* standard ouput stream */
    FILE    *stderr;            /* standard error stream */
    int     stdin_backup;       /* standard input backup file handle */
    int     stdout_backup;      /* standard output backup file handle */
    int     stderr_backup;      /* standard error backup file handle */
    // script dump vars
    int     dump_mode;          /* dump mode (0 = normal, > 0 = inside lambda expression) */
    int     dump_depth;         /* dump indent level */
    // state flags and data
    int     exit_flag;          /* script exit in progress */
    int     exit_value;         /* script return status */
    int     return_flag;        /* return in progress */
    int     continue_flag;      /* continue in progress */
    int     try_flag;           /* try in progress */
    int     except_flag;        /* exception in progress */
    void    *except_value;      /* exception value */
    int     loop_flag;          /* loop in progress */
    int     func_flag;          /* user-defined function in progress */
    int     break_flag;         /* break in progress */
    void    *retval;            /* last function return value */
    int     retval_cookie;      /* cookie of return value */
    int     global_cookie;      /* current cookie */
    int     float_count;        /* number of in-flight arguments */
    void    *float_args;        /* in-flight arguments */
    int     vector_count;       /* number of in-flight argument vectors */
    void    *vectors;           /* in-flight argument vectors */
    char    *new_cons;          /* current constructor name */
    void    *new_sig;           /* current constructor signature */
    void    *global_table;      /* global symbol table */
    int     local_depth;        /* local symbol table depth */
    void    *local_tables;      /* local symbol tables */
    char    *source_file;       /* current source file */
    int     source_line;        /* current line in source file */
    int     source_col;         /* current column in source file */
    void    *parser;            /* parser data */
    void    *script;            /* parsed script data */
    int     seed_init;          /* random generator initialization */
    int     safe_mode;          /* running script in safe mode (0=regular;1=safe mode)*/
    void    *sandboxes;         /* safe mode allowed paths (sandboxes) */
    void    *preload_modules;   /* Linked list of modules to pre-load during state init */
    void    *modules;           /* Linked list of loaded modules data */
} intend_state;

/*
 * State memory management
 */
intend_state *state_alloc(void);
void state_free(intend_state *state);

/*
 * Error printing
 */
void fatal(intend_state *s, const char *msg, ...);
void nonfatal(intend_state *s, const char *msg, ...);
void internal(const char *file, int line);

/*
 * Utilities
 */
char *xstrdup(const char *orig);

/*
 * Sanity checking macro
 */
#define sanity(x) do { if (!(x)) internal(__FILE__, __LINE__); } while(0)

/*
 * Memory allocation checking
 */
void *oom(void *ptr);

#endif
