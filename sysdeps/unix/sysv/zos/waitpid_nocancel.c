/* z/OS waitpid syscall implementation -- non-cancellable.
   Copyright (C) 2020 Free Software Foundation, Inc.
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

#include <stdint.h>
#include <errno.h>
#include <sys/wait.h>
#include <not-cancel.h>
#include <sysdep.h>

typedef void (*__bpx4wat_t) (const int32_t *pid,
			     const int32_t *options,
			     int32_t * const *status,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

__pid_t
__waitpid_nocancel (__pid_t pid, int *stat_loc, int options)
{
  /* z/OS NOTE: keep the contents of this in line with
     __waitpid.  */
  int32_t retval, return_code, reason_code;
  int32_t prid = pid;

  /* On other systems, WUNTRACED and WSTOPPED are the same flag.
     We can emulate the same behavior by setting WUNTRACED whenever we
     see WSTOPPED (which is not actually valid for waitid).  */
  if (options & WSTOPPED)
    {
      options |= WUNTRACED;
      options &= ~WSTOPPED;
    }

  BPX_CALL (wait, __bpx4wat_t, &prid, &options, &stat_loc,
	    &retval, &return_code, &reason_code);

  if (retval == -1)
    __set_errno (return_code);

  return retval;
}
libc_hidden_def (__waitpid_nocancel)
