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
#include <stddef.h>
#include <dirent.h>
#include <stdio.h>		/* For BUFSIZ.  */

#include "opendirwithfd.h"
#include "get-path-at-fd.h"

typedef void (*__bpx4ioc_t) (const uint32_t * fd,
			     const uint32_t * cmd,
			     const uint32_t * arg_len,
			     char *arg,
			     int32_t * retval, int32_t * retcode,
			     int32_t * reason_code);

/* Open a directory stream on FD.  */
DIR *
__fdopendir (int fd)
{
  uint32_t buf_len = __BPXK_PATH_MAX + 1;
  char buf[buf_len];

  int retval = GET_PATH_AT_FD (fd, buf, buf_len);

  if (retval != 0)
    {
      return NULL;
    }

  return __opendir_with_fd (buf, fd);
}

weak_alias (__fdopendir, fdopendir)
