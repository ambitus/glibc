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

#ifndef ZOS_FILE_ATTRS_H
#define ZOS_FILE_ATTRS_H

#include <fcntl.h>
#include <bits/types.h>

struct zos_file_attrs
{
  char		eyecatcher[4];
  unsigned short	version;
#define CHATTR_CURR_VER 3
  unsigned short	__res;
  unsigned int	set_flags;	/* Which fields to set.  */
#define CHATTR_SETTAG 0x00004000
  unsigned int	mode;
  unsigned int	uid;
  unsigned int	gid;
  unsigned char	__res2[3];
  unsigned char	flags2;
  unsigned char	__res3[3];
  unsigned char	flags3;
  unsigned long int	size;
  unsigned int	_bpx_atime32;
  unsigned int	_bpx_mtime32;
  unsigned int	auditoraudit;
  unsigned int	useraudit;
  unsigned int	_bpx_ctime32;
  unsigned int	_bpx_reftime32;
  unsigned char	format;
  unsigned char	__res4[3];
  struct zos_file_tag tag;
  unsigned char	__res5[8];
  __time_t		_bpx_atime64;
  __time_t		_bpx_mtime64;
  __time_t		_bpx_ctime64;
  __time_t		reftime;
  unsigned char	seclabel[8];
  unsigned char	__res6[8];
};

#endif
