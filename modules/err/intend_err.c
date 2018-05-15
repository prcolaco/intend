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
 * Intend C Error Handling module
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <intend.h>

#include "intend_err.h"

/*
 * System variables to register
 */
int err_E2BIG           = E2BIG;
int err_EACCES          = EACCES;
int err_EADDRINUSE      = EADDRINUSE;
int err_EADDRNOTAVAIL   = EADDRNOTAVAIL;
int err_EAFNOSUPPORT    = EAFNOSUPPORT;
int err_EAGAIN          = EAGAIN;
int err_EALREADY        = EALREADY;
int err_EBADE           = EBADE;
int err_EBADF           = EBADF;
int err_EBADFD          = EBADFD;
int err_EBADMSG         = EBADMSG;
int err_EBADR           = EBADR;
int err_EBADRQC         = EBADRQC;
int err_EBADSLT         = EBADSLT;
int err_EBUSY           = EBUSY;
int err_ECANCELED       = ECANCELED;
int err_ECHILD          = ECHILD;
int err_ECHRNG          = ECHRNG;
int err_ECOMM           = ECOMM;
int err_ECONNABORTED    = ECONNABORTED;
int err_ECONNREFUSED    = ECONNREFUSED;
int err_ECONNRESET      = ECONNRESET;
int err_EDEADLK         = EDEADLK;
int err_EDEADLOCK       = EDEADLOCK;
int err_EDESTADDRREQ    = EDESTADDRREQ;
int err_EDOM            = EDOM;
int err_EDQUOT          = EDQUOT;
int err_EEXIST          = EEXIST;
int err_EFAULT          = EFAULT;
int err_EFBIG           = EFBIG;
int err_EHOSTDOWN       = EHOSTDOWN;
int err_EHOSTUNREACH    = EHOSTUNREACH;
int err_EIDRM           = EIDRM;
int err_EILSEQ          = EILSEQ;
int err_EINPROGRESS     = EINPROGRESS;
int err_EINTR           = EINTR;
int err_EINVAL          = EINVAL;
int err_EIO             = EIO;
int err_EISCONN         = EISCONN;
int err_EISDIR          = EISDIR;
int err_EISNAM          = EISNAM;
int err_EKEYEXPIRED     = EKEYEXPIRED;
int err_EKEYREJECTED    = EKEYREJECTED;
int err_EKEYREVOKED     = EKEYREVOKED;
int err_EL2HLT          = EL2HLT;
int err_EL2NSYNC        = EL2NSYNC;
int err_EL3HLT          = EL3HLT;
int err_EL3RST          = EL3RST;
int err_ELIBACC         = ELIBACC;
int err_ELIBBAD         = ELIBBAD;
int err_ELIBMAX         = ELIBMAX;
int err_ELIBSCN         = ELIBSCN;
int err_ELIBEXEC        = ELIBEXEC;
int err_ELOOP           = ELOOP;
int err_EMEDIUMTYPE     = EMEDIUMTYPE;
int err_EMFILE          = EMFILE;
int err_EMLINK          = EMLINK;
int err_EMSGSIZE        = EMSGSIZE;
int err_EMULTIHOP       = EMULTIHOP;
int err_ENAMETOOLONG    = ENAMETOOLONG;
int err_ENETDOWN        = ENETDOWN;
int err_ENETRESET       = ENETRESET;
int err_ENETUNREACH     = ENETUNREACH;
int err_ENFILE          = ENFILE;
int err_ENOBUFS         = ENOBUFS;
int err_ENODATA         = ENODATA;
int err_ENODEV          = ENODEV;
int err_ENOENT          = ENOENT;
int err_ENOEXEC         = ENOEXEC;
int err_ENOKEY          = ENOKEY;
int err_ENOLCK          = ENOLCK;
int err_ENOLINK         = ENOLINK;
int err_ENOMEDIUM       = ENOMEDIUM;
int err_ENOMEM          = ENOMEM;
int err_ENOMSG          = ENOMSG;
int err_ENONET          = ENONET;
int err_ENOPKG          = ENOPKG;
int err_ENOPROTOOPT     = ENOPROTOOPT;
int err_ENOSPC          = ENOSPC;
int err_ENOSR           = ENOSR;
int err_ENOSTR          = ENOSTR;
int err_ENOSYS          = ENOSYS;
int err_ENOTBLK         = ENOTBLK;
int err_ENOTCONN        = ENOTCONN;
int err_ENOTDIR         = ENOTDIR;
int err_ENOTEMPTY       = ENOTEMPTY;
int err_ENOTSOCK        = ENOTSOCK;
int err_ENOTSUP         = ENOTSUP;
int err_ENOTTY          = ENOTTY;
int err_ENOTUNIQ        = ENOTUNIQ;
int err_ENXIO           = ENXIO;
int err_EOPNOTSUPP      = EOPNOTSUPP;
int err_EOVERFLOW       = EOVERFLOW;
int err_EPERM           = EPERM;
int err_EPFNOSUPPORT    = EPFNOSUPPORT;
int err_EPIPE           = EPIPE;
int err_EPROTO          = EPROTO;
int err_EPROTONOSUPPORT = EPROTONOSUPPORT;
int err_EPROTOTYPE      = EPROTOTYPE;
int err_ERANGE          = ERANGE;
int err_EREMCHG         = EREMCHG;
int err_EREMOTE         = EREMOTE;
int err_EREMOTEIO       = EREMOTEIO;
int err_ERESTART        = ERESTART;
int err_EROFS           = EROFS;
int err_ESHUTDOWN       = ESHUTDOWN;
int err_ESPIPE          = ESPIPE;
int err_ESOCKTNOSUPPORT = ESOCKTNOSUPPORT;
int err_ESRCH           = ESRCH;
int err_ESTALE          = ESTALE;
int err_ESTRPIPE        = ESTRPIPE;
int err_ETIME           = ETIME;
int err_ETIMEDOUT       = ETIMEDOUT;
int err_ETXTBSY         = ETXTBSY;
int err_EUCLEAN         = EUCLEAN;
int err_EUNATCH         = EUNATCH;
int err_EUSERS          = EUSERS;
int err_EWOULDBLOCK     = EWOULDBLOCK;
int err_EXDEV           = EXDEV;
int err_EXFULL          = EXFULL;

