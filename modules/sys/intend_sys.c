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
 * Intend C System module
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

#include <intend.h>

#include "intend_sys.h"

/*
 * Global signal handler context
 */
static intend_ctx __sig_context = NULL;

/*
 * System variables to register
 */
static int sys_SIGHUP     = SIGHUP;
static int sys_SIGINT     = SIGINT;
static int sys_SIGQUIT    = SIGQUIT;
static int sys_SIGILL     = SIGILL;
static int sys_SIGTRAP    = SIGTRAP;
static int sys_SIGABRT    = SIGABRT;
static int sys_SIGIOT     = SIGIOT;
static int sys_SIGBUS     = SIGBUS;
static int sys_SIGFPE     = SIGFPE;
static int sys_SIGKILL    = SIGKILL;
static int sys_SIGUSR1    = SIGUSR1;
static int sys_SIGSEGV    = SIGSEGV;
static int sys_SIGUSR2    = SIGUSR2;
static int sys_SIGPIPE    = SIGPIPE;
static int sys_SIGALRM    = SIGALRM;
static int sys_SIGTERM    = SIGTERM;
static int sys_SIGSTKFLT  = SIGSTKFLT;
static int sys_SIGCLD     = SIGCLD;
static int sys_SIGCHLD    = SIGCHLD;
static int sys_SIGCONT    = SIGCONT;
static int sys_SIGSTOP    = SIGSTOP;
static int sys_SIGTSTP    = SIGTSTP;
static int sys_SIGTTIN    = SIGTTIN;
static int sys_SIGTTOU    = SIGTTOU;
static int sys_SIGURG     = SIGURG;
static int sys_SIGXCPU    = SIGXCPU;
static int sys_SIGXFSZ    = SIGXFSZ;
static int sys_SIGVTALRM  = SIGVTALRM;
static int sys_SIGPROF    = SIGPROF;
static int sys_SIGWINCH   = SIGWINCH;
static int sys_SIGPOLL    = SIGPOLL;
static int sys_SIGIO      = SIGIO;
static int sys_SIGPWR     = SIGPWR;
static int sys_SIGSYS     = SIGSYS;
static int sys_SIGUNUSED  = SIGUNUSED;
static int sys_SIGRTMIN   = 0;
static int sys_SIGRTMAX   = 0;
static int sys_SIG_DFL    = (int)SIG_DFL;
static int sys_SIG_IGN    = (int)SIG_IGN;

