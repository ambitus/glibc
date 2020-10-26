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

#include <dirent.h>
#include <errno.h>
#include <stdio.h>		/* For BUFSIZ.  */
#include <sysdep.h>

#include <not-cancel.h>

#include "opendirwithfd.h"

DIR *
__opendir (const char *name)
{
  return __opendir_with_fd (name, -1);
}
weak_alias (__opendir, opendir)

#if IS_IN (libc)
DIR *
__opendirat (int fd, const char *name)
{
  if (name == NULL)
    {
      __set_errno (EINVAL);
      return NULL;
    }

  if (name[0] == '\0')
    {
      __set_errno (ENOENT);
      return NULL;
    }

  if (fd == AT_FDCWD || *name == '/')
    return __opendir_with_fd (name, -1);

  uint32_t file_len = strnlen (name, __BPXK_PATH_MAX);

  int i;
  int32_t retval, reason_code;
  int errcode;
  const int32_t cmd = 17;	/* IOCC#GETPATHNAME */
  uint32_t buf_len = __BPXK_PATH_MAX;
  char buf[__BPXK_PATH_MAX + file_len + 2];

  for (i = 0; i < __BPXK_PATH_MAX + file_len + 2; i++)
    buf[i] = '\0';

  BPX_CALL (w_ioctl, __bpx4ioc_t, &fd, &cmd, &buf_len, buf,
	    &retval, &errcode, &reason_code);

  if (retval != 0)
    {
      __set_errno (errcode);
      return NULL;
    }

  uint32_t path_len =
    tr_a_until_chr_or_len_in_place (buf, '\0', __BPXK_PATH_MAX);

  if (file_len + 1 + path_len > __BPXK_PATH_MAX)
    {
      __set_errno (ENAMETOOLONG);
      return NULL;
    }

  buf[path_len] = '/';

  for (i = 0; name[i] != '\0'; i++)
    buf[path_len + i + 1] = name[i];

  return __opendir_with_fd (buf, -1);
}
#endif