static intend_variable_data err_vars[] = {
    { "E2BIG",              INTEND_TYPE_INT,    &err_E2BIG              },
    { "EACCES",             INTEND_TYPE_INT,    &err_EACCES             },
    { "EADDRINUSE",         INTEND_TYPE_INT,    &err_EADDRINUSE         },
    { "EADDRNOTAVAIL",      INTEND_TYPE_INT,    &err_EADDRNOTAVAIL      },
    { "EAFNOSUPPORT",       INTEND_TYPE_INT,    &err_EAFNOSUPPORT       },
    { "EAGAIN",             INTEND_TYPE_INT,    &err_EAGAIN             },
    { "EALREADY",           INTEND_TYPE_INT,    &err_EALREADY           },
    { "EBADE",              INTEND_TYPE_INT,    &err_EBADE              },
    { "EBADF",              INTEND_TYPE_INT,    &err_EBADF              },
    { "EBADFD",             INTEND_TYPE_INT,    &err_EBADFD             },
    { "EBADMSG",            INTEND_TYPE_INT,    &err_EBADMSG            },
    { "EBADR",              INTEND_TYPE_INT,    &err_EBADR              },
    { "EBADRQC",            INTEND_TYPE_INT,    &err_EBADRQC            },
    { "EBADSLT",            INTEND_TYPE_INT,    &err_EBADSLT            },
    { "EBUSY",              INTEND_TYPE_INT,    &err_EBUSY              },
    { "ECANCELED",          INTEND_TYPE_INT,    &err_ECANCELED          },
    { "ECHILD",             INTEND_TYPE_INT,    &err_ECHILD             },
    { "ECHRNG",             INTEND_TYPE_INT,    &err_ECHRNG             },
    { "ECOMM",              INTEND_TYPE_INT,    &err_ECOMM              },
    { "ECONNABORTED",       INTEND_TYPE_INT,    &err_ECONNABORTED       },
    { "ECONNREFUSED",       INTEND_TYPE_INT,    &err_ECONNREFUSED       },
    { "ECONNRESET",         INTEND_TYPE_INT,    &err_ECONNRESET         },
    { "EDEADLK",            INTEND_TYPE_INT,    &err_EDEADLK            },
    { "EDEADLOCK",          INTEND_TYPE_INT,    &err_EDEADLOCK          },
    { "EDESTADDRREQ",       INTEND_TYPE_INT,    &err_EDESTADDRREQ       },
    { "EDOM",               INTEND_TYPE_INT,    &err_EDOM               },
    { "EDQUOT",             INTEND_TYPE_INT,    &err_EDQUOT             },
    { "EEXIST",             INTEND_TYPE_INT,    &err_EEXIST             },
    { "EFAULT",             INTEND_TYPE_INT,    &err_EFAULT             },
    { "EFBIG",              INTEND_TYPE_INT,    &err_EFBIG              },
    { "EHOSTDOWN",          INTEND_TYPE_INT,    &err_EHOSTDOWN          },
    { "EHOSTUNREACH",       INTEND_TYPE_INT,    &err_EHOSTUNREACH       },
    { "EIDRM",              INTEND_TYPE_INT,    &err_EIDRM              },
    { "EILSEQ",             INTEND_TYPE_INT,    &err_EILSEQ             },
    { "EINPROGRESS",        INTEND_TYPE_INT,    &err_EINPROGRESS        },
    { "EINTR",              INTEND_TYPE_INT,    &err_EINTR              },
    { "EINVAL",             INTEND_TYPE_INT,    &err_EINVAL             },
    { "EIO",                INTEND_TYPE_INT,    &err_EIO                },
    { "EISCONN",            INTEND_TYPE_INT,    &err_EISCONN            },
    { "EISDIR",             INTEND_TYPE_INT,    &err_EISDIR             },
    { "EISNAM",             INTEND_TYPE_INT,    &err_EISNAM             },
    { "EKEYEXPIRED",        INTEND_TYPE_INT,    &err_EKEYEXPIRED        },
    { "EKEYREJECTED",       INTEND_TYPE_INT,    &err_EKEYREJECTED       },
    { "EKEYREVOKED",        INTEND_TYPE_INT,    &err_EKEYREVOKED        },
    { "EL2HLT",             INTEND_TYPE_INT,    &err_EL2HLT             },
    { "EL2NSYNC",           INTEND_TYPE_INT,    &err_EL2NSYNC           },
    { "EL3HLT",             INTEND_TYPE_INT,    &err_EL3HLT             },
    { "EL3RST",             INTEND_TYPE_INT,    &err_EL3RST             },
    { "ELIBACC",            INTEND_TYPE_INT,    &err_ELIBACC            },
    { "ELIBBAD",            INTEND_TYPE_INT,    &err_ELIBBAD            },
    { "ELIBMAX",            INTEND_TYPE_INT,    &err_ELIBMAX            },
    { "ELIBSCN",            INTEND_TYPE_INT,    &err_ELIBSCN            },
    { "ELIBEXEC",           INTEND_TYPE_INT,    &err_ELIBEXEC           },
    { "ELOOP",              INTEND_TYPE_INT,    &err_ELOOP              },
    { "EMEDIUMTYPE",        INTEND_TYPE_INT,    &err_EMEDIUMTYPE        },
    { "EMFILE",             INTEND_TYPE_INT,    &err_EMFILE             },
    { "EMLINK",             INTEND_TYPE_INT,    &err_EMLINK             },
    { "EMSGSIZE",           INTEND_TYPE_INT,    &err_EMSGSIZE           },
    { "EMULTIHOP",          INTEND_TYPE_INT,    &err_EMULTIHOP          },
    { "ENAMETOOLONG",       INTEND_TYPE_INT,    &err_ENAMETOOLONG       },
    { "ENETDOWN",           INTEND_TYPE_INT,    &err_ENETDOWN           },
    { "ENETRESET",          INTEND_TYPE_INT,    &err_ENETRESET          },
    { "ENETUNREACH",        INTEND_TYPE_INT,    &err_ENETUNREACH        },
    { "ENFILE",             INTEND_TYPE_INT,    &err_ENFILE             },
    { "ENOBUFS",            INTEND_TYPE_INT,    &err_ENOBUFS            },
    { "ENODATA",            INTEND_TYPE_INT,    &err_ENODATA            },
    { "ENODEV",             INTEND_TYPE_INT,    &err_ENODEV             },
    { "ENOENT",             INTEND_TYPE_INT,    &err_ENOENT             },
    { "ENOEXEC",            INTEND_TYPE_INT,    &err_ENOEXEC            },
    { "ENOKEY",             INTEND_TYPE_INT,    &err_ENOKEY             },
    { "ENOLCK",             INTEND_TYPE_INT,    &err_ENOLCK             },
    { "ENOLINK",            INTEND_TYPE_INT,    &err_ENOLINK            },
    { "ENOMEDIUM",          INTEND_TYPE_INT,    &err_ENOMEDIUM          },
    { "ENOMEM",             INTEND_TYPE_INT,    &err_ENOMEM             },
    { "ENOMSG",             INTEND_TYPE_INT,    &err_ENOMSG             },
    { "ENONET",             INTEND_TYPE_INT,    &err_ENONET             },
    { "ENOPKG",             INTEND_TYPE_INT,    &err_ENOPKG             },
    { "ENOPROTOOPT",        INTEND_TYPE_INT,    &err_ENOPROTOOPT        },
    { "ENOSPC",             INTEND_TYPE_INT,    &err_ENOSPC             },
    { "ENOSR",              INTEND_TYPE_INT,    &err_ENOSR              },
    { "ENOSTR",             INTEND_TYPE_INT,    &err_ENOSTR             },
    { "ENOSYS",             INTEND_TYPE_INT,    &err_ENOSYS             },
    { "ENOTBLK",            INTEND_TYPE_INT,    &err_ENOTBLK            },
    { "ENOTCONN",           INTEND_TYPE_INT,    &err_ENOTCONN           },
    { "ENOTDIR",            INTEND_TYPE_INT,    &err_ENOTDIR            },
    { "ENOTEMPTY",          INTEND_TYPE_INT,    &err_ENOTEMPTY          },
    { "ENOTSOCK",           INTEND_TYPE_INT,    &err_ENOTSOCK           },
    { "ENOTSUP",            INTEND_TYPE_INT,    &err_ENOTSUP            },
    { "ENOTTY",             INTEND_TYPE_INT,    &err_ENOTTY             },
    { "ENOTUNIQ",           INTEND_TYPE_INT,    &err_ENOTUNIQ           },
    { "ENXIO",              INTEND_TYPE_INT,    &err_ENXIO              },
    { "EOPNOTSUPP",         INTEND_TYPE_INT,    &err_EOPNOTSUPP         },
    { "EOVERFLOW",          INTEND_TYPE_INT,    &err_EOVERFLOW          },
    { "EPERM",              INTEND_TYPE_INT,    &err_EPERM              },
    { "EPFNOSUPPORT",       INTEND_TYPE_INT,    &err_EPFNOSUPPORT       },
    { "EPIPE",              INTEND_TYPE_INT,    &err_EPIPE              },
    { "EPROTO",             INTEND_TYPE_INT,    &err_EPROTO             },
    { "EPROTONOSUPPORT",    INTEND_TYPE_INT,    &err_EPROTONOSUPPORT    },
    { "EPROTOTYPE",         INTEND_TYPE_INT,    &err_EPROTOTYPE         },
    { "ERANGE",             INTEND_TYPE_INT,    &err_ERANGE             },
    { "EREMCHG",            INTEND_TYPE_INT,    &err_EREMCHG            },
    { "EREMOTE",            INTEND_TYPE_INT,    &err_EREMOTE            },
    { "EREMOTEIO",          INTEND_TYPE_INT,    &err_EREMOTEIO          },
    { "ERESTART",           INTEND_TYPE_INT,    &err_ERESTART           },
    { "EROFS",              INTEND_TYPE_INT,    &err_EROFS              },
    { "ESHUTDOWN",          INTEND_TYPE_INT,    &err_ESHUTDOWN          },
    { "ESPIPE",             INTEND_TYPE_INT,    &err_ESPIPE             },
    { "ESOCKTNOSUPPORT",    INTEND_TYPE_INT,    &err_ESOCKTNOSUPPORT    },
    { "ESRCH",              INTEND_TYPE_INT,    &err_ESRCH              },
    { "ESTALE",             INTEND_TYPE_INT,    &err_ESTALE             },
    { "ESTRPIPE",           INTEND_TYPE_INT,    &err_ESTRPIPE           },
    { "ETIME",              INTEND_TYPE_INT,    &err_ETIME              },
    { "ETIMEDOUT",          INTEND_TYPE_INT,    &err_ETIMEDOUT          },
    { "ETXTBSY",            INTEND_TYPE_INT,    &err_ETXTBSY            },
    { "EUCLEAN",            INTEND_TYPE_INT,    &err_EUCLEAN            },
    { "EUNATCH",            INTEND_TYPE_INT,    &err_EUNATCH            },
    { "EUSERS",             INTEND_TYPE_INT,    &err_EUSERS             },
    { "EWOULDBLOCK",        INTEND_TYPE_INT,    &err_EWOULDBLOCK        },
    { "EXDEV",              INTEND_TYPE_INT,    &err_EXDEV              },
    { "EXFULL",             INTEND_TYPE_INT,    &err_EXFULL             },

    /* list terminator */
    { NULL,                 0,                  NULL                    }
};

