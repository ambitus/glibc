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

typedef void (*__bpx4lsn_t) (int32_t *socket_descriptor,
			     int32_t *backlog,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

int
listen (int fd, int backlog)
{
  int32_t retval, retcode, reason_code;
  BPX_CALL (listen, __bpx4lsn_t, &fd, &backlog,
	    &retval, &retcode, &reason_code);
  if (retval == -1)
    {
      switch (retcode)
	{
	case EINVAL:
	  retcode = ENOTSOCK;
	  break;
	}
      __set_errno (retcode);
    }

  return retval;
}
weak_alias (listen, __listen);
