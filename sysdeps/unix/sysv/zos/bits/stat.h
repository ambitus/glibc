/* Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#if !defined _SYS_STAT_H && !defined _FCNTL_H
# error "Never include <bits/stat.h> directly; use <sys/stat.h> instead."
#endif

#ifndef _BITS_STAT_H
#define _BITS_STAT_H	1

/* #include <bits/wordsize.h> */

# define _STAT_VER_KERNEL	0
# define _STAT_VER_LINUX	_STAT_VER_KERNEL
# define _STAT_VER		_STAT_VER_LINUX

#define _MKNOD_VER_LINUX	0

/* Tell code we have these members.  */
#define	_STATBUF_ST_BLKSIZE
#define _STATBUF_ST_RDEV

/* Nanosecond resolution time values are not supported.	 */
#ifdef _STATBUF_ST_NSEC
# undef _STATBUF_ST_NSEC
#endif /* _STATBUF_ST_NSEC */

/* Get struct stat and struct stat64, which are the same for us.  */
#define __bpxystat_struct_name stat
#include <bits/bpxystat.h>
#undef __bpxystat_struct_name
#define __bpxystat_struct_name stat64
#include <bits/bpxystat.h>
#undef __bpxystat_struct_name

/* TODO: define the __S_* constants */

#endif	/* bits/stat.h */
