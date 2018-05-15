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
 * Intend C Shell Tools module
 */

#include <stdio.h>
#include <stdlib.h>

#include <intend.h>

#include "intend_shell.h"

/*
 * System functions to register
 */
static intend_function_data shell_funcs[] = {
    { "shell",             shell_shell,             1,  "s",    '?' },
    { "grep",              shell_grep,              2,  "s?",   '?' },
    { "egrep",             shell_egrep,             2,  "s?",   '?' },
    { "awk",               shell_awk,               2,  "s?",   '?' },
    { "sed",               shell_sed,               2,  "s?",   '?' },
    { "uniq",              shell_uniq,              1,  "?",    '?' },

    /* list terminator */
    { NULL,		    NULL,			0,	NULL,	0	}
};

/*
 * Initialize the Shell Tools module
 */
void intend_shell_module_initialize(intend_ctx *ctx)
{
    // Register functions
    intend_register_function_array(ctx, shell_funcs);
}

/*
 * Destroy the Shell Tools module
 */
void intend_shell_module_destroy(intend_ctx *ctx)
{
    // Unregister functions
    intend_unregister_function_array(ctx, shell_funcs);
}

/*
 * Execute shell command
 *
 * Hands off the command given as the first argument to the system's command
 * interpreter, optionaly redirecting input from an Intend C string, and
 * output (stdout and/or stderr) to an Intend C string.
 * Returns the exit code of the command on success, or void on failure and errno
 * will be set to indicate the error..
 */
intend_value shell_shell(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value cmdline = argv[0];
    intend_value in = NULL;
    intend_value *out = NULL;
    intend_value *err = NULL;
    int res = 0;

    if (cmdline) {
        // Check what optional arguments we got
        switch (argc) {
            case 4:     // All IO redirect
                err = &(argv[3]);
            case 3:     // Only in and out redirect
                out = &(argv[2]);
            case 2:     // Only in redirect
                in = argv[1];
            default:
                break;
        }

        // Run the command
        res = intend_shell(ctx, cmdline, in, out, err);

        if (res != -1) {
            // Return the result
            return intend_create_value(INTEND_TYPE_INT, &res);
        }
    }

    // Return error
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Binding to grep shell tool
 */
intend_value shell_grep(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value expr = argv[0];
    intend_value input = argv[1];
    intend_value cmd = NULL;
	intend_value args = NULL;
	intend_value output = NULL;
    intend_value *out = NULL;
    intend_value *err = NULL;
    int res = 0;

	// Create command
	cmd = intend_create_value(INTEND_TYPE_STRING, "grep", NULL);

	// Create command arguments array
	args = intend_create_value(INTEND_TYPE_ARRAY, expr, NULL);

	// Check what optional arguments we got
	if (argc == 3) {
		err = &(argv[2]);
	}

	// Create dummy for output and assign it
	output = intend_create_value(INTEND_TYPE_VOID, NULL);
	out = &output;

	// Run the command
	res = intend_run(ctx, cmd, args, input, out, err);

	// Reload output
	output = *out;

	// Free all temporary stuff
	intend_free_value(cmd);
	intend_free_value(args);

	if (res != -1) {
		// Return the result
		return output;
	} else {
		intend_free_value(output);
	}

    // Return error
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Binding to grep shell tool with -E for regular expressions
 */
intend_value shell_egrep(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value expr = argv[0];
    intend_value input = argv[1];
    intend_value cmd = NULL;
	intend_value args = NULL;
	intend_value output = NULL;
    intend_value *out = NULL;
    intend_value *err = NULL;
	intend_value opt;
    int res = 0;

	// Create command
	cmd = intend_create_value(INTEND_TYPE_STRING, "grep", NULL);

	// Create command arguments array
	opt = intend_create_value(INTEND_TYPE_STRING, "-E" ,NULL);
	args = intend_create_value(INTEND_TYPE_ARRAY, opt, expr, NULL);
	intend_free_value(opt);

	// Check what optional arguments we got
	if (argc == 3) {
		err = &(argv[2]);
	}

	// Create dummy for output and assign it
	output = intend_create_value(INTEND_TYPE_VOID, NULL);
	out = &output;

	// Run the command
	res = intend_run(ctx, cmd, args, input, out, err);

	// Reload output
	output = *out;

	// Free all temporary stuff
	intend_free_value(cmd);
	intend_free_value(args);

	if (res != -1) {
		// Return the result
		return output;
	} else {
		intend_free_value(output);
	}

    // Return error
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Binding to awk shell tool
 */
intend_value shell_awk(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value prog = argv[0];
    intend_value input = argv[1];
    intend_value cmd = NULL;
	intend_value args = NULL;
	intend_value output = NULL;
    intend_value *out = NULL;
    intend_value *err = NULL;
    int res = 0;

	// Create command
	cmd = intend_create_value(INTEND_TYPE_STRING, "awk", NULL);

	// Create command arguments array
	args = intend_create_value(INTEND_TYPE_ARRAY, prog, NULL);

	// Check what optional arguments we got
	if (argc == 3) {
		err = &(argv[2]);
	}

	// Create dummy for output and assign it
	output = intend_create_value(INTEND_TYPE_VOID, NULL);
	out = &output;

	// Run the command
	res = intend_run(ctx, cmd, args, input, out, err);

	// Reload output
	output = *out;

	// Free all temporary stuff
	intend_free_value(cmd);
	intend_free_value(args);

	if (res != -1) {
		// Return the result
		return output;
	} else {
		intend_free_value(output);
	}

    // Return error
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Binding to sed shell tool
 */
intend_value shell_sed(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value expr = argv[0];
    intend_value input = argv[1];
    intend_value cmd = NULL;
	intend_value args = NULL;
	intend_value output = NULL;
    intend_value *out = NULL;
    intend_value *err = NULL;
    int res = 0;

	// Create command
	cmd = intend_create_value(INTEND_TYPE_STRING, "sed", NULL);

	// Create command arguments array
	args = intend_create_value(INTEND_TYPE_ARRAY, expr, NULL);

	// Check what optional arguments we got
	if (argc == 3) {
		err = &(argv[2]);
	}

	// Create dummy for output and assign it
	output = intend_create_value(INTEND_TYPE_VOID, NULL);
	out = &output;

	// Run the command
	res = intend_run(ctx, cmd, args, input, out, err);

	// Reload output
	output = *out;

	// Free all temporary stuff
	intend_free_value(cmd);
	intend_free_value(args);

	if (res != -1) {
		// Return the result
		return output;
	} else {
		intend_free_value(output);
	}

    // Return error
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Binding to uniq shell tool
 */
intend_value shell_uniq(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value input = argv[0];
    intend_value cmd = NULL;
	intend_value args = NULL;
    intend_value *out = NULL;
    intend_value *err = NULL;
    int res = 0;

	// Create command
	cmd = intend_create_value(INTEND_TYPE_STRING, "uniq", NULL);

	// Create command arguments array
	args = intend_create_value(INTEND_TYPE_ARRAY, NULL);

	// Check what optional arguments we got
	if (argc == 2) {
		err = &(argv[2]);
	}

	// Run the command
	res = intend_run(ctx, cmd, args, input, out, err);

	// Free all temporary stuff
	intend_free_value(cmd);
	intend_free_value(args);

	if (res == 0) {
		// Return the result
		return intend_copy_value(input);
	}

    // Return error
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

