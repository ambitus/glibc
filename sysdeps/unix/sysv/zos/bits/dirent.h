/* Copyright (C) 2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _DIRENT_H
# error "Never use <bits/dirent.h> directly; include <dirent.h> instead."
#endif

struct dirent
  {
#ifndef __USE_FILE_OFFSET64
    __ino_t d_ino;
#else
    __ino64_t d_ino;
#endif
    unsigned short int d_reclen;
    unsigned char d_type;
    char d_name[1];
  };

#ifdef __USE_LARGEFILE64
struct dirent64
  {
    __ino64_t d_ino;
    unsigned short int d_reclen;
    unsigned char d_type;
    char d_name[1];
  };
#endif

#define d_fileno	d_ino	/* Backwards compatibility.  */

#undef _DIRENT_HAVE_D_NAMLEN
#undef _DIRENT_HAVE_D_OFF
#define _DIRENT_HAVE_D_RECLEN
#define _DIRENT_HAVE_D_TYPE

#ifdef __INO_T_MATCHES_INO64_T
/* Inform libc code that these two types are effectively identical.  */
# define _DIRENT_MATCHES_DIRENT64	1
#else
# define _DIRENT_MATCHES_DIRENT64	0
#endif
