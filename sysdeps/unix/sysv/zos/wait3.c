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

#include <stdint.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sysdep-cancel.h>
#include <zos-wait.h>
#include <string.h>

typedef void (*__bpx4wte_t) (const int32_t *func_code,
			     const int32_t *idtype,
			     const int32_t *id,
			     int32_t **stat_loc_ptr,
			     const int32_t *options,
			     struct rusage **info_loc_ptr,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

#define WAIT3_CODE 1

/* Wait for a child to exit.  When one does, put its status in *STAT_LOC and
   return its process ID.  For errors return (pid_t) -1.  If USAGE is not nil,
   store information about the child's resource usage (as a `struct rusage')
   there.  If the WUNTRACED bit is set in OPTIONS, return status for stopped
   children; otherwise don't.  */
pid_t
__wait3 (int *stat_loc, int options, struct rusage *usage)
{
  int32_t retval, return_code, reason_code;
  const int32_t func = WAIT3_CODE;
  const int32_t dummy = 0;

  /* On other systems, WUNTRACED and WSTOPPED are the same flag.
     We can emulate the same behavior by setting WUNTRACED whenever we
     see WSTOPPED (which is not actually valid for waitid).  */
  if (options & WSTOPPED)
    {
      options |= WUNTRACED;
      options &= ~WSTOPPED;
    }

  if (usage != NULL)
    memset (usage, 0, sizeof (*usage));

  /* z/OS TODO: ABI: While this system's waitid also returns si_addr
     when the process was terminated by a SIGSEGV, SIGFPE, or SIGILL,
     the currently used siginfo structure can't represent si_addr at
     the same time as si_status, so we just discard it. We should
     consider changing the userspace siginfo_t structure.  */

  BPX_CALL_CANCEL (waitid_wait3, __bpx4wte_t, &func, &dummy, &dummy,
		   &stat_loc, &options, &usage, &retval, &return_code,
		   &reason_code);

  if (retval != -1)
    {
      usage->ru_utime.tv_usec &= 0x00000000FFFFFFFFllu;
      usage->ru_stime.tv_usec &= 0x00000000FFFFFFFFllu;
    }
  else
    __set_errno (return_code);

  return retval;
}
weak_alias (__wait3, wait3)
