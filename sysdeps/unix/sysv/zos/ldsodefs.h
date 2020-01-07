/* Run-time dynamic linker data structures for loaded ELF shared objects.
   Copyright (C) 2019 Free Software Foundation, Inc.
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

#ifndef	_ZOS_LDSODEFS_H
#define	_ZOS_LDSODEFS_H 1

#include <features.h>

#include <stdbool.h>
#define __need_size_t
#define __need_NULL
#include <stddef.h>
#include <stdint.h>

#include <unistd.h>

#define DL_FIND_HEADER(fd, mode) _dl_find_header (fd, mode)

/* Used by static binaries for initialization.  */
extern void _dl_psuedo_aux_init (void)
  attribute_hidden;

/* Find the ehdr in FD, if present. MODE indicates in which capacity we
   are opening the file. Assumes that the current file offset is at the
   start of the file.  */

static inline ssize_t
_dl_find_header (int fd, int mode)
{
  /* Check for a Program Object eyecatcher. If it's present, we're
     reading an executable. If not assume it's a shared library. Shared
     libraries are assumed to have the usual ELF structure, so nothing
     else is required for them.  */
  size_t readlen = 8;
  char eyecatcher[8];
  do
    {
      ssize_t retlen =
	__read_nocancel (fd, (char *) eyecatcher + 8 - readlen, readlen);
      if (retlen <= 0)
	break;
      readlen -= retlen;
    }
  while (__glibc_unlikely (readlen > 0));

  if (readlen != 0)
    return 0;


}

/* Get the real definitions.  */
#include_next <ldsodefs.h>

/* We don't have the auxiliary vector.  */
#undef HAVE_AUX_VECTOR

#undef HAVE_AUX_XID
#undef HAVE_AUX_SECURE
#undef HAVE_AUX_PAGESIZE

#endif /* _ZOS_LDSODEFS_H  */
