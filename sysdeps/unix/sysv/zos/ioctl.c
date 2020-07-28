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
#include <sys/types.h>
#include <sys/user.h>
#include <stdarg.h>
#include <stdint.h>
#include <sysdep.h>
#include <sys/ioctl.h>

typedef void (*__bpx4ioc_t) (const uint32_t *fd,
			     const uint32_t *cmd,
			     const uint32_t *arg_len,
			     char *arg,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

int
__ioctl(int fd, unsigned long int request, ...)
{
  int32_t retval, reason_code;
  INTERNAL_SYSCALL_DECL (retcode);
  int32_t fileds = fd;
  const int32_t cmd = request;
  int32_t data_len;
  void *data;
  va_list ap;
  if (request == TIOCGWINSZ || request == TIOCSWINSZ) {
    data_len = 8;
    va_start (ap, request);
    data = va_arg (ap, void *);
    va_end (ap);
  } else {
    __set_errno (ENOSYS);
    return -1;
  }

  BPX_CALL (w_ioctl, __bpx4ioc_t, &fileds, &cmd, &data_len, data,
	    &retval, &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    {
      __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));
      retval = -1;
    }

  return retval;
}

libc_hidden_def (__ioctl)
weak_alias (__ioctl, ioctl)
