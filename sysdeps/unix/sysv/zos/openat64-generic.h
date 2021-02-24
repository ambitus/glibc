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

#ifndef __OPENAT64_GENERIC_H
#define __OPENAT64_GENERIC_H

#include <stdio.h>
#include <fcntl.h>
#include <sysdep-cancel.h>
#include <zos-utils.h>
#include <unistd.h>

/* Open FILE with access OFLAG.  Interpret relative paths relative to
   the directory associated with FD.  If OFLAG includes O_CREAT or
   O_TMPFILE, a fourth argument is the file protection.
   Headers wishing to use this macro must include any functions they
   implictly rely on when they instantiate this macro.
*/
#define DO_OPENAT64_AS(nocancel, get_fd_cancel, fd, file, oflag, mode)	\
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
  /* This buffer doens't need to be larger than this, if the path is */	\
  /* longer than PATH_MAX we exit with an error. */			\
  uint32_t buf_len = __BPXK_PATH_MAX;					\
  char buf[__BPXK_PATH_MAX];						\
									\
  ret = get_path_at_fd_##get_fd_cancel (_fd, buf, buf_len);		\
  									\
  if (ret == -1)							\
    {									\
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

#endif /* __OPENAT64_GENERIC_H */
