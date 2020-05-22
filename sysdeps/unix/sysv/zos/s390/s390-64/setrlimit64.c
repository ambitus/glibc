/* z/OS setrlimit64 implementation (64 bits off_t).
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

/* Add this redirection so the strong_alias for __RLIM_T_MATCHES_RLIM64_T
   linking setrlimit64 to {__}setrlimit does not throw a type error.  */
#undef setrlimit
#undef __setrlimit
#define setrlimit setrlimit_redirect
#define __setrlimit __setrlimit_redirect
#include <sys/resource.h>
#undef setrlimit
#undef __setrlimit

#include <errno.h>
#include <sys/types.h>
#include <sysdep.h>


typedef void (*__bpx4srl_t) (const int32_t *resource,
			     const struct rlimit64 *rlimit,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Set the soft and hard limits for RESOURCE to *RLIMITS.
   Only the super-user can increase hard limits.
   Return 0 if successful, -1 if not (and sets errno).  */
int
__setrlimit64 (enum __rlimit_resource resource,
	       const struct rlimit64 *rlimits)
{
  int32_t retval, reason_code;
  int32_t resource_no = resource;
  struct rlimit64 rlim;
  const struct rlimit64 *rlim_ptr;
  INTERNAL_SYSCALL_DECL (retcode);

  if (resource == RLIMIT_CORE)
    {
      /* Internally RLIMIT_CORE uses 4160-byte increments, we need to
	 convert it to the normal format.  */
      rlim.rlim_cur = (rlimits->rlim_cur == RLIM_INFINITY
		       ? RLIM_INFINITY : rlimits->rlim_cur * 4160);
      rlim.rlim_max = (rlimits->rlim_max == RLIM_INFINITY
		       ? RLIM_INFINITY : rlimits->rlim_max * 4160);
      rlim_ptr = &rlim;
    }
  else
    rlim_ptr = rlimits;

  BPX_CALL (setrlimit, __bpx4srl_t, &resource_no, rlim_ptr, &retval,
	    &retcode, &reason_code);

  if (retval != 0)
    {
      __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));
      retval = -1;
    }

  return retval;
}
weak_alias (__setrlimit64, setrlimit64)

strong_alias (__setrlimit64, __setrlimit)
weak_alias (__setrlimit64, setrlimit)
# ifdef SHARED
__hidden_ver1 (__setrlimit64, __GI___setrlimit, __setrlimit64);
# endif
