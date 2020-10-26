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

#ifndef __GET_PATH_AT_FD_H
#define __GET_PATH_AT_FD_H

#include <sysdep-cancel.h>
#include <errno.h>
#include <zos-utils.h>

/*
 * get_path_at_fd takes an fd and a buffer of at least length
 * __BPXK_PATH_MAX and places the path (if any) associated with fd at
 * the begining of buffer. A non-zero return value indicates an error,
 * either due to a too short buffer or an error in underlying w_ioctl
 * syscall.
 *
 * Regargless of the error condition the buffer is filled with '\0'.
 */
#define GET_PATH_AT_FD_AS(cancel, fd, buffer, buf_len)			\
({									\
  __label__ _ret_label;                                                 \
  int ret = 0;								\
  int _fd = fd;								\
  char *_buffer = buffer;						\
  int _buf_len = buf_len;						\
									\
  int i, errcode;							\
  int32_t retval, reason_code;						\
  const int32_t cmd = 17;	/* IOCC#GETPATHNAME */			\
									\
  for (i = 0; i < _buf_len; i++)					\
    {									\
      _buffer[i] = '\0';						\
    }									\
									\
  if (_buf_len < __BPXK_PATH_MAX)					\
    {									\
      __set_errno (EINVAL);						\
      ret = -1;								\
      goto _ret_label;							\
    }									\
									\
  BPX_CALL##cancel (w_ioctl, __bpx4ioc_t, &_fd, &cmd,			\
		    &_buf_len, _buffer, &retval, &errcode,		\
		    &reason_code);					\
									\
  if (retval != 0)							\
    {									\
      __set_errno (errcode);						\
      ret = -1;								\
      goto _ret_label;							\
    }									\
									\
  tr_a_until_len_in_place (_buffer, _buf_len);				\
									\
 _ret_label:								\
  ret;									\
 })

#define GET_PATH_AT_FD(fd, buffer, buf_len)		\
  GET_PATH_AT_FD_AS (_CANCEL, (fd), (buffer), (buf_len))

#define GET_PATH_AT_FD_NOCANCEL(fd, buffer, buf_len)	\
  GET_PATH_AT_FD_AS (, (fd), (buffer), (buf_len))

#endif /* __GET_PATH_AT_FD_H */
