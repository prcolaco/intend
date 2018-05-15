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
 * Intend C System commands in runtime
 */

// To avoid warnings on fmemopen function
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "runtime.h"

/*
 * Default Shell for system_shell
 */
char *__default_shell = "sh";

/*
 * Composes the command and arguments to pass to execvp
 */
static void system_compose_cmd_args(char *cmd, value *arr, char ***args)
{
    value *val;
    char *str;
    int len;
    int argcount = 0;
    int i, count;

    // Get the size of the array
    count = ARRLEN_OF(arr);

    // Alloc the array space
    *args = oom(calloc(count + 2, sizeof(char *)));

    // Add command at position 0
    (*args)[argcount] = cmd;
    argcount++;

    for (i = 0; i < count; i++) {
        val = value_get_array(arr, i);
        if (TYPE_OF(val) != VALUE_TYPE_STRING)
            value_cast_inplace(NULL, &val, VALUE_TYPE_STRING);
        len = STRLEN_OF(val) + 1;
        str = STR_OF(val);
        (*args)[argcount] = oom(malloc(len));
        memcpy((*args)[argcount], STR_OF(val), len);
        value_free(val);
        argcount++;
    }
    (*args)[argcount] = NULL;
}

/*
 * Composes the command and arguments to pass to execvp
 */
static void system_free_cmd_args(char **args)
{
    char *p;
    int i;

    // args[0] = cmd and not allocated
    for (i = 1; p = args[i]; i++)
        free(p);
    free(args);
}

/*
 * Execute a system command, redirecting IO if needed and returning the exit status
 */
int system_run(intend_state *s, value *cmd, value *cmdargs,
               value *in, value **out, value **err)
{
    char *exe = STR_OF(cmd);
    char **args = NULL;
    char *cin = NULL;
    int sin;
    int  pin[2];
    FILE *fout = NULL;
    int pout[2];
    char *cout;
    size_t sout;
    FILE *ferr = NULL;
    int perr[2];
    char *cerr;
    size_t serr;
    char buf[64];
    ssize_t readout = 0;
    ssize_t readerr = 0;
    pid_t pid = 0;
    int status;
    int res = -1;

    // If command present
    if (cmd) {
        // Compose command and arguments
        system_compose_cmd_args(exe, cmdargs, &args);

        // Verify and cast if input if needed
        if (in) {
            // Check in type and cast it if needed
            if (TYPE_OF(in) == VALUE_TYPE_VOID) {
                in = NULL;
            } else if (TYPE_OF(in) != VALUE_TYPE_STRING) {
                value_cast_inplace(NULL, &in, VALUE_TYPE_STRING);
            }
            if (in && (STRLEN_OF(in) == 0)) {
                in = NULL;
            }
        }

        // Create pipe for stdin and set content if needed
        if (in) {
            cin = STR_OF(in);
            sin = STRLEN_OF(in);
            pipe(pin);
        }

        // Create pipe for stdout if needed
        if (out) {
            pipe(pout);
        }

        // Create pipe for stderr if needed
        if (err) {
            pipe(perr);
        }

        // Fork a child process to execute the program and check for errors
        if ((pid = fork()) != -1) {

            // Child process
            if (pid == 0) {
                // Redirect stdin if needed
                if (in) {
                    close(pin[1]); // Close write end
                    dup2(pin[0], STDIN_FILENO);
                }

                // Redirect stdout if needed
                if (out) {
                    close(pout[0]); // Close read end
                    dup2(pout[1], STDOUT_FILENO);
                }

                // Redirect stderr if needed
                if (err) {
                    close(perr[0]); // Close read end
                    dup2(perr[1], STDERR_FILENO);
                }

                // Run the command
                res = execvp(exe, args);

                // Exit child process in case of error (this should never be reached)
                _exit(res);
            }

            // Main process
            if (pid > 0) {
                // Send input if needed
                if (in) {
                    close(pin[0]); // Close read end
                    write(pin[1], cin, sin);
                    close(pin[1]); // Close write end
                }

                // Open memstream for stdout if needed
                // Also, close output end of pipe and set control variable to 1
                if (out) {
                    fout = open_memstream(&cout, &sout);
                    close(pout[1]);
                    readout = 1;
                }

                // Open memstream stdout if needed
                // Also, close output end of pipe and set control variable to 1
                if (err) {
                    ferr = open_memstream(&cerr, &serr);
                    close(perr[1]);
                    readerr = 1;
                }

                // Read outputs while available
                while (readout || readerr) {
                    // Read stdout if needed
                    if (out && ((readout = read(pout[0], &buf, 64)) > 0)) {
                        fwrite(buf, 1, readout, fout);
                    }
                    // Read stderr if needed
                    if (err && ((readerr = read(perr[0], &buf, 64)) > 0)) {
                        fwrite(buf, 1, readerr, ferr);
                    }
                }

                // Close read output ends when finished
                if (out) close(pout[0]);
                if (err) close(perr[0]);

                // Wait for child
                while (1) {
                    if (waitpid(pid, &status, 0) != pid)
                        continue;
                    if (WIFEXITED(status))
                        break;
                    if (WIFSIGNALED(status) && WCOREDUMP(status))
                        break;
                }

                // Return result
                if (WIFEXITED(status)) {
                    res = WEXITSTATUS(status);
                }

                // Set the reference outputs
                if (out) {
                    fclose(fout);
                    value_free(*out);
                    *out = value_make_memstring(cout, sout);
                    free(cout);
                }
                if (err) {
                    fclose(ferr);
                    value_free(*err);
                    *err = value_make_memstring(cerr, serr);
                    free(cerr);
                }
            }
        } else {
            // Close the pipes
            if (in) {
                close(pin[0]);
                close(pin[1]);
            }
            if (out) {
                close(pout[0]);
                close(pout[1]);
            }
            if (err) {
                close(perr[0]);
                close(perr[1]);
            }
        }

        // Free up command  arguments
        system_free_cmd_args(args);
    }

    // Return result
    return res;
}

