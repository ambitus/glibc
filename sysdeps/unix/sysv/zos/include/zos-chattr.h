/* Copyright (C) 2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Harrison Kaiser <harrisonmatthewkaiser@gmail.com>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _ZOS_CHATTR_H
#define _ZOS_CHATTR_H

#include <sys/stat.h>

extern int __zos_fchattr (int fd, struct zos_file_attrs *attrs,
			  unsigned long attrs_size) attribute_hidden;
extern int __zos_chattr (const char *path, struct zos_file_attrs *attrs,
			 unsigned long attrs_size) attribute_hidden;
extern int __zos_lchattr (const char *path, struct zos_file_attrs *attrs,
			 unsigned long attrs_size) attribute_hidden;

#endif
