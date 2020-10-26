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

#ifndef __OPENAT64_H
#define __OPENAT64_H

#include <fcntl.h>
#include <sysdep-cancel.h>
#include <zos-utils.h>
#include "get-path-at-fd.h"

/* Open FILE with access OFLAG.  Interpret relative paths relative to
   the directory associated with FD.  If OFLAG includes O_CREAT or
   O_TMPFILE, a fourth argument is the file protection.  */
#define DO_OPENAT64_AS(nocancel, nocancel_up, fd, file, oflag, mode)	\
({									\
  __label__ _ret;                                                       \
  int _fd = (fd);							\
  const char *_file = (file);						\
  int _oflag = (oflag);							\
  mode_t _mode = (mode);						\
  int ret;								\
									\
  if (_file == NULL)							\
    {									\
      __set_errno (EINVAL);						\
      ret = -1;								\
      goto _ret;							\
    }									\
									\
  if (_file[0] == '\0')							\
    {									\
      __set_errno (ENOENT);						\
      ret = -1;								\
      goto _ret;							\
    }									\
									\
  if (_fd == AT_FDCWD || *_file == '/')					\
    {									\
      ret = __open##nocancel (_file, _oflag, _mode);			\
      goto _ret;							\
    }									\
									\
  uint32_t file_len = strnlen (_file, __BPXK_PATH_MAX);			\
									\
  int retval;								\
  /* This buffer doens't need to be larger than this, if the path is */	\
  /* longer than PATH_MAX we exit with an error. */			\
  uint32_t buf_len = __BPXK_PATH_MAX;					\
  char buf[__BPXK_PATH_MAX];						\
									\
  retval = GET_PATH_AT_FD##nocancel_up (_fd, buf, buf_len);		\
									\
  if (retval != 0)							\
    {									\
      ret = -1;								\
      goto _ret;							\
    }									\
									\
  uint32_t path_len = strnlen (buf, buf_len);				\
									\
  if (file_len + 1 + path_len > __BPXK_PATH_MAX)			\
    {									\
      __set_errno (ENAMETOOLONG);					\
      ret = -1;								\
      goto _ret;							\
    }									\
									\
  buf[path_len] = '/';							\
									\
  for (int i = 0; _file[i] != '\0'; i++)				\
    buf[path_len + i + 1] = _file[i];					\
									\
  ret = __open##nocancel (buf, _oflag, _mode);				\
 _ret:									\
  ret;									\
})

#define DO_OPENAT64(fd, file, oflag, mode)	\
  DO_OPENAT64_AS (,, fd, file, oflag, mode)

#define DO_OPENAT64_NOCANCEL(fd, file, oflag, mode)		\
  DO_OPENAT64_AS(_nocancel, _NOCANCEL, fd, file, oflag, mode)

#endif /* __OPENAT64_H */
