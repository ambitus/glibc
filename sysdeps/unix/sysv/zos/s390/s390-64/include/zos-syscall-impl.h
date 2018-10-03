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
#include <sys/mman.h>  /* for user-facing mmap constant values.  */

#include <bpxk-constants.h>

/* some things use these aliases  */
#define __zos_sys_fcntl64      __zos_sys_fcntl
#define __zos_sys_fadvise64_64 __zos_sys_fadvise64
#define __zos_sys_truncate64   __zos_sys_truncate
#define __zos_sys_ftruncate64  __zos_sys_ftruncate
#define __zos_sys_pwritev64    __zos_sys_pwritev
#define __zos_sys_pwritev64v2  __zos_sys_pwritev2
#define __zos_sys_preadv64     __zos_sys_preadv
#define __zos_sys_preadv64v2   __zos_sys_preadv2

/* TODO: We could absolutely implement some sort of strace equivalent by
   adding hooks to the wrapper functions in this file.  */

/* TODO: We have not even started trying to map the errnos from the ones
   returned by the bpx services to the ones that would be returned in
   the same situation on linux. In many cases, those two sets are
   actually the same or effectively the same for typical use cases,
   but relying on that is dangerous.  */

/* TODO: Decide whether to implement the *at functions by implementing
   the underlying syscalls here, or by directly implementing  the C
   functions. The implementation logic will be the same regardless.  */

static inline ssize_t
__zos_sys_write (int *errcode, int fd,
		 const void *buf, size_t nbytes);

/* There is ambiguity in the USS callable services documentation.
   The __bpxk_32_t type serves as an indicator of where that ambiguity
   is. This type is mostly just an annotation for us.

   Any parameter of type __bpxk_32_t indicates a parameter for which
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
   each __bpxk_32_t type and replace __bpxk_32_t with uint32_t or
   int32_t in the syscall declaration parameter list as appropriate,
   and if necessary add in the body of the associated syscall
   wrapper.  */
typedef uint32_t __bpxk_32_t;

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


/* Base I/O syscalls.  */

