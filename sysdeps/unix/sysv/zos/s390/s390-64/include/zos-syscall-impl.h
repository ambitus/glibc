/* z/OS syscall wrapper prototypes.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.

   TODO: The static inline stuff is really a crutch. We're bleeding
   way too many headers. This library-in-a-header stuff is bad practice,
   but without it linking gets really complicated.  */

#ifndef _ZOS_DECL_H
#define _ZOS_DECL_H 1

/* we are only included from zos-syscall-base.h right now, and we rely
   on things it defines.  */
#ifndef _ZOS_SYSCALL_COMMON_H
# include <zos-syscall-base.h>
#endif

#include <sys/cdefs.h>
#include <sys/types.h>
#define __need_size_t
#include <stddef.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>

#include <limits.h>  /* for PATH_MAX */


typedef void (*bpx4opn_t)(const uint32_t *pathname_len,
			  const char * const *pathname,
			  const int32_t *options,
			  const mode_t *mode,
			  int32_t *retval, int32_t *retcode,
			  int32_t *reason_code);

static inline int
__zos_sys_open (int *errcode, const char *pathname,
		int flags, mode_t mode)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  BPX_CALL (open, bpx4opn_t, &pathname_len, &pathname, &flags,
	    &mode, &retval, errcode, &reason_code);
  /* TODO: check important reason codes. */
  return retval;
}


static inline int
__zos_sys_openat (int *errcode, int dirfd, const char *pathname,
		  int flags, mode_t mode)
{
  if (dirfd == AT_FDCWD || *pathname == '/')
    return __zos_sys_open (errcode, pathname, flags, mode);
  SHIM_NOT_YET_IMPLEMENTED;
}


typedef void (*bpx4wrt_t) (const int32_t *fd, const void * const *buf,
			   const int32_t *buf_alet,
			   const uint32_t *count,
			   int32_t *retval, int32_t *retcode,
			   int32_t *reason_code);

static inline ssize_t
__zos_sys_write (int *errcode, int fd,
		 const void *buf, size_t nbytes)
{
  int32_t retval, reason_code;
  uint32_t count;
  const int32_t alet = 0;
  if (!IS_UINT32 (nbytes))
    {
      /* z/OS can't handle writes larger than a uint32 */
      /* TODO: is there a better errno for this? */
      *errcode = EOVERFLOW;
      return -1;
    }
  count = nbytes;
  BPX_CALL (write, bpx4wrt_t, &fd, &buf, &alet, &count,
	    &retval, errcode, &reason_code);
  /* TODO: check important reason codes */
  return retval;
}

#endif /* _ZOS_DECL_H */
