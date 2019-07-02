/* z/OS getrlimit64 implementation (64 bits rlim_t).
   Copyright (C) 2019 Free Software Foundation, Inc.
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
#include <sysdep.h>

/* Add this redirection so the strong_alias for __RLIM_T_MATCHES_RLIM64_T
   linking getrlimit64 to {__}getrlimit does not throw a type error.  */
#undef getrlimit
#undef __getrlimit
#define getrlimit getrlimit_redirect
#define __getrlimit __getrlimit_redirect
#include <sys/resource.h>
#undef getrlimit
#undef __getrlimit

/* We need to move the syscall wrapper code into this file because of the
   above include complications.  */
typedef void (*__bpx4grl_t) (const uint32_t *resource,
			     struct rlimit64 *rlimit,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Put the soft and hard limits for RESOURCE in *RLIMITS.
   Returns 0 if successful, -1 if not (and sets errno).  */
int
__getrlimit64 (enum __rlimit_resource resource, struct rlimit64 *rlimits)
{
  int32_t retval, reason_code;
  int32_t resource_no = resource;
  INTERNAL_SYSCALL_DECL (retcode);

  BPX_CALL (getrlimit, __bpx4grl_t, &resource_no, rlimits, &retval,
	    &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    {
      __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));
      retval = -1;
    }

  /* Internally RLIMIT_CORE uses 4160-byte increments, we need to convert
     it to the normal format.  */
  if (resource == RLIMIT_CORE)
    {
      if (rlimits->rlim_cur != RLIM_INFINITY)
	rlimits->rlim_cur *= 4160;
      if (rlimits->rlim_max != RLIM_INFINITY)
	rlimits->rlim_max *= 4160;
    }

  return retval;
}
libc_hidden_def (__getrlimit64)

/* getrlimit and getrlimit64 are exactly the same.  */

strong_alias (__getrlimit64, __GI_getrlimit)
strong_alias (__getrlimit64, __GI___getrlimit)
strong_alias (__getrlimit64, __getrlimit)

weak_alias (__getrlimit64, getrlimit)
weak_alias (__getrlimit64, getrlimit64)
libc_hidden_weak (getrlimit64)