/*
 * Execute a system command in a shell, redirecting IO if needed and returning the exit status
 */
int system_shell(intend_state *s, value *cmdline,
               value *in, value **out, value **err)
{
	char *shell = NULL;
    char *line = STR_OF(cmdline);
    char *args[4];
    char *cin = NULL;
    int sin;
    int  pin[2];
    FILE *fout = NULL;
    int pout[2];
    char *cout;
    size_t sout;
    FILE *ferr = NULL;
    int perr[2];
    char *cerr;
    size_t serr;
    char buf[64];
    ssize_t readout = 0;
    ssize_t readerr = 0;
    pid_t pid = 0;
    int status;
    int res = -1;

    // If command present
    if (line) {
		// Get the shell
		shell = getenv("SHELL");
		if(!shell) shell = __default_shell;

        // Compose command and arguments
        args[0] = shell;
		args[1] = "-c";
		args[2] = line;
		args[3] = NULL;

        // Verify and cast if input if needed
        if (in) {
            // Check in type and cast it if needed
            if (TYPE_OF(in) == VALUE_TYPE_VOID) {
                in = NULL;
            } else if (TYPE_OF(in) != VALUE_TYPE_STRING) {
                value_cast_inplace(NULL, &in, VALUE_TYPE_STRING);
            }
            if (in && (STRLEN_OF(in) == 0)) {
                in = NULL;
            }
        }

        // Create pipe for stdin and set content if needed
        if (in) {
            cin = STR_OF(in);
            sin = STRLEN_OF(in);
            pipe(pin);
        }

        // Create pipe for stdout if needed
        if (out) {
            pipe(pout);
        }

        // Create pipe for stderr if needed
        if (err) {
            pipe(perr);
        }

        // Fork a child process to execute the program and check for errors
        if ((pid = fork()) != -1) {

            // Child process
            if (pid == 0) {
                // Redirect stdin if needed
                if (in) {
                    close(pin[1]); // Close write end
                    dup2(pin[0], STDIN_FILENO);
                }

                // Redirect stdout if needed
                if (out) {
                    close(pout[0]); // Close read end
                    dup2(pout[1], STDOUT_FILENO);
                }

                // Redirect stderr if needed
                if (err) {
                    close(perr[0]); // Close read end
                    dup2(perr[1], STDERR_FILENO);
                }

                // Run the command
                res = execvp(shell, args);

                // Exit child process in case of error (this should never be reached)
                _exit(res);
            }

            // Main process
            if (pid > 0) {
                // Send input if needed
                if (in) {
                    close(pin[0]); // Close read end
                    write(pin[1], cin, sin);
                    close(pin[1]); // Close write end
                }

                // Open memstream for stdout if needed
                // Also, close output end of pipe and set control variable to 1
                if (out) {
                    fout = open_memstream(&cout, &sout);
                    close(pout[1]);
                    readout = 1;
                }

                // Open memstream stdout if needed
                // Also, close output end of pipe and set control variable to 1
                if (err) {
                    ferr = open_memstream(&cerr, &serr);
                    close(perr[1]);
                    readerr = 1;
                }

                // Read outputs while available
                while (readout || readerr) {
                    // Read stdout if needed
                    if (out && ((readout = read(pout[0], &buf, 64)) > 0)) {
                        fwrite(buf, 1, readout, fout);
                    }
                    // Read stderr if needed
                    if (err && ((readerr = read(perr[0], &buf, 64)) > 0)) {
                        fwrite(buf, 1, readerr, ferr);
                    }
                }

                // Close read output ends when finished
                if (out) close(pout[0]);
                if (err) close(perr[0]);

                // Wait for child
                while (1) {
                    if (waitpid(pid, &status, 0) != pid)
                        continue;
                    if (WIFEXITED(status))
                        break;
                    if (WIFSIGNALED(status) && WCOREDUMP(status))
                        break;
                }

                // Return result
                if (WIFEXITED(status)) {
                    res = WEXITSTATUS(status);
                }

                // Set the reference outputs
                if (out) {
                    fclose(fout);
                    value_free(*out);
                    *out = value_make_memstring(cout, sout);
                    free(cout);
                }
                if (err) {
                    fclose(ferr);
                    value_free(*err);
                    *err = value_make_memstring(cerr, serr);
                    free(cerr);
                }
            }
        } else {
            // Close the pipes
            if (in) {
                close(pin[0]);
                close(pin[1]);
            }
            if (out) {
                close(pout[0]);
                close(pout[1]);
            }
            if (err) {
                close(perr[0]);
                close(perr[1]);
            }
        }
    }

    // Return result
    return res;
}

/*
 * Execute a system command, substituting the current process with it
 */
int system_exec(intend_state *s, value *cmd, value *cmdargs)
{
    char *exe = STR_OF(cmd);
    char **args = NULL;
    int res;

    // If command present
    if (cmd) {
        // Compose command and arguments
        system_compose_cmd_args(exe, cmdargs, &args);

        // Run the command
        res = execvp(exe, args);

        // Should exec fail, free up command  arguments
        system_free_cmd_args(args);
    }

    // Return error (this should never be reached)
    return res;
}
