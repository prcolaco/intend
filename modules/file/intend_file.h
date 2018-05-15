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
 * Intend C File operations module, based on standard C facilities
 */

#ifndef INTEND_MODULE_FILE_H
#define INTEND_MODULE_FILE_H

void intend_file_module_initialize(intend_ctx ctx);
void intend_file_module_destroy(intend_ctx ctx);

intend_value file_is_resource(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_basename(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_dirname(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_realpath(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_getcwd(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_chdir(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_mkdir(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_rmdir(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_stat(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_access(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_eaccess(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_umask(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_chown(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_chmod(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_open(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_reopen(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_seek(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_tell(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_read(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_getc(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_gets(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_write(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_setbuf(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_flush(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_is_eof(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_is_error(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_clearerr(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_close(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_link(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_symlink(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_unlink(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_remove(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_rename(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_copy(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_scandir(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_scandirs(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_scanfiles(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_fileread(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_filewrite(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_filereadarray(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_filewritearray(intend_ctx ctx, unsigned int argc, intend_value *argv);
intend_value file_pipe(intend_ctx ctx, unsigned int argc, intend_value *argv);

#endif
