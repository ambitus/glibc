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

#include <errno.h>
#include <sys/stat.h>
#include <sysdep.h>
#include "get-path-at-fd-cancel.h"

/* Change the access time of FILE to TSP[0] and
   the modification time of FILE to TSP[1]. */
int
utimensat (int fd, const char *file, const struct timespec tsp[2], int flags)
{
  if (flags & ~AT_SYMLINK_NOFOLLOW)
    {
      __set_errno (EINVAL);
      return -1;
    }

  if (file == NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  struct zos_file_attrs attrs = { 0 };
  attrs.set_flags = _CHATTR_SETATIME | _CHATTR_SETMTIME;

  struct timeval tv[2];

  if (tsp != NULL)
    {
      tv[0].tv_sec = tsp[0].tv_sec;
      tv[0].tv_usec = tsp[0].tv_nsec / 1000;
      tv[1].tv_sec = tsp[1].tv_sec;
      tv[1].tv_usec = tsp[1].tv_nsec / 1000;
      attrs.atime = tsp[0].tv_sec;
      attrs.mtime = tsp[1].tv_sec;
    }

  if ((flags & AT_SYMLINK_NOFOLLOW)
      && (fd == AT_FDCWD || *file == '/'))
    {
      return __utimes(file, tv);
    }
  else if ((!(flags & AT_SYMLINK_NOFOLLOW))
	   && (fd == AT_FDCWD || *file == '/'))
    {
      return zos_lchattr (file, &attrs, 0);
    }

  size_t buf_len = __BPXK_PATH_MAX;
  char buf[__BPXK_PATH_MAX];

  if (get_path_at_fd_cancel (fd, buf, buf_len) == -1)
    {
      return -1;
    }

  size_t file_len = strnlen (file, __BPXK_PATH_MAX);
  size_t path_len = strnlen (buf, buf_len);

  if (file_len + 1 + path_len > __BPXK_PATH_MAX)
    {
      __set_errno (ENAMETOOLONG);
      return -1;
    }

  buf[path_len] = '/';

  for (size_t i = 0; file[i] != '\0'; i++)
    {
      buf[path_len + i + 1] = file[i];
    }

  if (flags & AT_SYMLINK_NOFOLLOW)
    {
      return zos_lchattr ((const char *) buf, &attrs, 0);
    }
  else /* !(flags & AT_SYMLINK_NOFOLLOW) */
    {
      return __utimes((const char *) buf, tv);
    }
}
