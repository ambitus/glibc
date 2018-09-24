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

/* There is ambiguity in the USS callable services documentation.
   The kernel_32_t type serves as an indicator of where that ambiguity
   is. This type is mostly just an annotation for us.

   Any parameter of type kernel_32_t indicates a parameter for which
   several things are true:
       (1) The associated value is 32 bits wide.
       (2) The associated value SHOULD be unsigned.
       (3) We don't know whether the kernel interprets it as signed or
	   unsigned.
       (4) We do not currently check whether it the user-supplied value
	   is signed or unsigned, we assume (hope) that the kernel does
	   some sort of error checking.

   TODO: This type exists as an indication that we have not yet done
   the work to determine how the kernel handles the associated
   parameter. We must definitively determine how the kernel handles
   each kernel_32_t type and replace kernel_32_t with uint32_t or
   int32_t in the syscall declaration parameter list as appropriate,
   and if necessary add in the body of the associated syscall
   wrapper.  */
typedef kernel_32_t uint32_t;

typedef void (*bpx4opn_t) (const uint32_t *pathname_len,
			   const char * const *pathname,
			   const int32_t *options,
			   const kernel_32_t *mode,
			   int32_t *retval, int32_t *retcode,
			   int32_t *reason_code);

static inline int
__zos_sys_open (int *errcode, const char *pathname,
		int flags, mode_t mode)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  kernel_32_t kernel_mode = mode;
  BPX_CALL (open, bpx4opn_t, &pathname_len, &pathname, &flags,
	    &kernel_mode, &retval, errcode, &reason_code);
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

/* TODO: the 'count' parameter seems to be interpreted as a signed
   quantity by the kernel, for some utterly inexplicable reason.
   It appears to do the necessary consistency checking itself, so all
   we need to do is make sure it fits into 32 bits. Confirm this
   behavior definitively.  */

typedef void (*bpx4wrt_t) (const int32_t *fd, const void * const *buf,
			   const int32_t *buf_alet,
			   const kernel_32_t *count,
			   int32_t *retval, int32_t *retcode,
			   int32_t *reason_code);

static inline ssize_t
__zos_sys_write (int *errcode, int fd,
		 const void *buf, size_t nbytes)
{
  int32_t retval, reason_code;
  kernel_32_t count;
  const int32_t alet = 0;
  if (!IS_UINT32 (nbytes))
    {
      /* z/OS can't handle writes that can't fit into a 32-bit quantity
	 TODO: is there a better errno for this? */
      *errcode = EINVAL;
      return -1;
    }
  count = nbytes;
  BPX_CALL (write, bpx4wrt_t, &fd, &buf, &alet, &count,
	    &retval, errcode, &reason_code);
  /* TODO: check important reason codes */
  return retval;
}

typedef void (*bpx4sta_t) (const uint32_t *pathname_len,
			   const char * const *pathname,
			   const uint32_t *statbuf_len,
			   struct stat *statbuf,
			   int32_t *retval, int32_t *retcode,
			   int32_t *reason_code);

/* TODO: Check that this is correct.  */
static const uint32_t bpxystat_len = sizeof (struct stat) -
				     (sizeof (__time_t) +
				      sizeof (unsigned long int)) * 3;

static inline int
__zos_sys_stat (int *errcode, const char *pathname,
		struct stat *statbuf)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  uint32_t statbuf_len = bpxystat_len;
  BPX_CALL (stat, bpx4sta_t, &pathname_len, &pathname, &statbuf_len,
	    statbuf, &retval, errcode, &reason_code);
  return retval;
}

/* This is exactly the same as __zos_sys_stat except for the
   BPX_CALL. */
static inline int
__zos_sys_lstat (int *errcode, const char *pathname,
		 struct stat *statbuf)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  uint32_t statbuf_len = bpxystat_len;
  BPX_CALL (lstat, bpx4sta_t, &pathname_len, &pathname, &statbuf_len,
	    statbuf, &retval, errcode, &reason_code);
  return retval;
}

static inline int
__zos_sys_fstat (int *errcode, int fd, struct stat *statbuf)
{
  int32_t retval, reason_code;
  uint32_t statbuf_len = bpxystat_len;
  BPX_CALL (fstat, bpx4sta_t, &fd, &statbuf_len, statbuf, &retval,
	    errcode, &reason_code);
  return retval;
}

/* For z/OS, stat64 is exactly equivalent to stat, so wrappers for any
   of the *stat64 calls shouldn't be necessary.	 */
#endif /* _ZOS_DECL_H */
