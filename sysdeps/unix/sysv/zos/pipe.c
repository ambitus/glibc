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

#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <sysdep.h>

typedef void (*__bpx4pip_t) (int32_t *read_fd, int32_t *write_fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Create a one-way communication channel (__pipe).
   If successful, two file descriptors are stored in PIPEDES;
   bytes written on PIPEDES[1] can be read from PIPEDES[0].
   Returns 0 if successful, -1 if not.  */
int
__pipe (int __pipedes[2])
{
  int32_t retval, retcode, reason_code;

  if (__pipedes == NULL)
    {
      __set_errno (EFAULT);
      return -1;
    }

  BPX_CALL (pipe, __bpx4pip_t, &__pipedes[0], &__pipedes[1], &retval,
	    &retcode, &reason_code);

  if (retval < 0)
    __set_errno (retcode);

  return retval;
}
libc_hidden_def (__pipe)
weak_alias (__pipe, pipe)
