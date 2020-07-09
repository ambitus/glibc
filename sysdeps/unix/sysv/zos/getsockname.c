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
#include <sys/socket.h>
#include <sysdep.h>

typedef void (*__bpx4gnm_t) (const int32_t *sockd,
                             const int32_t *operation,
                             int32_t *addrlen,
                             struct sockaddr *addr,
                             int32_t *retval, int32_t *retcode,
                             int32_t *reason_code);

int
__getsockname (int fd, __SOCKADDR_ARG addr, socklen_t *len)
{
  const int32_t operation = 2;  /* getsockname */
  int32_t retval, retcode, reason_code;

  BPX_CALL (getpeername, __bpx4gnm_t, &fd, &operation,
	    len, addr.__sockaddr__, &retval, &retcode, &reason_code);

  if (retval != 0)
    __set_errno (retcode);

  return retval;
}
weak_alias (__getsockname, getsockname)