static intend_variable_data sys_vars[] = {
    { "SIGHUP",     INTEND_TYPE_INT,    &sys_SIGHUP         },
    { "SIGINT",     INTEND_TYPE_INT,    &sys_SIGINT         },
    { "SIGQUIT",    INTEND_TYPE_INT,    &sys_SIGQUIT        },
    { "SIGILL",     INTEND_TYPE_INT,    &sys_SIGILL         },
    { "SIGTRAP",    INTEND_TYPE_INT,    &sys_SIGTRAP        },
    { "SIGABRT",    INTEND_TYPE_INT,    &sys_SIGABRT        },
    { "SIGIOT",     INTEND_TYPE_INT,    &sys_SIGIOT         },
    { "SIGBUS",     INTEND_TYPE_INT,    &sys_SIGBUS         },
    { "SIGFPE",     INTEND_TYPE_INT,    &sys_SIGFPE         },
    { "SIGKILL",    INTEND_TYPE_INT,    &sys_SIGKILL        },
    { "SIGUSR1",    INTEND_TYPE_INT,    &sys_SIGUSR1        },
    { "SIGSEGV",    INTEND_TYPE_INT,    &sys_SIGSEGV        },
    { "SIGUSR2",    INTEND_TYPE_INT,    &sys_SIGUSR2        },
    { "SIGPIPE",    INTEND_TYPE_INT,    &sys_SIGPIPE        },
    { "SIGALRM",    INTEND_TYPE_INT,    &sys_SIGALRM        },
    { "SIGTERM",    INTEND_TYPE_INT,    &sys_SIGTERM        },
    { "SIGSTKFLT",  INTEND_TYPE_INT,    &sys_SIGSTKFLT      },
    { "SIGCLD",     INTEND_TYPE_INT,    &sys_SIGCLD         },
    { "SIGCHLD",    INTEND_TYPE_INT,    &sys_SIGCHLD        },
    { "SIGCONT",    INTEND_TYPE_INT,    &sys_SIGCONT        },
    { "SIGSTOP",    INTEND_TYPE_INT,    &sys_SIGSTOP        },
    { "SIGTSTP",    INTEND_TYPE_INT,    &sys_SIGTSTP        },
    { "SIGTTIN",    INTEND_TYPE_INT,    &sys_SIGTTIN        },
    { "SIGTTOU",    INTEND_TYPE_INT,    &sys_SIGTTOU        },
    { "SIGURG",     INTEND_TYPE_INT,    &sys_SIGURG         },
    { "SIGXCPU",    INTEND_TYPE_INT,    &sys_SIGXCPU        },
    { "SIGXFSZ",    INTEND_TYPE_INT,    &sys_SIGXFSZ        },
    { "SIGVTALRM",  INTEND_TYPE_INT,    &sys_SIGVTALRM      },
    { "SIGPROF",    INTEND_TYPE_INT,    &sys_SIGPROF        },
    { "SIGWINCH",   INTEND_TYPE_INT,    &sys_SIGWINCH       },
    { "SIGPOLL",    INTEND_TYPE_INT,    &sys_SIGPOLL        },
    { "SIGIO",      INTEND_TYPE_INT,    &sys_SIGIO          },
    { "SIGPWR",     INTEND_TYPE_INT,    &sys_SIGPWR         },
    { "SIGSYS",     INTEND_TYPE_INT,    &sys_SIGSYS         },
    { "SIGUNUSED",  INTEND_TYPE_INT,    &sys_SIGUNUSED      },
    { "SIGRTMIN",   INTEND_TYPE_INT,    &sys_SIGRTMIN       },
    { "SIGRTMAX",   INTEND_TYPE_INT,    &sys_SIGRTMAX       },
    { "SIG_DFL",    INTEND_TYPE_INT,    &sys_SIG_DFL        },
    { "SIG_IGN",    INTEND_TYPE_INT,    &sys_SIG_IGN        },

    /* list terminator */
    { NULL,         0,                  NULL                }
};

/*
 * System functions to register
 */
static intend_function_data sys_funcs[] = {
    { "getuid",           sys_getuid,           0,  "",     'i' },
    { "geteuid",          sys_geteuid,          0,  "",     'i' },
    { "getgid",           sys_getgid,           0,  "",     'i' },
    { "getegid",          sys_getegid,          0,  "",     'i' },
    { "getgroups",        sys_getgroups,        0,  "",     '?' },
    { "group_member",     sys_group_member,     0,  "i",    'b' },
    { "setuid",           sys_setuid,           0,  "i",    'b' },
    { "setgid",           sys_setgid,           0,  "i",    'b' },
    { "sleep",            sys_sleep,            0,  "i",    'i' },
    { "fork",             sys_fork,             0,  "",     'i' },
    { "daemon",           sys_daemon,           2,  "bb",   'b' },
    { "getpid",           sys_getpid,           0,  "",     'i' },
    { "getppid",          sys_getppid,          0,  "",     'i' },
    { "waitpid",          sys_waitpid,          1,  "i",    'i' },
    { "signal",           sys_signal,           1,  "?",    'b' },
    { "strsignal",        sys_strsignal,        1,  "i",    's' },
    { "psignal",          sys_psignal,          2,  "is",   'v' },
    { "raise",            sys_raise,            1,  "i",    'b' },
    { "kill",             sys_kill,             2,  "ii",   'b' },
    { "alarm",            sys_alarm,            1,  "i",    'i' },
    { "pause",            sys_pause,            0,  "",     'v' },
    { "system",           sys_system,           2,  "sa",    '?' },
    { "exec",             sys_exec,             2,  "sa",    'v' },

    /* list terminator */
    { NULL,		    NULL,			0,	NULL,	0	}
};

/*
 * Initialize the System module
 */
