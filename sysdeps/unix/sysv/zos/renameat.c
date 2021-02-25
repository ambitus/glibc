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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <fcntl.h>
#include <sysdep.h>
#include <errno.h>
#include "get-path-at-fd-cancel.h"

int
__renameat (int oldfd, const char *old, int newfd, const char *new)
{
  if (old == NULL || new == NULL)
    {
      __set_errno (EFAULT);
      return -1;
    }

  char oldbuf[__BPXK_PATH_MAX];
  char *oldbufp = oldbuf;

  if (oldfd == AT_FDCWD || *old == '/')
    {
      oldbufp = (char *)old;
    }
  else
    {
      if (get_path_at_fd_cancel (oldfd, oldbuf, __BPXK_PATH_MAX) == -1)
	{
	  return -1;
	}

      size_t old_len = strnlen (old, __BPXK_PATH_MAX);
      size_t old_fd_len = strnlen (oldbuf, __BPXK_PATH_MAX);

      if (old_len + old_fd_len + 1 > __BPXK_PATH_MAX)
	{
	  __set_errno (ENAMETOOLONG);
	  return -1;
	}

      oldbuf[old_fd_len] = '/';

      for (size_t i = 0; old[i] != '\0'; i++)
	{
	  oldbuf[old_fd_len + i + 1] = old[i];
	}
    }


  char newbuf[__BPXK_PATH_MAX];
  char *newbufp = newbuf;

  if (newfd == AT_FDCWD || *new == '/')
    {
      newbufp = (char *)new;
    }
  else
    {
      if (get_path_at_fd_cancel (newfd, newbuf, __BPXK_PATH_MAX) == -1)
	{
	  return -1;
	}

      size_t new_len = strnlen (new, __BPXK_PATH_MAX);
      size_t new_fd_len = strnlen (newbuf, __BPXK_PATH_MAX);

      if (new_len + new_fd_len + 1 > __BPXK_PATH_MAX)
	{
	  __set_errno (ENAMETOOLONG);
	  return -1;
	}

      newbuf[new_fd_len] = '/';

      for (size_t i = 0; new[i] != '\0'; i++)
	{
	  newbuf[new_fd_len + i + 1] = new[i];
	}
    }

  return INLINE_SYSCALL_CALL (rename, (const char *) oldbufp,
			      (const char *) newbufp);
}
libc_hidden_def (__renameat)
weak_alias (__renameat, renameat)