typedef void (*__bpx4opn_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     const int32_t *options,
			     const int32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* TODO: the 'count' parameter of bpx4wrt seems to be interpreted as a
   signed quantity by the kernel, for some utterly inexplicable reason.
   It appears to do the necessary consistency checking itself, so all
   we need to do is make sure it fits into 32 bits. Confirm this
   behavior definitively.  */

typedef void (*__bpx4wrt_t) (const int32_t *fd, const void * const *buf,
			     const int32_t *buf_alet,
			     const __bpxk_32_t *count,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_open (int *errcode, const char *pathname,
		int flags, mode_t mode)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  /* We actually just use the native mode flags format for the bpx
     services, we don't need to do any translation for mode here.  */
  int32_t kernel_mode = mode;
  BPX_CALL (open, __bpx4opn_t, &pathname_len, &pathname, &flags,
	    &kernel_mode, &retval, errcode, &reason_code);
  /* TODO: check important reason codes. */
  /* TODO: confirm retvals are in line with what linux gives.  */
  return retval;
}


static inline int
__zos_sys_openat (int *errcode, int dirfd, const char *pathname,
		  int flags, mode_t mode)
{
  if (dirfd == AT_FDCWD || *pathname == '/')
    return __zos_sys_open (errcode, pathname, flags, mode);
  SHIM_NOT_YET_IMPLEMENTED_FATAL ("openat not implemnted", -1);
}


static inline ssize_t
__zos_sys_write (int *errcode, int fd,
		 const void *buf, size_t nbytes)
{
  int32_t retval, reason_code;
  const int32_t alet = 0;
  if (!IS_UINT32 (nbytes))
    {
      /* z/OS can't handle writes that can't fit into a 32-bit quantity
	 TODO: is there a better errno for this?  */
      *errcode = EINVAL;
      return -1;
    }
  __bpxk_32_t count = nbytes;
  BPX_CALL (write, __bpx4wrt_t, &fd, &buf, &alet, &count,
	    &retval, errcode, &reason_code);
  /* TODO: check important reason codes  */
  /* TODO: confirm retvals are in line with what linux gives.  */
  return retval;
}


/* stat and related syscalls.  */

typedef void (*__bpx4sta_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     const uint32_t *statbuf_len,
			     struct stat *statbuf,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4fst_t) (const int32_t *fd,
			     const uint32_t *statbuf_len,
			     struct stat *statbuf,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef __bpx4sta_t __bpx4lst_t;


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
  /* TODO: confirm retvals are in line with what linux gives.  */
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
  /* TODO: confirm retvals are in line with what linux gives.  */
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
  /* TODO: confirm retvals are in line with what linux gives.  */
  return retval;
}

/* For z/OS, stat64 is exactly equivalent to stat, so wrappers for any
   of the *stat64 calls shouldn't be necessary.	 */


/* mmap and related syscalls.  */

/* ret_map_addr below is actualy a void *, but glibc expects it to
   be a long (or long-compatible type).  */
typedef void (*__bpx4mmp_t) (void * const *addr,
			     const uint64_t *length,
			     const int32_t *protect_opts,
			     const int32_t *map_type,
			     const int32_t *fd,
			     const int64_t *offset,
			     int64_t *ret_map_addr,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4mun_t) (void * const *addr,
			     const uint64_t *length,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4mpr_t) (void * const *addr,
			     const uint64_t *length,
			     const int32_t *protect_opts,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline long
__zos_sys_mmap (int *errcode, void *addr, size_t length, int prot,
		int flags, int fd, off64_t offset)
{
  int32_t retval, reason_code;
  int32_t protect_opts, map_type = 0;
  int64_t retmap;

  /* Convert prot from a linux-compatible format into the format
     expected by the mmap syscall.  */
  if (prot == PROT_NONE)
    protect_opts = ZOS_SYS_PROT_NONE;
  else
    {
      protect_opts = 0;
      if ((prot & PROT_READ) == PROT_READ)
	protect_opts |= ZOS_SYS_PROT_READ;
      if ((prot & PROT_WRITE) == PROT_WRITE)
	protect_opts |= ZOS_SYS_PROT_WRITE;
      if ((prot & PROT_WRITE) == PROT_WRITE)
	protect_opts |= ZOS_SYS_PROT_EXEC;
      /* Linux accepts arbitrary other bits being set in the flags
	 argument, and it's safe for us to do the same because these
	 are only three flags that actually do anything on linux
	 (aside from PROT_SAO, which is powerpc-specific, and will
	 not be present in portable programs).  */
    }

  /* Convert flags from a linux-compatible format into the format
     expected by the mmap syscall.

     TODO: This section is very sketchy at the moment. Do we want to
     emulate exactly the behavior of the S390 mmap syscall, or do we
     want to emulate the behavior of the x86_64 mmap c library
     function? Do we want to handle every possible flag for mmap?

     TODO: Take advantage of MAP_MEGA somehow.  */

  /* Downgrade MAP_SHARED_VALIDATE to MAP_SHARED.  */
  if ((flags & MAP_SHARED_VALIDATE) == MAP_SHARED_VALIDATE)
    map_type |= ZOS_SYS_MAP_SHARED;
  /* Downgrade MAP_FIXED_NOREPLACE to MAP_FIXED.  */
  if ((flags & MAP_FIXED_NOREPLACE) == MAP_FIXED_NOREPLACE)
    map_type |= ZOS_SYS_MAP_FIXED;

  if ((flags & MAP_SHARED) == MAP_SHARED)
    map_type |= ZOS_SYS_MAP_SHARED;
  if ((flags & MAP_PRIVATE) == MAP_PRIVATE)
    map_type |= ZOS_SYS_MAP_SHARED;
  if ((flags & MAP_FIXED) == MAP_FIXED)
    map_type |= ZOS_SYS_MAP_SHARED;

  /* TODO: This is very, very bad. z/OS mmap doesn't support
     MAP_ANONYMOUS.  */
  if ((flags & MAP_ANONYMOUS) == MAP_ANONYMOUS)
    SHIM_NOT_YET_IMPLEMENTED_FATAL ("No MAP_ANONYMOUS support!",
				    (long)MAP_FAILED);
  /* TODO: There is no way for us to support MAP_GROWSDOWN without
     kernel-level support, I think, but we should make sure.  */
  if ((flags & MAP_GROWSDOWN) == MAP_GROWSDOWN)
    SHIM_NOT_YET_IMPLEMENTED_FATAL ("No MAP_GROWSDOWN support",
				    (long)MAP_FAILED);
  /* TODO: There might be a way to implement a subset of MAP_HUGETLB's
     behavior using MAP_MEGA.  */
  if ((flags & MAP_HUGETLB) == MAP_HUGETLB)
    SHIM_NOT_YET_IMPLEMENTED_FATAL ("No MAP_HUGETLB support",
				    (long)MAP_FAILED);
  /* TODO: See mlock.  */
  if ((flags & MAP_HUGETLB) == MAP_HUGETLB)
    SHIM_NOT_YET_IMPLEMENTED_FATAL ("No MAP_LOCKED support",
				    (long)MAP_FAILED);
  /* We just can't support this one.  */
  if ((flags & MAP_NORESERVE) == MAP_NORESERVE)
    SHIM_NOT_YET_IMPLEMENTED_FATAL ("No MAP_NORESERVE support",
				    (long)MAP_FAILED);
  /* TODO: this one's relatively easy. Read(?) on byte from each
     page.  */
  if ((flags & MAP_POPULATE) == MAP_POPULATE)
    SHIM_NOT_YET_IMPLEMENTED_FATAL ("MAP_POPULATE unimplemented",
				    (long)MAP_FAILED);
  /* We probably just can't support this one.  */
  if ((flags & MAP_SYNC) == MAP_SYNC)
    SHIM_NOT_YET_IMPLEMENTED_FATAL ("No MAP_SYNC support",
				    (long)MAP_FAILED);


  if (flags & MAP_HUGE_MASK)
    {
      *errcode = EINVAL;
      return (long)MAP_FAILED;
    }

  /* Ignore MAP_DENYWRITE, MAP_EXECUTABLE, MAP_FILE, and MAP_STACK
     because they are noops.  */

  /* TODO: should we try to support MAP_32BIT? What about
     MAP_UNINITIALIZED (which doesn't have a glibc-provided
     declaration)?  */

  BPX_CALL (mmap, __bpx4mmp_t, &addr, &length, &protect_opts, &map_type,
	    &fd, &offset, &retmap, &retval, errcode, &reason_code);
  /* TODO: confirm retvals are in line with what linux gives.  */
  return (long)retmap;
}


static inline int
__zos_sys_munmap (int *errcode, void *addr, size_t length)
{
  int32_t retval, reason_code;
  BPX_CALL (munmap, __bpx4mun_t, &addr, &length, &retval, errcode,
	    &reason_code);
  /* TODO: confirm retvals are in line with what linux gives.  */
  return retval;
}


static inline int
__zos_sys_mprotect (int *errcode, void *addr, size_t length, int prot)
{
  /* TODO: Reexamine how z/OS mprotect works.  */
  int32_t retval, reason_code;
  int32_t protect_opts;
  /* Convert prot from a linux-compatible format into the format
     expected by the munmap syscall.  */
  if (prot == PROT_NONE)
    protect_opts = ZOS_SYS_PROT_NONE;
  else
    {
      protect_opts = 0;
      if ((prot & PROT_READ) == PROT_READ)
	protect_opts |= ZOS_SYS_PROT_READ;
      if ((prot & PROT_WRITE) == PROT_WRITE)
	protect_opts |= ZOS_SYS_PROT_WRITE;
      if ((prot & PROT_WRITE) == PROT_WRITE)
	protect_opts |= ZOS_SYS_PROT_EXEC;
      /* TODO: Unless we can query mappings, we can't support this.  */
      if ((prot & PROT_GROWSUP) == PROT_GROWSUP)
	SHIM_NOT_YET_IMPLEMENTED_FATAL ("PROT_GROWSUP unsupported",
					-1);
      /* TODO: Unless we can query mappings, we can't support this.  */
      if ((prot & PROT_GROWSDOWN) == PROT_GROWSDOWN)
	SHIM_NOT_YET_IMPLEMENTED_FATAL ("PROT_GROWSDOWN unsupported",
					-1);
      /* Linux accepts arbitrary other bits being set in the flags
	 argument, and it's safe for us to do the same because these
	 are only three flags that actually do anything on linux
	 (aside from PROT_SAO, which is powerpc-specific, and will
	 not be present in portable programs).  */
    }

  BPX_CALL (mprotect, __bpx4mpr_t, &addr, &length, &protect_opts,
	    &retval, errcode, &reason_code);
  /* TODO: confirm retvals are in line with what linux gives.  */
  return retval;
}


/* Core Unix syscalls with trivial wrappers.  */

typedef void (*__bpx4chm_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     const int32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4fcm_t) (const int32_t *fd,
			     const int32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_chmod (int *errcode, const char *pathname, mode_t mode)
{
  /* TODO: Figure out how to avoid IBM's S_ISVTX executable behavior.  */
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  /* TODO: this mask might be unnecessary. Linux allows extra bits
     to be set in chmod's mode. We haven't tested to see whether
     or not the bpx services do the same, so we mask to be safe.  */
  int32_t kernel_mode = mode & 0x0fff;
  BPX_CALL (chmod, __bpx4chm_t, &pathname_len, &pathname, &kernel_mode,
	    &retval, errcode, &reason_code);
  /* retvals are the same as for linux.  */
  return retval;
}


static inline int
__zos_sys_fchmod (int *errcode, int fd, mode_t mode)
{
  /* TODO: Figure out how to avoid IBM's S_ISVTX exacutable behavior.  */
  int32_t retval, reason_code;
  /* TODO: this mask might be unnecessary. Linux allows extra bits
     to be set in chmod's mode. We haven't tested to see whether
     or not the bpx services do the same, so we mask to be safe.  */
  int32_t kernel_mode = mode & 0x0fff;
  BPX_CALL (fchmod, __bpx4fcm_t, &fd, &kernel_mode, &retval, errcode,
	    &reason_code);
  /* retvals are the same as for linux.  */
  return retval;
}


static inline int
__zos_sys_fchmodat (int *errcode, int dirfd, const char *pathname,
		    mode_t mode)
{
  /* We can ignore flags entirely, since the only supported value,
     AT_SYMLINK_NOFOLLOW, is unimplemented.  */
  if (dirfd == AT_FDCWD || *pathname == '/')
    return __zos_sys_chmod (errcode, pathname, mode);
  SHIM_NOT_YET_IMPLEMENTED_FATAL ("fchmodat not implemented", -1);
}


typedef void (*__bpx4cho_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     const uint32_t *owner_uid,
			     const uint32_t *group_id,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4fco_t) (const int32_t *fd,
			     const uint32_t *owner_uid,
			     const uint32_t *group_id,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef __bpx4cho_t __bpx4lco_t;


static inline int
__zos_sys_chown (int *errcode, const char *pathname, uid_t owner,
		 gid_t group)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  uint32_t owner_uid = owner, group_id = group;
  BPX_CALL (chown, __bpx4cho_t, &pathname_len, &pathname, &owner_uid,
	    &group_id, &retval, errcode, &reason_code);
  return retval;
}


static inline int
__zos_sys_fchown (int *errcode, int fd, uid_t owner, gid_t group)
{
  int32_t retval, reason_code;
  uint32_t owner_uid = owner, group_id = group;
  BPX_CALL (fchown, __bpx4fco_t, &fd, &owner_uid, &group_id, &retval,
	    errcode, &reason_code);
  return retval;
}


static inline int
__zos_sys_lchown (int *errcode, const char *pathname, uid_t owner,
		  gid_t group)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  uint32_t owner_uid = owner, group_id = group;
  BPX_CALL (lchown, __bpx4lco_t, &pathname_len, &pathname, &owner_uid,
	    &group_id, &retval, errcode, &reason_code);
  return retval;
}


static inline int
__zos_sys_fchownat (int *errcode, int dirfd, const char *pathname,
		    uid_t owner, gid_t group, int flags)
{
  int32_t retval, reason_code;
  if ((flags & AT_EMPTY_PATH) && *pathname == '\0')
    {
      if (dirfd == AT_FDCWD)
	/* TODO: Is this okay? "." can't be a symlink, right?  */
	return __zos_sys_chown (errcode, ".", owner, group);
      return __zos_sys_fchown (errcode, dirfd, owner, group);
    }
  if (dirfd == AT_FDCWD || *pathname == '/')
    {
      if (flags & AT_SYMLINK_NOFOLLOW)
	return __zos_sys_lchown (errcode, pathname, owner, group);
      return __zos_sys_chown (errcode, pathname, owner, group);
    }
  SHIM_NOT_YET_IMPLEMENTED_FATAL ("fchownat not implemented", -1);
}


typedef void (*__bpx4chd_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4fcd_t) (const int32_t *dfd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_chdir (int *errcode, const char *path)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (path, -1);
  BPX_CALL (chdir, __bpx4chd_t, &pathname_len, &path, &retval, errcode,
	    &reason_code);
  return retval;
}


static inline int
__zos_sys_fchdir (int *errcode, int fd)
{
  int32_t retval, reason_code;
  BPX_CALL (fchdir, __bpx4fcd_t, &fd, &retval, errcode, &reason_code);
  return retval;
}

/* TODO: ftruncate on linux can be used to set the size of a shared
   memory object. Put in some handling for that, because z/OS ftruncate
   sure can't do that. Do that once we figure out how to support POSIX
   shared memory objects.  */

typedef void (*__bpx4tru_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     uint64_t *file_length,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4ftr_t) (const int32_t *fd,
			     uint64_t *file_length,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_truncate (int *errcode, const char *path, off_t length)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (path, -1);
  if (length < 0)
    {
      *errcode = EINVAL;
      return -1;
    }
  uint64_t file_length = length;
  BPX_CALL (truncate, __bpx4tru_t, &pathname_len, &path, &file_length,
	    &retval, errcode, &reason_code);
  return retval;
}


static inline int
__zos_sys_ftruncate (int *errcode, int fd, off_t length)
{
  int32_t retval, reason_code;
  if (length < 0)
    {
      *errcode = EINVAL;
      return -1;
    }
  uint64_t file_length = length;
  BPX_CALL (ftruncate, __bpx4ftr_t, &fd, &file_length, &retval, errcode,
	    &reason_code);
  return retval;
}


typedef void (*__bpx4mkd_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     uint32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_mkdir (int *errcode, const char *pathname, mode_t mode)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  uint32_t dmode = mode;
  BPX_CALL (mkdir, __bpx4mkd_t, &pathname_len, &pathname, &dmode,
	    &retval, errcode, &reason_code);
  return retval;
}


static inline int
__zos_sys_mkdirat (int *errcode, int dirfd, const char *pathname,
		   mode_t mode)
{
  if (dirfd == AT_FDCWD || *pathname == '/')
    return __zos_sys_mkdir (errcode, pathname, mode);
  SHIM_NOT_YET_IMPLEMENTED_FATAL ("mkdirat not implemented", -1);
}


typedef void (*__bpx4rmd_t) (const uint32_t *pathname_len,
			     const char * const *pathname,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_rmdir (int *errcode, const char *pathname)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  BPX_CALL (rmdir, __bpx4rmd_t, &pathname_len, &pathname, &retval,
	    errcode, &reason_code);
  return retval;
}


/* Notice that these have different prototypes from all the other
   syscalls.  */
typedef void (*__bpx4uid_t) (const uint32_t *);

typedef __bpx4uid_t __bpx4geu_t;
typedef __bpx4uid_t __bpx4gid_t;
typedef __bpx4uid_t __bpx4geg_t;


static inline int
__zos_sys_getuid (int *errcode __attribute__ ((unused)))
{
  uint32_t ret_id;
  BPX_CALL (getuid, __bpx4uid_t, &ret_id);
  return ret_id;
}


static inline int
__zos_sys_geteuid (int *errcode __attribute__ ((unused)))
{
  uint32_t ret_id;
  BPX_CALL (geteuid, __bpx4geu_t, &ret_id);
  return ret_id;
}


static inline int
__zos_sys_getgid (int *errcode __attribute__ ((unused)))
{
  uint32_t ret_id;
  BPX_CALL (getgid, __bpx4gid_t, &ret_id);
  return ret_id;
}


static inline int
__zos_sys_getegid (int *errcode __attribute__ ((unused)))
{
  uint32_t ret_id;
  BPX_CALL (getegid, __bpx4geg_t, &ret_id);
  return ret_id;
}


typedef void (*__bpx4umk_t) (const uint32_t *file_mode_creation_mask,
			     uint32_t *ret_mask);


static inline mode_t
__zos_sys_umask (int *errcode, mode_t mask)
{
  /* TODO: See if umask works with ACLs the same way as it does on
     linux.  */
  uint32_t file_mode_creation_mask = mask, ret_mask;
  BPX_CALL (umask, __bpx4umk_t, &file_mode_creation_mask, &ret_mask);
  return (mode_t)ret_mask;
}


/* setuid/setgid, etc.

   TODO: setuid and setgid programs interact in some way with the MVS
   permission system. Figure out exactly how they interact.

   TODO: Can we emulate the _POSIX_SAVED_IDS feature, which the linux
   version implements? Can we implement setresuid/setresgid
   meaningfully?

   TODO: The linux threading implementation requires special handling
   for these. Remember that when implementing whatever threading
   implemenation we decide on.

   TODO: Why do the bpx services have dedicated seteuid/gid syscalls?
   They are meaningless given setreuid/gid syscalls. */

typedef void (*__bpx4sui_t) (const uint32_t *,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef __bpx4sui_t __bpx4sgi_t;

typedef void (*__bpx4sru_t) (const uint32_t *ruid,
			     const uint32_t *euid,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef __bpx4sru_t __bpx4srg_t;


static inline int
__zos_sys_setuid (int *errcode, uid_t uid)
{
  int32_t retval, reason_code;
  uint32_t user_id = uid;
  BPX_CALL (setuid, __bpx4sui_t, &user_id, &retval, errcode,
	    &reason_code);
  return retval;
}


static inline int
__zos_sys_setgid (int *errcode, gid_t gid)
{
  int32_t retval, reason_code;
  uint32_t group_id = gid;
  BPX_CALL (setgid, __bpx4sgi_t, &group_id, &retval, errcode,
	    &reason_code);
  return retval;
}


static inline int
__zos_sys_setreuid (int *errcode, uid_t ruid, uid_t euid)
{
  int32_t retval, reason_code;
  uint32_t ruser_id = ruid, euser_id = euid;
  BPX_CALL (setreuid, __bpx4sru_t, &ruser_id, &euser_id, &retval,
	    errcode, &reason_code);
  return retval;
}


static inline int
__zos_sys_setregid (int *errcode, gid_t rgid, gid_t egid)
{
  int32_t retval, reason_code;
  uint32_t rgroup_id = rgid, egroup_id = egid;
  BPX_CALL (setregid, __bpx4srg_t, &rgroup_id, &egroup_id, &retval,
	    errcode, &reason_code);
  return retval;
}

#endif /* _ZOS_DECL_H */
