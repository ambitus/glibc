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

#ifndef __GET_PATH_AT_FD_CANCEL_H
#define __GET_PATH_AT_FD_CANCEL_H

#include <sysdep-cancel.h>
#include <errno.h>
#include <zos-utils.h>
#include "get-path-at-fd.h"

/*
 * get_path_at_fd takes an fd and a buffer of at least length
 * __BPXK_PATH_MAX and places the path (if any) associated with fd at
 * the begining of buffer. A non-zero return value indicates an error,
 * either due to a too short buffer or an error in underlying w_ioctl
 * syscall.
 *
 * Regargless of the error condition the buffer is filled with '\0'.
 */
static int
get_path_at_fd_cancel (int fd, char *buffer, int buf_len)
{
  return GET_PATH_AT_FD_AS (_CANCEL, fd, buffer, buf_len);
}

#endif /* __GET_PATH_AT_FD_H */