void intend_sys_module_initialize(intend_ctx *ctx)
{
    // Set globals
    sys_SIGRTMIN   = SIGRTMIN;
    sys_SIGRTMAX   = SIGRTMAX;

    // Register variables
    intend_register_variable_array(ctx, sys_vars);

    // Register functions
    intend_register_function_array(ctx, sys_funcs);
}

/*
 * Destroy the System module
 */
void intend_sys_module_destroy(intend_ctx *ctx)
{
    // Unregister functions
    intend_unregister_function_array(ctx, sys_funcs);

    // Unregister variables
    intend_unregister_variable_array(ctx, sys_vars);
}

/*
 * Returns the real user ID of the process
 */
intend_value sys_getuid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    uid_t uid;

    uid = getuid();

    return intend_create_value(INTEND_TYPE_INT, &uid);
}

/*
 * Returns the effective user ID of the process
 */
intend_value sys_geteuid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    uid_t uid;

    uid = geteuid();

    return intend_create_value(INTEND_TYPE_INT, &uid);
}

/*
 * Returns the real group ID of the process
 */
intend_value sys_getgid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    gid_t gid;

    gid = getgid();

    return intend_create_value(INTEND_TYPE_INT, &gid);
}

/*
 * Returns the effective group ID of the process
 */
intend_value sys_getegid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    gid_t gid;

    gid = getegid();

    return intend_create_value(INTEND_TYPE_INT, &gid);
}

/*
 * Returns an array of supplementary group IDs of the process
 */
intend_value sys_getgroups(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int size;
    gid_t *gids;
    int i;
    intend_value val, res;

    // Get the number of groups
    size = getgroups(0, NULL);

    // Alloc groups buffer
    gids = intend_oom(calloc(size, sizeof(gid_t)));

    // Get the groups
    size = getgroups(size, gids);

    // Check if we get the groups
    if (size == -1) {
        return intend_create_value(INTEND_TYPE_VOID, NULL);
    }

    // Create the result array
    res = intend_create_value(INTEND_TYPE_ARRAY, NULL);

    // Go through the gids list and add them to result array
    for (i = 0; i < size; i++) {
        val = intend_create_value(INTEND_TYPE_INT, &gids[i]);
        intend_array_add(res, val);
        intend_free_value(val);
    }

    // Return the result array
    return res;
}

/*
 * Checks if the calling process is member of group ID
 */
