/* Convert between the kernel's `struct stat' format, and libc's.
   Copyright (C) 1991-2018 Free Software Foundation, Inc.
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

#include <errno.h>
#include <sys/stat.h>
#include <kernel_stat.h>
#include <string.h>

int
__xstat_conv (int vers, struct kernel_stat *kbuf, void *ubuf)
{
  struct stat *buf = ubuf;

  switch (vers)
    {
    case _STAT_VER_LINUX:
      __builtin_memcpy (buf, kbuf, sizeof (struct stat));
      buf->st_atim.tv_sec = kbuf->_bpx_atime64;
      buf->st_atim.tv_nsec = 0;
      buf->st_mtim.tv_sec = kbuf->_bpx_mtime64;
      buf->st_mtim.tv_nsec = 0;
      buf->st_ctim.tv_sec = kbuf->_bpx_ctime64;
      buf->st_ctim.tv_nsec = 0;

    default:
      __set_errno (EINVAL);
      return -1;
    }
  return 0;
}
