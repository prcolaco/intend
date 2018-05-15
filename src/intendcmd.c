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
 *   it under the terms of the GNU General Public License as               *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
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
 * Intend C Command Line Interpreter main program
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <intend.h>

#define MODE_NORMAL 0x0
#define MODE_CHECK  0x1
#define MODE_DUMP   0x2

/*
 * Interpreter mode
 */
static int mode = MODE_NORMAL;

/*
 * First input file and position in command line
 */
static char *filename = NULL;
static int  filearg = 0;

/*
 * Execution context and script data
 */
intend_ctx context = NULL;

/*
 * Print usage message
 */
static void usage(char *name)
{
    printf("Usage: %s [options] script [script options]\n"
           "Options:\n"
           "\t-h, --help\t\t\tdisplay this help message\n"
           "\t-V, --version\t\t\tdisplay interpreter version and exit\n"
           "\t-c, --check\t\t\tsyntax check only\n"
           "\t-d, --dump\t\t\tparse script and dump parse tree\n"
           "\t-s, --safe\t\t\trun script in safe mode\n"
           "\t-p, --path-ro <name> <path>\tuse a read-only named <name> sandbox in <path> (multiple)\n"
           "\t-P, --path-rw <name> <path>\tuse a read-write named <name> sandbox in <path> (multiple)\n"
           "\t-l, --load-module <module>\tpre-load intend module <module> (multiple)\n"
           "\n"
           "The script is not executed when -c or -d are in effect.\n"
           "You can use the special option -- to terminate option\n"
           "processing; the next argument will then be used as the\n"
           "name of the script to execute.\n"
           , name);
}

/*
 * Evaluate command line options
 */
static void options(intend_ctx ctx, int argc, char **argv)
{
    int i, in_opts = 1;
    char *opt;

    for (i = 1; i < argc; i++) {
        opt = argv[i];
        if (in_opts) {
            if (strcmp(opt, "-d") == 0 || strcmp(opt, "--dump") == 0) {
                mode = MODE_DUMP;
                continue;
            } else if (strcmp(opt, "-c") == 0 || strcmp(opt, "--check") == 0) {
                mode = MODE_CHECK;
                continue;
            } else if (strcmp(opt, "-h") == 0 || strcmp(opt, "--help") == 0) {
                usage(argv[0]);
                exit(0);
            } else if (strcmp(opt, "-V") == 0 || strcmp(opt, "--version") == 0) {
                printf("%s\n", VERSION);
                exit(0);
            } else if (strcmp(opt, "-s") == 0 || strcmp(opt, "--safe") == 0) {
                intend_safe_mode_set(ctx, INTEND_SAFE_MODE_ON);
                continue;
            } else if (strcmp(opt, "-p") == 0 || strcmp(opt, "--path-ro") == 0) {
                if (argv[i + 1] && argv[i + 2]) {
                    intend_sandbox_add(ctx, argv[i + 1], argv[i + 2], INTEND_SANDBOX_RO);
                    i += 2;
                } else {
                    fprintf(stderr, "intend: -S,--sandbox need a name and a path as arguments\n");
                    exit(1);
                }
                continue;
            } else if (strcmp(opt, "-P") == 0 || strcmp(opt, "--path-rw") == 0) {
                if (argv[i + 1] && argv[i + 2]) {
                    intend_sandbox_add(ctx, argv[i + 1], argv[i + 2], INTEND_SANDBOX_RW);
                    i += 2;
                } else {
                    fprintf(stderr, "intend: -S,--sandbox need a name and a path as arguments\n");
                    exit(1);
                }
                continue;
            } else if (strcmp(opt, "-l") == 0 || strcmp(opt, "--load-module") == 0) {
                if (argv[i + 1]) {
                    intend_module_add_preload(ctx, argv[i + 1]);
                    i++;
                } else {
                    fprintf(stderr, "intend: -l,--load-module need a module name as argument\n");
                    exit(1);
                }
                continue;
            } else if (strcmp(opt, "--") == 0) {
                in_opts = 0;
                continue;
            } else if (*opt == '-') {
                fprintf(stderr, "intend: unknown option `%s'\n", opt);
                exit(1);
            }
        }
        if (!filename) {
            filename = argv[i];
            filearg  = i;
            break;
        }
    }
}

/*
 * Memory cleanup at interpreter exit
 */
static void teardown(void)
{
    if (context) intend_free_ctx(context);
}

/*
 * Main function
 *
 * This function will attempt to parse the first command line
 * argument as a script and will then execute that script if
 * it was parsed successfully.
 */
int main(int argc, char **argv)
{
    int status = 0;

    /* Initialize streams */
    setvbuf(stdin,  NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    /* Create  a new context */
    context = intend_new_ctx();

    /* Process command line options */
    options(context, argc, argv);
    atexit(teardown);

    /* Check if filename was passed */
    if (!filename) {
        fprintf(stderr, "intend: missing script name on command line\n");
        exit(1);
    }

    /* Initialize the context */
    intend_init_ctx(context, argc - filearg, argv + filearg);

    /* Parse the script or return error */
    if (!intend_parse_script_file(context, filename)) {
        return 1;
    }

    /* Execute script or dump it */
    if (mode == MODE_NORMAL) {
        status = intend_execute_script(context);
    } else if (mode == MODE_DUMP) {
        intend_dump_script(context);
    }

    return(status >= 0) ? status : 1;
}
