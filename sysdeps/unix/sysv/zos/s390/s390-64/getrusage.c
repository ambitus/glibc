/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#include <sys/resource.h>
#include <errno.h>
#include <sysdep.h>

/* We need to move the syscall wrapper code into this file because of the
   problem with including "sys/resource.h" in "zos-syscall-impl.h". */

typedef void (*__bpx4gru_t) (const int32_t *who,
			     struct rusage *usage,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Return resource usage information on process indicated by WHO
   and put it in USAGE.  Returns 0 for success, -1 for failure.  */
int
__getrusage (enum __rusage_who who, struct rusage *usage)
{
  /* z/OS callable service returns very little information about
     resources that are used by the calling process or its child
     processes - only user and system time are available.

     z/OS TODO: Figure out for which additional resources we can obtain
     information according to the linux struct rusage fields.

     z/OS TODO: Figure out whether it is possible to add support for
     who=RUSAGE_THREAD. */

  int32_t retval, reason_code;
  int32_t who_no = who;
  INTERNAL_SYSCALL_DECL (retcode);

  BPX_CALL (getrusage, __bpx4gru_t, &who_no, usage, &retval,
	    &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    {
      __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));
      retval = -1;
    }

  /* High 4 bytes of tv_usec should be ignored (padding in BPXYRLIM
     macro) */
  usage->ru_utime.tv_usec &= 0x00000000FFFFFFFFlu;
  usage->ru_stime.tv_usec &= 0x00000000FFFFFFFFlu;

  return retval;
}
libc_hidden_def (__getrusage)
weak_alias (__getrusage, getrusage)
