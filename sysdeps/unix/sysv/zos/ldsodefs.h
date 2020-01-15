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
#include <not-cancel.h>

#define DL_FIND_HEADER(fd) _dl_find_header (fd)
#define DL_FIND_HEADER_MMAPPED(contents, len)	\
  _dl_find_header_mmaped (contents, len)

/* Used by static binaries for initialization.  */
extern void _dl_psuedo_aux_init (void) attribute_hidden;

static inline size_t
__dl_zos_read_fully (int fd, void *buf, size_t len)
{
  do
    {
      ssize_t retlen = __read_nocancel (fd, (char *) buf + 8 - len, len);
      if (retlen <= 0)
	break;
      len -= retlen;
    }
  while (__glibc_unlikely (len > 0));

  return len;
}

/* Find the ehdr in FD, if present. Assumes that the current file offset
   is 0.  */

static inline ssize_t
_dl_find_header (int fd)
{
  /* Check for a Program Object eyecatcher. If it's present, we're
     reading an executable. If not assume it's a shared library. Shared
     libraries are assumed to have the usual ELF structure, so nothing
     else is required for them.  */
  uint64_t eyecatcher;
  uint32_t ehdr_offset;

  /* Check if the file is a Program Object.  */
  if (__dl_zos_read_fully (fd, &eyecatcher, 8) != 0
      || eyecatcher != 0xc9c5e6d7d3d4c840UL)
    {
      __lseek (fd, 0, SEEK_SET);
      return 0;
    }

  /* Jump to the part of the Program Object header that points to the
     contained code.  */
  __lseek (fd, 0x64, SEEK_SET);

  if (__dl_zos_read_fully (fd, &ehdr_offset, 4) != 0)
    return 0;

  ssize_t off = (ssize_t) (uint64_t) ehdr_offset;

  /* Reposition the file offset to point to the ehdr.  */
  __lseek (fd, off, SEEK_SET);

  return off;
}

/* Find the ehdr of the in-memory file starting at CONTENTS,
   if present.  */

static inline ssize_t
_dl_find_header_mmaped (void *contents, size_t length)
{
  /* Check for a Program Object eyecatcher. If it's present, we're
     reading an executable. If not assume it's a shared library. Shared
     libraries are assumed to have the usual ELF structure, so nothing
     else is required for them.  */
  uint32_t ehdr_offset;

  /* Check if the file is a Program Object.  */
  if (length < 0x68
      || *(uint64_t *) contents != 0xc9c5e6d7d3d4c840UL)
    return 0;

  ehdr_offset = *(uint32_t *) ((uintptr_t) contents + 0x64);

  if (ehdr_offset >= length)
    return 0;

  return (ssize_t) (uint64_t) ehdr_offset;
}

/* Get the real definitions.  */
#include_next <ldsodefs.h>

/* We don't have the auxiliary vector.  */
#undef HAVE_AUX_VECTOR

#undef HAVE_AUX_XID
#undef HAVE_AUX_SECURE
#undef HAVE_AUX_PAGESIZE

#endif /* _ZOS_LDSODEFS_H  */
