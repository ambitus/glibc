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
#include <sys/ptrace.h>
#include <sys/user.h>
#include <stdarg.h>
#include <stdint.h>
#include <signal.h>
#include <sysdep.h>

typedef void (*__bpx4ptr_t) (const int32_t *request,
			     const int32_t *pid,
			     void **address,
			     void **data,
			     void **buffer,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

long int
ptrace (enum __ptrace_request request, ...)
{
  va_list ap;
  pid_t pid;
  void *addr, *data, *buff, *first;
  int32_t intval, retval, retcode = 0, reason_code, req = request;

  va_start (ap, request);
  pid = va_arg (ap, pid_t);

  /* A few of the request types have different-sized first operands.  */
  switch (request)
    {
    case PT_READ_GPR:
    case PT_READ_GPRH:
    case PT_WRITE_GPR:
    case PT_WRITE_GPRH:
    case PT_EVENTS:
    case PT_READ_U:
      intval = va_arg (ap, int32_t);
      first = &intval;
      break;

    default:
      addr = va_arg (ap, void *);
      first = &addr;
      break;
    }

  data = va_arg (ap, void *);
  buff = va_arg (ap, void *);
  va_end (ap);

  BPX_CALL (ptrace, __bpx4ptr_t, &req, &pid, first, &data, &buff,
	    &retval, &retcode, &reason_code);

  if (retcode != 0)
    __set_errno (retcode);

  return retval;
}
