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

#ifndef INTEND_MODULE_SYS_H
#define INTEND_MODULE_SYS_H

/*
 * Initialize the System module
 */
void intend_sys_module_initialize(intend_ctx *ctx);

/*
 * Destroy the System module
 */
void intend_sys_module_destroy(intend_ctx *ctx);

intend_value sys_getuid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_geteuid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_getgid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_getegid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_getgroups(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_group_member(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_setuid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_setgid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_sleep(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_fork(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_daemon(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_getpid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_getppid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_waitpid(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_signal(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_strsignal(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_psignal(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_raise(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_kill(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_alarm(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_pause(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_system(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value sys_exec(intend_ctx ctx, unsigned int argc, intend_value *argv);

#endif