/*
 * System functions to register
 */
static intend_function_data err_funcs[] = {
    { "errno",            err_errno,            0,  "",     'i' },
    { "strerror",         err_strerror,         1,  "i",    's' },
    { "perror",           err_perror,           0,  "",     'v' },
    { "printerr",         err_printerr,         0,  "",     'v' },

    /* list terminator */
    { NULL,		    NULL,			0,	NULL,	0	}
};

/*
 * Initialize the System module
 */
void intend_err_module_initialize(intend_ctx *ctx)
{
    // Register variables
    intend_register_variable_array(ctx, err_vars);

    // Register functions
    intend_register_function_array(ctx, err_funcs);
}

/*
 * Destroy the System module
 */
void intend_err_module_destroy(intend_ctx *ctx)
{
    // Unregister functions
    intend_unregister_function_array(ctx, err_funcs);

    // Unregister variables
    intend_unregister_variable_array(ctx, err_vars);
}

/*
 * Get error number of last function call
 */
intend_value err_errno(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    return intend_create_value(INTEND_TYPE_INT, &errno);
}

/*
 * Get error message for error number
 */
intend_value err_strerror(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char err[512];

    strerror_r(intend_int_value(argv[0]), err, 512);
    return intend_create_value(INTEND_TYPE_STRING, err, NULL);
}

