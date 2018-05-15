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

#ifndef INTEND_MODULE_SHELL_H
#define INTEND_MODULE_SHELL_H

/*
 * Initialize the Shell Tools module
 */
void intend_shell_module_initialize(intend_ctx *ctx);

/*
 * Destroy the Shell Tools module
 */
void intend_shell_module_destroy(intend_ctx *ctx);

intend_value shell_shell(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value shell_grep(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value shell_egrep(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value shell_awk(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value shell_sed(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value shell_uniq(intend_ctx ctx, unsigned int argc, intend_value *argv);

#endif

