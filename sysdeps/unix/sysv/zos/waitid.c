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

#include <stddef.h>
#include <errno.h>
#include <sys/wait.h>
#include <sysdep-cancel.h>
#include <zos-wait.h>

typedef void (*__bpx4wte_t) (const int32_t *func_code,
			     const int32_t *idtype,
			     const int32_t *id,
			     int32_t ** stat_loc_ptr,
			     const int32_t *options,
			     siginfo_t **info_loc_ptr,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

#define WAITID_CODE 2

int
__waitid (idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
  int32_t retval, retcode, reason_code;
  const int32_t func = WAITID_CODE;
  void *stat_loc = NULL;
  int32_t k_idtype = idtype, k_id = id, k_options = options;
  kernel_siginfo_t k_info;
  kernel_siginfo_t *k_info_ptr = &k_info;

  /* On other systems, WUNTRACED and WSTOPPED are the same flag.
     We can emulate the same behavior by setting WSTOPPED whenever we
     see WUNTRACED (which is not actually valid for waitid).  */
  if (k_options & WUNTRACED)
    {
      k_options |= WSTOPPED;
      k_options &= ~WUNTRACED;
    }

  /* z/OS TODO: ABI: While this system's waitid also returns si_addr
     when the process was terminated by a SIGSEGV, SIGFPE, or SIGILL,
     the currently used siginfo structure can't represent si_addr at
     the same time as si_status, so we just discard it. We should
     consider changing the userspace siginfo_t structure.  */

  BPX_CALL_CANCEL (waitid_wait3, __bpx4wte_t, &func, &k_idtype, &k_id,
		   &stat_loc, &k_options, &k_info_ptr, &retval, &retcode,
		   &reason_code);

  if (retval != -1)
    {
      infop->si_signo = k_info.k_si_signo;
      infop->si_errno = k_info.k_si_errno;
      infop->si_code = k_info.k_si_code;
      infop->si_pid = k_info.k_si_pid;
      infop->si_uid = k_info.k_si_uid;
      infop->si_status = k_info.k_si_status;
    }
  else
    __set_errno (retcode);

  return retval;
}
weak_alias (__waitid, waitid)
strong_alias (__waitid, __libc_waitid)