intend_value sys_group_member(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    gid_t gid = intend_int_value(argv[0]);
    int res;

    res = group_member(gid) != 0 ? 1 : 0;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Sets the user ID of the process
 *
 * Set the user ID of the calling process to UID.
 * If the calling process is the super-user, set the real
 * and effective user IDs, and the saved set-user-ID to UID;
 * if not, the effective user ID is set to UID.
 */
intend_value sys_setuid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    uid_t uid = intend_int_value(argv[0]);
    int res;

    res = setuid(uid) == 0 ? 1 : 0;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Sets the group ID of the process
 *
 * Set the group ID of the calling process to GID.
 * If the calling process is the super-user, set the real
 * and effective group IDs, and the saved set-group-ID to GID;
 * if not, the effective group ID is set to GID.
 */
intend_value sys_setgid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    gid_t gid = intend_int_value(argv[0]);
    int res;

    res = setgid(gid) == 0 ? 1 : 0;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Make the process sleep for SECONDS seconds, or until a signal arrives
 * and is not ignored.  The function returns the number of seconds less
 * than SECONDS which it actually slept (thus zero if it slept the full time).
 * There is no return value to indicate error, but if `sleep' returns SECONDS,
 * it probably didn't work.
 */
intend_value sys_sleep(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int seconds = intend_int_value(argv[0]);
    int res;

    res = sleep(seconds);

    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Clone the calling process, creating an exact copy.
 * Return -1 for errors, 0 to the new process,
 * and the process ID of the new process to the old process.
 */
intend_value sys_fork(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    pid_t res = -1;
    pid_t pid;
    char *func;
    intend_value rc;
    int ec;

    if (argc == 0) {
        res = fork();
    } else if (argc == 1) {
        if (pid = fork()) {     // This process
            // Just return the pid
            res = pid;
        } else {                // New process
            // Get the name of the function to call
            func = intend_fn_name_ptr(argv[0]);
            // Call the function
            rc = intend_call_function(ctx, func, 0);
            // Get exit code equal to return int value
            ec = intend_int_value(rc);
            // Free rc
            intend_free_value(rc);
            // Exit
            exit(ec);
        }
    }

    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Detaches the current process from the controlling terminal
 */
intend_value sys_daemon(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int nochdir = intend_bool_value(argv[0]);
    int noclose = intend_bool_value(argv[1]);
    int res = 0;

    res = (daemon(nochdir, noclose) == 0) ? 1 : 0;

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Get the process ID of the current process
 */
intend_value sys_getpid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    pid_t res;

    res = getpid();

    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Get the process ID of the parent of current process
 */
intend_value sys_getppid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    pid_t res;

    res = getppid();

    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Wait for the process ID to terminate, returning its exit status
 */
intend_value sys_waitpid(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    pid_t pid = intend_int_value(argv[0]);
    int status = 0;
    int res = -1;

    // While process ID not terminated
    do {
        sleep(1);
        waitpid(pid, &status, 0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    // Return the status
    if (WIFEXITED(status)) res = WEXITSTATUS(status);

    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * The intend signal handler function
 */
static void sys_signal_handler(int sig)
{
    intend_ctx ctx = __sig_context;
    intend_value val, hnd, res;
    char *sig_handler = NULL;

    // If the signal context is valid
    if (ctx) {
        // Get the intend signal handler
        hnd = intend_get_variable(ctx, "__sig_handler");
        // Check correct type
        if (hnd && intend_value_type(hnd) == INTEND_TYPE_FN) {
            // Get the handler function name
            sig_handler = intend_fn_name_ptr(hnd);
            // Do we have a name?
            if (sig_handler) {
                // Create an intend variable for sig argument
                val = intend_create_value(INTEND_TYPE_INT, &sig);
                // Call the handler function with the argument
                res = intend_call_function(ctx, sig_handler, 1, val);
                // Free argument and result
                intend_free_value(val);
                intend_free_value(res);
            }
        }
    }
}

/*
 * Sets all signal handlers to hnd
 */
static void set_signal_handlers(void *hnd)
{
    signal(SIGHUP, (__sighandler_t)hnd);
    signal(SIGINT, (__sighandler_t)hnd);
    signal(SIGQUIT, (__sighandler_t)hnd);
    signal(SIGILL, (__sighandler_t)hnd);
    signal(SIGTRAP, (__sighandler_t)hnd);
    signal(SIGABRT, (__sighandler_t)hnd);
    signal(SIGIOT, (__sighandler_t)hnd);
    signal(SIGBUS, (__sighandler_t)hnd);
    signal(SIGFPE, (__sighandler_t)hnd);
    signal(SIGUSR1, (__sighandler_t)hnd);
    signal(SIGSEGV, (__sighandler_t)hnd);
    signal(SIGUSR2, (__sighandler_t)hnd);
    signal(SIGPIPE, (__sighandler_t)hnd);
    signal(SIGALRM, (__sighandler_t)hnd);
    signal(SIGTERM, (__sighandler_t)hnd);
    signal(SIGSTKFLT, (__sighandler_t)hnd);
    signal(SIGCLD, (__sighandler_t)hnd);
    signal(SIGCHLD, (__sighandler_t)hnd);
    signal(SIGCONT, (__sighandler_t)hnd);
    signal(SIGTSTP, (__sighandler_t)hnd);
    signal(SIGTTIN, (__sighandler_t)hnd);
    signal(SIGTTOU, (__sighandler_t)hnd);
    signal(SIGURG, (__sighandler_t)hnd);
    signal(SIGXCPU, (__sighandler_t)hnd);
    signal(SIGXFSZ, (__sighandler_t)hnd);
    signal(SIGVTALRM, (__sighandler_t)hnd);
    signal(SIGPROF, (__sighandler_t)hnd);
    signal(SIGWINCH, (__sighandler_t)hnd);
    signal(SIGPOLL, (__sighandler_t)hnd);
    signal(SIGIO, (__sighandler_t)hnd);
    signal(SIGPWR, (__sighandler_t)hnd);
    signal(SIGSYS, (__sighandler_t)hnd);
    signal(SIGUNUSED, (__sighandler_t)hnd);
}

/*
 * Set the signal handler function
 */
intend_value sys_signal(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value val;
    int hnd;
    int res = 0;

    // Check if called with one fn argument
    if (intend_value_type(argv[0]) == INTEND_TYPE_FN) {
        // Set handler global
        intend_register_variable(ctx, "__sig_handler", INTEND_TYPE_FN, argv[0]);
        // Check if none registered before
        if (!__sig_context) {
            // Set the signal context
            __sig_context = ctx;
            // Set the handlers
            set_signal_handlers(sys_signal_handler);
            // Set result
            res = 1;
        }
    } else {
        // Get the default handler constant passed
        hnd = intend_int_value(argv[0]);
        // Validate default handlers
        if (hnd == (int)SIG_DFL || hnd == (int)SIG_IGN) {
            // Set the default handlers
            set_signal_handlers((__sighandler_t)hnd);
            // Clean the signal context
            __sig_context = NULL;
            // Clean the handler global
            intend_register_variable(ctx, "__sig_handler", INTEND_TYPE_VOID, NULL);
            // Set result
            res = 1;
        }
    }

    // Return result
    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Get signal string from sinal id
 */
intend_value sys_strsignal(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *str;

    str = strsignal(intend_int_value(argv[0]));
    return intend_create_value(INTEND_TYPE_STRING, str, NULL);
}

/*
 * Print signal message to stderr
 */
intend_value sys_psignal(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int sig = intend_int_value(argv[0]);
    char *prefix = NULL;
    FILE *err;
    char *msg;

    if (argc == 2) {
        prefix = intend_string_ptr(argv[1]);
    }
    err = intend_stream_get_stderr(ctx);
    msg = strsignal(sig);
    if (msg) {
        if (prefix) {
            fprintf(err, "%s: %s\n", prefix, msg);
        } else {
            fprintf(err, "%s\n", msg);
        }
    } else {
        if (prefix) {
            fprintf(err, "%s: undefined signal\n", prefix);
        } else {
            fprintf(err, "undefined signal\n", msg);
        }
    }

    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Raise signal to own process
 */
intend_value sys_raise(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int sig = intend_int_value(argv[0]);
    int res = 0;

    res = !raise(sig);

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Send a signal to a process
 */
intend_value sys_kill(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int pid = intend_int_value(argv[0]);
    int sig = intend_int_value(argv[1]);
    int res = 0;

    res = !kill(pid, sig);

    return intend_create_value(INTEND_TYPE_BOOL, &res);
}

/*
 * Schedule an alarm signal to current process
 */
intend_value sys_alarm(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    int sec = intend_int_value(argv[0]);
    int res = 0;

    res = alarm(sec);

    return intend_create_value(INTEND_TYPE_INT, &res);
}

/*
 * Pause the current process until a signal
 */
intend_value sys_pause(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    pause();

    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Execute system command in a new process, optionaly redirecting input from an
 * Intend C string, and output (stdout and/or stderr) to an Intend C string.
 * Returns the exit code of the command on success, or void on failure and errno
 * will be set to indicate the error.
 */
intend_value sys_system(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value cmd = argv[0];
    intend_value cmdargs = argv[1];
    intend_value in = NULL;
    intend_value *out = NULL;
    intend_value *err = NULL;
    int res = 0;

    if (cmd) {
        // Check what optional arguments we got
        switch (argc) {
            case 5:     // All IO redirect
                err = &(argv[4]);
            case 4:     // Only in and out redirect
                out = &(argv[3]);
            case 3:     // Only in redirect
                in = argv[2];
            default:
                break;
        }

        // Run the command
        res = intend_run(ctx, cmd, cmdargs, in, out, err);

        if (res != -1) {
            // Return the result
            return intend_create_value(INTEND_TYPE_INT, &res);
        }
    }

    // Return error
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Execute system command, substituting the current process with it. On success
 * this function never returns. On failure returns void and errno will be set to
 * indicate the error.
 */
intend_value sys_exec(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    intend_value cmd = argv[0];
    intend_value cmdargs = argv[1];

    // Run the command
    intend_exec(ctx, cmd, cmdargs);

    // This should never be reached, but you never know...
    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

