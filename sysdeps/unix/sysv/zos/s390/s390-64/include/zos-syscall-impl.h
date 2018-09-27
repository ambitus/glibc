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
   but without it linking gets really complicated.

   TODO: merge this with zos-syscall-base.h.  */

#ifndef _ZOS_DECL_H
#define _ZOS_DECL_H 1

/* we are only included from zos-syscall-base.h right now, and we rely
   on things it defines.  */
#ifndef _ZOS_SYSCALL_COMMON_H
# include <zos-syscall-base.h>
#endif

#include <features.h>
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
   The __kernel_32_t type serves as an indicator of where that ambiguity
   is. This type is mostly just an annotation for us.

   Any parameter of type __kernel_32_t indicates a parameter for which
   several things are true:
       (1) The associated value is 32 bits wide.
       (2) The associated value SHOULD be unsigned.
       (3) We don't know whether the kernel interprets it as signed or
	   unsigned.
       (4) We do not currently check whether the user-supplied value
	   is signed or unsigned, we assume (hope) that the kernel does
	   some sort of error checking.

   TODO: This type exists as an indication that we have not yet done
   the work to determine how the kernel handles the associated
   parameter. We must definitively determine how the kernel handles
   each __kernel_32_t type and replace __kernel_32_t with uint32_t or
   int32_t in the syscall declaration parameter list as appropriate,
   and if necessary add in the body of the associated syscall
   wrapper.  */
typedef uint32_t __kernel_32_t;

/***************************************************
 * Utility functions/macros
 ***************************************************/

static inline void
__initialize_times (struct stat *statbuf)
{
  /* We don't get nanosecond-level {a,m,c}time resolution on z/OS. In
     fact, we don't get subsecond-level precision for anything besides
     ctime, for which we get microsecond-level resolution (The fact that
     we only get this level of precision for ONE of them is one of the
     most baffling design decisions in the whole of USS). However, many
     applications expect the st_{a,m,c}tim.tv_nsec fields to exist in
     struct stat. To rectify this, at the moment we simply add on proper
     time fields to the end of struct stat, where it (presumably) won't
     get modified by the kernel, so we can set it up ourselves. We could
     alternatively decouple the userspace struct stat from the kernel
     struct stat, which might be more forward-compatible.

     To be strictly pedantically correct, but also compatible with
     programs that expect the nanosecond field in struct stat
     to be present, we truncate (see below) ctime to the nearest second
     and initialize the nanosecond fields to 0 unconditionally.

     TODO: Should we round the ctime to the nearest second, or just
     truncate it? We should do what the kernel does for atime and
     mtime, but its not clear whether it truncates the those times or
     rounds them. Test out the kernel behavior, but until then we will
     simply truncate it.

     TODO: We could also attempt to convert ctime's microseconds to
     nanoseconds, then propogate that value to the corresponding tv_nsec
     fields whenever atim.tv_sec == ctim.tv_sec or
     mtim.tv_sec == ctim.tv_sec, one of which is almost always true.
     This would yield better behavior of of programs that need to track
     file access and modification time (make, for example). It's
     possible however that this would break the expectations of some
     applications using exotic access patterns or directly modifying
     atime or mtime, (through utimes(), for example).

     TODO: Decouple struct stat from the kernel struct stat as is done
     on Linux. We don't need to use the same struct for both, and if
     we're adding fields to it, defining a conversion function is the
     more pedantically correct approach. This function's logic should
     then be moved into xstatconv.c at that time.  */
#ifdef __USE_XOPEN2K8
  statbuf->st_atim.tv_sec = statbuf->_bpx_atime64;
  statbuf->st_atim.tv_nsec = 0;
  statbuf->st_mtim.tv_sec = statbuf->_bpx_mtime64;
  statbuf->st_mtim.tv_nsec = 0;
  statbuf->st_ctim.tv_sec = statbuf->_bpx_ctime64;
  statbuf->st_ctim.tv_nsec = 0;
#else
  statbuf->st_atime = statbuf->_bpx_atime64;
  statbuf->st_atimensec = 0;
  statbuf->st_mtime = statbuf->_bpx_mtime64;
  statbuf->st_mtimensec = 0;
  statbuf->st_ctime = statbuf->_bpx_ctime64;
  statbuf->st_ctimensec = 0;
#endif
}

/***************************************************
 * Syscall wrappers
 ***************************************************/

/* The first parameter to all of these should be an int *errcode.  */

typedef void (*__bpx4opn_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     const int32_t *options,
			     const __kernel_32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_open (int *errcode, const char *pathname,
		int flags, mode_t mode)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  __kernel_32_t kernel_mode = mode;
  BPX_CALL (open, __bpx4opn_t, &pathname_len, &pathname, &flags,
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

typedef void (*__bpx4wrt_t) (const int32_t *fd, const void * const *buf,
			     const int32_t *buf_alet,
			     const __kernel_32_t *count,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline ssize_t
__zos_sys_write (int *errcode, int fd,
		 const void *buf, size_t nbytes)
{
  int32_t retval, reason_code;
  __kernel_32_t count;
  const int32_t alet = 0;
  if (!IS_UINT32 (nbytes))
    {
      /* z/OS can't handle writes that can't fit into a 32-bit quantity
	 TODO: is there a better errno for this?  */
      *errcode = EINVAL;
      return -1;
    }
  count = nbytes;
  BPX_CALL (write, __bpx4wrt_t, &fd, &buf, &alet, &count,
	    &retval, errcode, &reason_code);
  /* TODO: check important reason codes  */
  return retval;
}

typedef void (*__bpx4sta_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     const uint32_t *statbuf_len,
			     struct stat *statbuf,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef __bpx4sta_t __bpx4lst_t;

typedef void (*__bpx4fst_t) (const int32_t *fd,
			     const uint32_t *statbuf_len,
			     struct stat *statbuf,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* TODO: Check that this is correct.  */
static const uint32_t __bpxystat_len = sizeof (struct stat) -
				       (sizeof (__time_t) +
					sizeof (unsigned long int)) * 3;

static inline int
__zos_sys_stat (int *errcode, const char *pathname,
		struct stat *statbuf)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  uint32_t statbuf_len = __bpxystat_len;
  BPX_CALL (stat, __bpx4sta_t, &pathname_len, &pathname, &statbuf_len,
	    statbuf, &retval, errcode, &reason_code);
  __initialize_times (statbuf);
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
  uint32_t statbuf_len = __bpxystat_len;
  BPX_CALL (lstat, __bpx4lst_t, &pathname_len, &pathname, &statbuf_len,
	    statbuf, &retval, errcode, &reason_code);
  __initialize_times (statbuf);
  return retval;
}

static inline int
__zos_sys_fstat (int *errcode, int fd, struct stat *statbuf)
{
  int32_t retval, reason_code;
  uint32_t statbuf_len = __bpxystat_len;
  BPX_CALL (fstat, __bpx4fst_t, &fd, &statbuf_len, statbuf, &retval,
	    errcode, &reason_code);
  __initialize_times (statbuf);
  return retval;
}

/* For z/OS, stat64 is exactly equivalent to stat, so wrappers for any
   of the *stat64 calls shouldn't be necessary.	 */
#endif /* _ZOS_DECL_H */
