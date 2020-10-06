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

#include <sys/socket.h>
#include <sysdep.h>

typedef void (*__bpx4con_t) (int32_t *socket_descriptor,
			     int32_t *sockaddr_length,
			     char *sockaddr,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

int
__libc_connect (int fd, __CONST_SOCKADDR_ARG addr, socklen_t len)
{
  int32_t retval, retcode, reason_code;
  BPX_CALL (connect, __bpx4con_t, &fd, &len,
	    addr.__sockaddr__, &retval, &retcode,
	    &reason_code);

  if (retval == -1)
    {
      switch (retcode)
	{
	case EIO:
	  /* EIO is a low level network or transport failure,
	     here we lie that it is a software abort,
	     there may be better lie to tell. */
	  retcode = ECONNABORTED;
	  break;
	case EINVAL:
	  /* z/OS TODO: is this acceptable? */
	  retcode = EPROTOTYPE;
	  break;
	case ENOBUFS:
	  retcode = EAGAIN;
	  break;
	case EOPNOTSUPP:
	  /* z/OS TODO: is this acceptable? */
	  retcode = ECONNREFUSED;
	  break;
	case EWOULDBLOCK:
	  retcode = EAGAIN;
	  break;
	}
      __set_errno (retcode);
    }

  return retval;
}
weak_alias (__libc_connect, connect)
weak_alias (__libc_connect, __connect)
libc_hidden_weak (__connect)