/*
 * Print errno error message to stderr
 */
intend_value err_perror(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    char *prefix = NULL;
    FILE *err;
    char msg[512];

    if (argc == 1) {
        prefix = intend_string_ptr(argv[0]);
    }
    err = intend_stream_get_stderr(ctx);
    strerror_r(errno, msg, 512);
    if (msg) {
        if (prefix) {
            fprintf(err, "%s: %s\n", prefix, msg);
        } else {
            fprintf(err, "%s\n", msg);
        }
    } else {
        if (prefix) {
            fprintf(err, "%s: unknown error\n", prefix);
        } else {
            fprintf(err, "unknown error\n", msg);
        }
    }

    return intend_create_value(INTEND_TYPE_VOID, NULL);
}

/*
 * Print error message to stderr
 */
intend_value err_printerr(intend_ctx ctx, unsigned int argc, intend_value *argv)
{
    FILE *err;
	int i;
	intend_value val;

    err = intend_stream_get_stderr(ctx);
	for(i=0; i<argc; i++) {
		val = intend_copy_value(argv[i]);
		intend_cast_value(&val, INTEND_TYPE_STRING);
		fprintf(err, "%s", intend_string_ptr(val));
		intend_free_value(val);
	}

	return intend_create_value(INTEND_TYPE_VOID, NULL);
}

