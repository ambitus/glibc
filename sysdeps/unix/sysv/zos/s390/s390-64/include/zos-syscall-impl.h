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
#include <utime.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>

#include <sys/mman.h>  /* for user-facing mmap constant values.  */
#include <signal.h>
#include <sir.h>

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

static inline ssize_t
__zos_sys_write (int *errcode, int fd,
		 const void *buf, size_t nbytes);
static inline int
__zos_sys_fcntl (int *errcode, int fd, int cmd, void *arg);

static inline int
__zos_sys_lstat (int *errcode, const char *pathname,
		 struct stat *statbuf);

static inline int
__zos_sys_fstat (int *errcode, int fd, struct stat *statbuf);

static inline int
__zos_sys_close (int *errcode, int fd);

extern int
__set_file_tag_if_empty_unsafe (int fd,
				const struct zos_file_tag *tag)
  attribute_hidden;

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

/* Translate a path into EBCDIC and check its size along the way.  */

static inline uint32_t
translate_and_check_size (const char *str, char ebcstr[__BPXK_PATH_MAX])
{
  return (uint32_t) tr_e_until_chr_or_len (str, ebcstr, '\0',
					   __BPXK_PATH_MAX);
}


/***************************************************
 * Syscall wrappers
 ***************************************************/

/* The first parameter to all of these should be an int *errcode.  */


/* Base I/O syscalls.  */

/* Map linux O_* flags to z/OS O_* flags. We do this for the benefit
   of poorly-written programs that make too many assumptions about
   the values of certain constants.

   TODO: Profile. If it becomes apparent that any noticable amount
   of time is spent in this code, we should consider just using the
   z/OS native values as the actual values for the O_* constants,
   skipping this step entirely.  */
static inline int32_t
__map_common_oflags_to_zos (int flags)
{
  uint32_t zflags;
  uint32_t uflags = (uint32_t) flags;

  /* Handle ACCMODE, lowest 3 bits.  */
  switch (uflags & O_ACCMODE)
  {
  case O_RDONLY:
  default:
    /* On linux, the 0x3 value actually results in open returning a
       file descriptor that cannot be used for reading or writing. I
       don't know how to emulate that behavior, so we make it return
       a readonly fd.  */
    zflags = ZOS_SYS_O_RDONLY;
  case O_WRONLY:
    zflags = ZOS_SYS_O_WRONLY;
  case O_RDWR:
    zflags = ZOS_SYS_O_RDWR;
  }

  /* Get the bit we want and shift it to where we want it.
     TODO: The shift stuff is really here to avoid branches, which may or
     may not be less efficient.  */
#define validate_shift(larger, smaller)					\
  ({ const int _sh = __builtin_ctz (larger) - __builtin_ctz (smaller);	\
    _Static_assert (_sh > 0, "bad flag values");			\
    _sh; })
#define shift_up(flg) \
  zflags |= (uflags & (flg)) << validate_shift (ZOS_SYS_##flg, (flg))
#define shift_down(flg) \
  zflags |= (uflags & (flg)) >> validate_shift ((flg), ZOS_SYS_##flg)

  shift_up (O_CREAT);
  shift_down (O_EXCL);
  shift_down (O_NOCTTY);
  shift_down (O_TRUNC);
  shift_down (O_APPEND);
  shift_down (O_NONBLOCK);
  if ((O_SYNC & uflags) == O_SYNC) zflags |= ZOS_SYS_O_SYNC;
  shift_down (O_LARGEFILE);
  /* We can't emulate O_DSYNC, so alias it to O_SYNC.  */
  if (O_DSYNC & uflags) zflags |= ZOS_SYS_O_SYNC;
  /* z/OS TODO: Is this appropriate?  */
  if ((O_ASYNC & uflags) == O_ASYNC) zflags |= ZOS_SYS_O_ASYNCSIG;

  /* TODO: Allow O_NOLARGEFILE.  */
#undef shift_up
#undef shift_down

  return (int32_t) zflags;
}

static inline int32_t
__map_common_oflags_from_zos (int zflgs)
{
  uint32_t flags;
  uint32_t zflags = (uint32_t) zflgs;

  /* Handle ACCMODE, lowest 3 bits.  */
  switch (zflags & ZOS_SYS_O_ACCMODE)
    {
    case ZOS_SYS_O_RDONLY:
      flags = O_RDONLY;
    case ZOS_SYS_O_WRONLY:
      flags = O_WRONLY;
    case ZOS_SYS_O_RDWR:
      flags = O_RDWR;
    case 0:
      /* This isn't actually documented to be a possibility, but we need
	 to cover all bases.  */
      flags = 0x3;
    }

#define shift_up(flg)							\
  flags |= (zflags & (flg)) << validate_shift ((flg), ZOS_SYS_##flg)
#define shift_down(flg)							\
  flags |= (zflags & (flg)) >> validate_shift (ZOS_SYS_##flg, (flg))

  /* TODO: Allow O_NOLARGEFILE.  */
  flags &= ~ZOS_SYS_O_NOLARGEFILE;

  shift_down (O_CREAT);
  shift_up (O_EXCL);
  shift_up (O_NOCTTY);
  shift_up (O_TRUNC);
  shift_up (O_APPEND);
  shift_up (O_NONBLOCK);
  if ((ZOS_SYS_O_SYNC & zflags) == ZOS_SYS_O_SYNC) flags |= O_SYNC;
  shift_up (O_LARGEFILE);
  /* z/OS TODO: Is this appropriate?  */
  if ((ZOS_SYS_O_ASYNCSIG & zflags) == ZOS_SYS_O_ASYNCSIG)
    flags |= O_ASYNC;

#undef validate_shift
#undef shift_up
#undef shift_down

  return (int32_t) flags;
}

typedef void (*__bpx4opn_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     const int32_t *options,
			     const int32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* TODO: the 'count' parameter of bpx4wrt seems to be interpreted as a
   signed quantity by the kernel, for some utterly inexplicable reason.
   It appears to do the necessary consistency checking itself, so all
   we need to do is make sure it fits into 32 bits. Confirm this
   behavior definitively.  */

typedef void (*__bpx4red_t) (const int32_t *fd, void * const *buf,
			     const int32_t *buf_alet,
			     const __bpxk_32_t *count,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

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
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  /* We use the linux values for the user-visible O_* constants, but
     then we translate them to the proper z/OS constants, and to an
     extent validate them and reject ones that we cannot support or
     emulate.  */
  int32_t kernel_mode = mode;

  /* There are some flags that z/OS has no equivalent for as of yet,
     so we must fall back to a racy emulation.  */

  if (flags & O_DIRECT)
    {
      /* TODO: This! */
      SHIM_NOT_YET_IMPLEMENTED_FATAL ("O_DIRECT", -1);
    }

  if (flags & O_NOATIME)
    {
      /* TODO: This! */
      SHIM_NOT_YET_IMPLEMENTED_FATAL ("O_NOATIME", -1);
    }

  if (flags & O_PATH)
    {
      /* TODO: This! */
      SHIM_NOT_YET_IMPLEMENTED_FATAL ("O_PATH", -1);
    }

  if (flags & O_TMPFILE)
    {
      /* TODO: This! */
      SHIM_NOT_YET_IMPLEMENTED_FATAL ("O_TMPFILE", -1);
    }

  if (flags & O_ASYNC)
    {
      /* TODO: Need to ask IBM. What is O_ASYNCSIG? Is it similar to
         O_ASYNC on other platforms? If so, is it a drop-in
	 replacement?  */
      SHIM_NOT_YET_IMPLEMENTED_FATAL ("O_ASYNC", -1);
    }

  int tmp_err;
  bool not_creating = false;
  struct stat path_target;

  /* Poor man's O_NOFOLLOW and O_DIRECTORY emulation.  */
  if (flags & (O_NOFOLLOW | O_DIRECTORY))
    {
      /* Try an lstat. If it works, check that the target path is the
	 correct type of file. If it is not, then fail.
	 TODO: If we're trying to O_CREAT a file that doesn't exist
	 the first stat will always fail, and having failure all the
	 time in the syscall trace is less than desireable.  */
      if (__zos_sys_lstat (&tmp_err, pathname, &path_target) == 0)
	{
	  if ((flags & O_NOFOLLOW) && S_ISLNK (path_target.st_mode))
	    {
	      *errcode = ELOOP;
	      return -1;
	    }
	  if ((flags & O_DIRECTORY) && !S_ISDIR (path_target.st_mode))
	    {
	      *errcode = ENOTDIR;
	      return -1;
	    }
	  not_creating = true;
	}
      else if (!(flags & O_CREAT) || tmp_err != ENOENT)
	{
	  *errcode = tmp_err;
	  return -1;
	}
      else
	not_creating = false;
    }

  int32_t zflags = __map_common_oflags_to_zos (flags);
  /* Do the syscall.  */
  BPX_CALL (open, __bpx4opn_t, &path_len, translated_path, &zflags,
	    &kernel_mode, &retval, errcode, &reason_code);
  /* TODO: check important reason codes. */
  /* TODO: confirm retvals are in line with what linux gives.  */

  /* If the call itself fails, we are done.  */
  if (retval == -1)
    return retval;

  if ((flags & (O_NOFOLLOW | O_DIRECTORY)) && not_creating)
    {
      struct stat fd_target;

      /* While we already checked the path we were opening once, that
	 alone is not sufficient. It's still possible someone removed
	 the file that we checked and replaced it with something else, so
	 now we do an fstat on whatever we actually ended up opening to
	 see if it seems okay.  */
      if (__zos_sys_fstat (&tmp_err, retval, &fd_target) == -1)
	{
	  __zos_sys_close (errcode, retval);
	  *errcode = tmp_err;
	  return -1;
	}

      if ((flags & O_DIRECTORY) && !S_ISDIR (fd_target.st_mode))
	{
	  __zos_sys_close (errcode, retval);
	  *errcode = ENOTDIR;
	  return -1;
	}

      /* For O_NOFOLLOW, check that the inode and dev numbers of the file
	 we eneded up opening are the same as the file we checked, if
	 not, then fail.

	 Unfortunately, if O_TRUNC was used then damage may have already
	 been done and there's not a lot we can do about it. Also, if
	 it looked like we were creating the file with O_CREAT when we
	 checked the path, then we have nothing to compare to.
	 TODO: Can we do better? Also, ask IBM to add these flags.
	 TODO: Should we fail here, or should we retry?  */
      if ((flags & O_NOFOLLOW)
	  || fd_target.st_ino != path_target.st_ino
	  || fd_target.st_dev != path_target.st_dev)
	{
	  /* TODO: This errno is not quite correct, but I can't think of
	     anything better right now.  */
	  __zos_sys_close (errcode, retval);
	  *errcode = ELOOP;
	  return -1;
	}
    }

  if (flags & O_CLOEXEC)
    {
      /* TODO: There's a race condition here, but we could mitigate it by
	 incrementing a global hazard variable that all threads check
	 before forking. If it's nonzero the forking thread waits for a
	 while then checks it again, if it has been incremented then wait
	 again, but if that happens too many times just go ahead and fork
	 anyway. Decrement it after we've done the cloexec call. For
	 total safety, we would need some way to decrement the value if
	 this thread dies (maybe a secondary thread-local sentinel and a
	 resource manager?). */
      if (__glibc_unlikely (__zos_sys_fcntl (&tmp_err, retval, F_SETFD,
					     (void *) FD_CLOEXEC) == -1))
	{
	  /* TODO: should we report this error to the user, or silently
	      ignore it? It might be confusing, and a failure here would
	      almost always be nonfatal, it would just be a slow leak of
	      file descriptors. For now we report it.  */
	  *errcode = tmp_err;
	  return -1;
	}
    }

  /* z/OS NOTE: Unilaterally tag everything opened for writing as
     ASCII. This should only succeed when the file is empty.  */
  if (flags & (O_WRONLY | O_RDWR))
    {
      struct zos_file_tag tag;

      tag.ft_ccsid = 819;
      tag.ft_flags = FT_PURETXT;

      __zos_sys_fcntl (&tmp_err, retval, F_SETTAG, &tag);
    }

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


static inline int
__zos_sys_creat (int *errcode, const char *pathname, mode_t mode)
{
  return __zos_sys_open (errcode, pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
}


static inline ssize_t
__zos_sys_read (int *errcode, int fd, void *buf, size_t nbytes)
{
  /* TODO: Note that while the linux syscall returns an ssize_t, we can
     only return an int. Address that somehow.  */

  int32_t retval, reason_code;
  const int32_t alet = 0;
  if (!IS_UINT32 (nbytes))
    {
      /* z/OS can't handle reads that can't fit into a 32-bit quantity
	 TODO: is there a better errno for this?  */
      *errcode = EINVAL;
      return -1;
    }
  __bpxk_32_t count = nbytes;
  BPX_CALL (read, __bpx4red_t, &fd, &buf, &alet, &count,
	    &retval, errcode, &reason_code);
  /* TODO: check important reason codes  */
  /* TODO: confirm retvals are in line with what linux gives.  */
  return retval;
}


static inline ssize_t
__zos_sys_write (int *errcode, int fd, const void *buf, size_t nbytes)
{
  /* TODO: Note that while the linux syscall returns an ssize_t, we can
     only return an int. Address that somehow.  */

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


typedef void (*__bpx4vecio_t) (const int32_t *fd,
			       const int32_t *iov_count,
			       const struct iovec *iov,
			       const int32_t *iov_alet,
			       const int32_t *iov_buffer_alet,
			       int32_t *retval, int32_t *retcode,
			       int32_t *reason_code);

typedef __bpx4vecio_t __bpx4rdv_t;
typedef __bpx4vecio_t __bpx4wrv_t;

static inline ssize_t
__zos_sys_readv (int *errcode,
		 int fd, const struct iovec *iov, int iovcnt)
{
  /* TODO: Allow users to optionally specify the buffer ALET in some
     way that cannot potentially break compatibility with other
     platforms. Do this for all syscalls using ALETs.

     TODO: Note that while the linux syscall returns an ssize_t, we can
     only return an int. Address that somehow.  */

  /* As it turns out, the z/OS format for iovecs exactly matches
     the Linux format.  */
  int32_t retval, reason_code;
  const int32_t alet = 0;

  BPX_CALL (readv, __bpx4rdv_t, &fd,
	    &iovcnt, iov, &alet, &alet, &retval, errcode, &reason_code);
  return retval;
}


static inline ssize_t
__zos_sys_writev (int *errcode,
		  int fd, const struct iovec *iov, int iovcnt)
{
  /* TODO: Allow users to optionally specify the buffer ALET in some
     way that cannot potentially break compatibility with other
     platforms. Do this for all syscalls using ALETs.

     TODO: Apparently, the system does absolutely no checking on how
     many bytes are to be written, so we should probably do some sort
     of validation.

     TODO: Note that while the linux syscall returns an ssize_t, we can
     only return an int. Address that somehow.  */

  /* As it turns out, the z/OS format for iovecs exactly matches
     the Linux format.  */
  int32_t retval, reason_code;
  const int32_t alet = 0;

  BPX_CALL (writev, __bpx4wrv_t, &fd,
	    &iovcnt, iov, &alet, &alet, &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4lsk_t) (const int32_t *fd,
			     int64_t *offset,
			     const int32_t *ref,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline off_t
__zos_sys_lseek (int *errcode, int fd, off_t offset, int whence)
{
  int32_t retval, reason_code;
  int64_t off = offset;

  /* We don't need to translate whence.  */
  BPX_CALL (lseek, __bpx4lsk_t, &fd, &off, &whence,
	    &retval, errcode, &reason_code);

  return retval == 0 ? off : -1;
}


typedef void (*__bpx4pip_t) (int32_t *read_fd, int32_t *write_fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_pipe2 (int *errcode, int pipedes[2], int flags)
{
  int32_t retval, reason_code;
  int32_t *rfd, *wfd;
  int tmp_err;

  if (flags & O_DIRECT)
    {
      /* We don't support and can't emulate Linux's peculiar packet
	 mode pipes.  */
      *errcode = EINVAL;
      return -1;
    }

  rfd = &pipedes[0];
  wfd = &pipedes[1];

  BPX_CALL (pipe, __bpx4pip_t, rfd, wfd, &retval, errcode, &reason_code);

  if (flags & O_CLOEXEC)
    {
      /* z/OS TODO: There's a race condition here, but we could mitigate
	 it by incrementing a global hazard variable that all threads
	 check before forking. If it's nonzero the forking thread waits
	 for a while then checks it again, if it has been incremented
	 then wait again, but if that happens too many times just go
	 ahead and fork anyway. Decrement it after we've done the cloexec
	 call. For total safety, we would need some way to decrement the
	 value if this thread dies (maybe a secondary thread-local
	 sentinel and a resource manager?).  */

      if (__zos_sys_fcntl (&tmp_err, pipedes[0], F_SETFD,
			   (void *) FD_CLOEXEC) == -1
	  || __zos_sys_fcntl (&tmp_err, pipedes[1], F_SETFD,
			      (void *) FD_CLOEXEC) == -1)
	{
	  /* z/OS TODO: should we report this error to the user, or
	     silently ignore it? It might be confusing, and a failure
	     here would almost always be nonfatal, it would just be a
	     slow leak of file descriptors. For now we report it.  */
	  *errcode = tmp_err;
	  return -1;
	}
    }

  /* Would this need to be guarded in any manner, like should be done for
     O_CLOEXEC?  */
  if (flags & O_NONBLOCK
      && (__zos_sys_fcntl (&tmp_err, pipedes[0], F_SETFL,
			   (void *) O_NONBLOCK) == -1
	  || __zos_sys_fcntl (&tmp_err, pipedes[1], F_SETFL,
			      (void *) O_NONBLOCK) == -1))
    {
      *errcode = tmp_err;
      return -1;
    }

  return retval;
}


/* stat and related syscalls.  */

typedef void (*__bpx4sta_t) (const uint32_t *pathname_len,
			     const char *pathname,
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
  char translated_path[__BPXK_PATH_MAX];
  uint32_t statbuf_len = __bpxystat_len;
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (stat, __bpx4sta_t, &path_len, translated_path, &statbuf_len,
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
  char translated_path[__BPXK_PATH_MAX];
  uint32_t statbuf_len = __bpxystat_len;
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (lstat, __bpx4lst_t, &path_len, translated_path, &statbuf_len,
	    statbuf, &retval, errcode, &reason_code);
  __initialize_times (statbuf);
  /* TODO: confirm retvals are in line with what linux gives.  */
  return retval;
}

/* For z/OS, stat64 is exactly equivalent to stat, so wrappers for any
   of the *stat64 calls shouldn't be necessary.	 */


typedef void (*__bpx4clo_t) (const int32_t *fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_close (int *errcode, int fd)
{
  int32_t retval, reason_code;
  BPX_CALL (close, __bpx4clo_t, &fd, &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4fct_t) (const int32_t *fd,
			     const int32_t *action,
			     intptr_t *argument,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* 'arg' is actually the argument, not necessarily a pointer, glibc just
   prefers to treat that argument as a void pointer because the kernel
   is expected to know its size and type from the command.  */
static inline int
__zos_sys_fcntl (int *errcode, int fd, int cmd, void *arg)
{
  int32_t retval, reason_code;
  bool set_cloexec_after = false;
  int32_t zcmd = cmd;
  int32_t real_arg;
  void *real_arg_ptr = &real_arg;

  /* Map linux fcntl commands to z/OS commands. Do a racy emulation
     of the ones we can emulate, and return ENOSYS for the rest.  */
  switch (cmd)
    {
    case F_DUPFD_CLOEXEC:
      /* TODO: There's a race condition here, but we could mitigate it by
	 incrementing a global hazard variable that all threads check
	 before forking. If it's nonzero the forking thread waits for a
	 while then checks it again, if it has been incremented then wait
	 again, but if that happens too many times just go ahead and fork
	 anyway. Decrement it after we've done the cloexec call.  */
      zcmd = F_DUPFD;
      set_cloexec_after = true;
      /* Fallthrough.  */

    case F_DUPFD:
    case F_SETFD:
      real_arg = (int32_t) (intptr_t) arg;
      /* TODO: Maybe provide a define for FD_CLOFORK.  */
      break;

    case F_GETFD:
    case F_GETFL:
      real_arg = 0;
      break;

    case F_SETTAG:
      real_arg_ptr = &arg;
      break;

    case F_CONTROL_CVT:
      real_arg_ptr = arg;
      break;

    /* z/OS TODO: The rest of these.  */
    case F_SETFL:

    /* case F_GETLK64:  */
    case F_GETLK:
    /* case F_SETLK64:  */
    case F_SETLK:
    /* case F_SETLKW64:  */
    case F_SETLKW:

    case F_GETOWN:
    case F_SETOWN:

    case F_GETOWN_EX:
    case F_SETOWN_EX:

    case F_GETSIG:
    case F_SETSIG:

    case 17 /* F_GETOWNER_UIDS  */:

    case F_OFD_GETLK:
    case F_OFD_SETLK:
    case F_OFD_SETLKW:

    case F_GETLEASE:
    case F_SETLEASE:

    case F_NOTIFY:

    case F_GETPIPE_SZ:
    case F_SETPIPE_SZ:

    case F_ADD_SEALS:
    case F_GET_SEALS:

    case F_GET_RW_HINT:
    case F_SET_RW_HINT:
    case F_GET_FILE_RW_HINT:
    case F_SET_FILE_RW_HINT:
      SHIM_NOT_YET_IMPLEMENTED_FATAL ("SOME F_* flag", -1);
      break;

    default:
      real_arg = (int32_t) (intptr_t) arg;
      break;
    }

  BPX_CALL (fcntl, __bpx4fct_t, &fd, &zcmd, real_arg_ptr, &retval,
	    errcode, &reason_code);
  /* TODO: confirm retvals are in line with what linux gives.  */

  /* z/OS TODO: We could actually make this more safe by atomically
     incrementing a sentinel value that is checked against zero in
     the fork wrapper. The fork wrapper must wait until the sentinel
     is zero again. For total safety, we would need some way to decrement
     the value if this thread dies (maybe a secondary thread-local
     sentinel and a resource manager?).  */
  if (set_cloexec_after && retval != -1)
    {
      int tmp_err;
      if (__glibc_unlikely (__zos_sys_fcntl (&tmp_err, fd, F_SETFD,
					     (void *) FD_CLOEXEC) == -1))
	{
	  /* TODO: should we report this error to the user, or silently
	     ignore it? It might be confusing, and a failure here would
	     almost always be nonfatal, it would just be a slow leak of
	     file descriptors. For now we report it.  */
	  *errcode = tmp_err;
	  return -1;
	}
    }

  /* TODO: For F_DUPFD2, F_GETFD, F_GETFL, and F_GETLK, we need to
     convert the results from a z/OS format to a linux-like format.  */
  switch (cmd)
    {
    case F_GETFL:
      if (retval != -1)
	retval = __map_common_oflags_from_zos (retval);
      break;

    case F_GETFD:
      /* z/OS TODO: Some programs are poorly written so that they test
	 the result for equality against FD_CLOEXEC, not for presence.
	 For now we do nothing, so those will break in the presence of
	 FD_CLOFORK. Should we do anything?  */

    case F_GETLK:
    default:
      break;
    }

  return retval;
}


static inline int
__zos_sys_dup2 (int *errcode, int oldfd, int newfd)
{
  int32_t retval, reason_code;
  intptr_t nfd = newfd;
  const int32_t fcntl_cmd = ZOS_SYS_F_DUPFD2;
  BPX_CALL (fcntl, __bpx4fct_t, &oldfd, &fcntl_cmd, &nfd, &retval,
	    errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4rdl_t) (const __bpxk_32_t *link_name_len,
			     const char *link_name,
			     const __bpxk_32_t *buffer_len,
			     char * const *bufptr,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);
static inline ssize_t
__zos_sys_readlink (int *errcode,
		    const char *pathname, char *buf, size_t bufsiz)
{
  /* TODO: Note that bufsize is size_t on linux and (u?)int32_t on z/OS.
     We need to address that somehow, but what's the right course of
     action?  */
  int32_t retval, reason_code;
  uint32_t bufsiz_trunc;
  char translated_link_path[__BPXK_PATH_MAX];
  uint32_t link_path_len =
    translate_and_check_size (pathname, translated_link_path);

  if (__glibc_unlikely (link_path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  if (__glibc_unlikely (bufsiz > INT32_MAX))
    /* TODO: What should we do here?  */
    bufsiz_trunc = INT32_MAX;
  else
    bufsiz_trunc = bufsiz;

  BPX_CALL (readlink, __bpx4rdl_t, &link_path_len, translated_link_path,
	    &bufsiz_trunc, &buf, &retval, errcode, &reason_code);

  if (retval != -1)
    /* Translate the written characters to ASCII. Anything that might
	be a filename is assumed to be EBCDIC.  */
    tr_a_until_len_in_place (buf, retval);

  return retval;
}


static inline int
__zos_sys_readlinkat (int *errcode, int dirfd,
		      const char *pathname, char *buf, size_t bufsiz)
{
  /* TODO: I'd like to move the *at syscall emulation somewhere
     else, however the inline form is used in linux code, so until
     we override those usages, this needs to stay.  */

  if (dirfd == AT_FDCWD || *pathname == '/')
    return __zos_sys_readlink (errcode, pathname, buf, bufsiz);
  SHIM_NOT_YET_IMPLEMENTED_FATAL ("readlinkat not implemented", -1);
}


typedef void (*__bpx4uti_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     const int64_t *times,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_utime (int *errcode, const char *pathname,
		 const struct utimbuf *times)
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (utime, __bpx4uti_t, &path_len, translated_path,
	    times, &retval, errcode, &reason_code);

  return retval;
}


typedef void (*__bpx4exi_t) (const int32_t *status);

static inline int
__zos_sys_exit_group (int *errcode __attribute__ ((unused)), int status)
{
  /* Somewhat confusingly, our _exit() act's like linux's exit_group().

     TODO: what happens on linux when a thread invokes _exit?
     What happens to other threads in the process? Emulate this.
     TODO: We need to figure out how to emulate exit_group().
     TODO: The current syscall handling macros don't allow void syscalls,
     so extra code is being generated.  */

  status = (status & 0xff) << 8;

  BPX_CALL (_exit, __bpx4exi_t, &status);

  /* If we did return, something went terribly wrong.  */
  return -1;
}


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
			     const char *pathname,
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
  char translated_path[__BPXK_PATH_MAX];
  /* TODO: this mask might be unnecessary. Linux allows extra bits
     to be set in chmod's mode. We haven't tested to see whether
     or not the bpx services do the same, so we mask to be safe.  */
  int32_t kernel_mode = mode & 0x0fff;
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (chmod, __bpx4chm_t, &path_len, translated_path, &kernel_mode,
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
			     const char *pathname,
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
  char translated_path[__BPXK_PATH_MAX];
  uint32_t owner_uid = owner, group_id = group;
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (chown, __bpx4cho_t, &path_len, translated_path, &owner_uid,
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
  uint32_t owner_uid = owner, group_id = group;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (lchown, __bpx4lco_t, &path_len, translated_path, &owner_uid,
	    &group_id, &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4chd_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4fcd_t) (const int32_t *dfd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_chdir (int *errcode, const char *path)
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (path, translated_path);

  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (chdir, __bpx4chd_t, &path_len, translated_path, &retval,
	    errcode, &reason_code);
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
			     const char *pathname,
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
  char translated_path[__BPXK_PATH_MAX];
  uint64_t file_length = length;
  uint32_t path_len = translate_and_check_size (path, translated_path);

  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  if (length < 0)
    {
      *errcode = EINVAL;
      return -1;
    }
  BPX_CALL (truncate, __bpx4tru_t, &path_len, translated_path,
	    &file_length, &retval, errcode, &reason_code);
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
			     const char *pathname,
			     uint32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_mkdir (int *errcode, const char *pathname, mode_t mode)
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t dmode = mode;
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (mkdir, __bpx4mkd_t, &path_len, translated_path, &dmode,
	    &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4rmd_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_rmdir (int *errcode, const char *pathname)
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (rmdir, __bpx4rmd_t, &path_len, translated_path, &retval,
	    errcode, &reason_code);
  return retval;
}

/* The getrlimit wrapper is in getrlimit64.c.  */
/* The getrusage wrapper is in getrusage.c.  */

typedef void (*__bpx4gcw_t) (const __bpxk_32_t *buf_len,
			     char *buf,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_getcwd (int *errcode, char *buf, size_t size)
{
  int32_t retval, reason_code;
  const int32_t buf_len = size <= INT32_MAX ? (int32_t) size : INT32_MAX;

  BPX_CALL (getcwd, __bpx4gcw_t, &buf_len, buf, &retval, errcode,
	    &reason_code);

  return retval;
}


typedef void (*__bpx4acc_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     const uint32_t *mode,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_access (int *errcode, const char *pathname, int mode)
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  /* The z/OS flags are effectively the same, so no mapping.
     z/OS TODO: Decide if we want to provide defines for the special
     access flags unique to z/OS.  */

  BPX_CALL (access, __bpx4acc_t, &path_len, translated_path, &mode,
	    &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4unl_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline int
__zos_sys_unlink (int *errcode, const char *pathname)
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (unlink, __bpx4unl_t, &path_len, translated_path,
	    &retval, errcode, &reason_code);

  return retval;
}


typedef void (*__bpx4fname_t) (const uint32_t *fname1_len,
			       const char *fname1,
			       const uint32_t *fname2_len,
			       const char *fname2,
			       int32_t *retval, int32_t *retcode,
			       int32_t *reason_code);

typedef __bpx4fname_t __bpx4lnk_t;
typedef __bpx4fname_t __bpx4ren_t;

static inline int
__zos_sys_link (int *errcode, const char *filename, const char *linkname)
{
  int32_t retval, reason_code;
  char translated_filename[__BPXK_PATH_MAX];
  char translated_linkname[__BPXK_PATH_MAX];

  uint32_t filename_len = translate_and_check_size (filename,
						    translated_filename);
  if (__glibc_unlikely (filename_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  uint32_t linkname_len = translate_and_check_size (linkname,
						    translated_linkname);
  if (__glibc_unlikely (linkname_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (link, __bpx4lnk_t, &filename_len, translated_filename,
	    &linkname_len, translated_linkname, &retval, errcode,
	    &reason_code);

  return retval;
}


static inline int
__zos_sys_rename (int *errcode, const char *oldname, const char *newname)
{
  int32_t retval, reason_code;
  char translated_oldname[__BPXK_PATH_MAX];
  char translated_newname[__BPXK_PATH_MAX];

  uint32_t oldname_len = translate_and_check_size (oldname,
						   translated_oldname);
  if (__glibc_unlikely (oldname_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  uint32_t newname_len = translate_and_check_size (newname,
						   translated_newname);
  if (__glibc_unlikely (newname_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (rename, __bpx4ren_t, &oldname_len, translated_oldname,
	    &newname_len, translated_newname, &retval, errcode,
	    &reason_code);

  return retval;
}


typedef void (*__bpx4mkn_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     const uint32_t *mode,
			     const uint32_t *dev,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_mknod (int *errcode, const char *pathname, mode_t mode,
		 dev_t dev)
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t mode_int = mode;
  uint32_t dev_int = dev;
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (mknod, __bpx4mkn_t, &path_len, translated_path,
	    &mode_int, &dev_int, &retval, errcode, &reason_code);

  return retval;
}


typedef void (*__bpx4pas_t) (int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_pause (int *errcode)
{
  int32_t retval, reason_code;
  BPX_CALL (pause, __bpx4pas_t, &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4wat_t) (const int32_t *pid,
			     const int32_t *options,
			     int32_t * const *status,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


static inline pid_t
__zos_sys_waitpid (int *errcode, pid_t pid, int *status, int options)
{
  int32_t retval, reason_code;
  int32_t pid_int = pid;
  BPX_CALL (wait, __bpx4wat_t, &pid_int, &options, &status,
	    &retval, errcode, &reason_code);
  return retval;
}


static inline pid_t
__zos_sys_wait (int *errcode, int *status)
{
  return __zos_sys_waitpid (errcode, -1, status, 0);
}


typedef void (*__bpx4exc_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     const uint32_t *arg_count,
			     const uint32_t *const *arg_len_list,
			     const uint32_t *const *arg_list,
			     const uint32_t *env_count,
			     const uint32_t *const *env_len_list,
			     const uint32_t *const *env_list,
			     void **exit_routine,
			     void **exit_routine_params,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_execve (int *errcode, const char *pathname, char *const argv[],
		  char *const envp[])
{
  int32_t retval, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  uint32_t path_len = translate_and_check_size (pathname,
						translated_path);
  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  uint64_t total_size;
  uint32_t argc, envc;
  uint32_t **arglen_ptrs, **envlen_ptrs, *arglens, *envlens;
  char **args, **envs;
  char *translated;
  void *lens;

  extern int printf(const char *, ...);
  /* Support argv == NULL or envp == NULL like linux does.  */
  char *const dummy[] = { NULL };
  if (argv == NULL)
    argv = dummy;
  if (envp == NULL)
    envp = dummy;

#define count_args(list, count)					\
  do {								\
    uint64_t _c;						\
    for (_c = 0; list[_c] && _c < UINT32_MAX; ++_c);		\
    if (_c == UINT32_MAX && list[_c] != NULL)			\
      {								\
	*errcode = E2BIG;					\
	return -1;						\
      }								\
    count = (uint32_t) _c;					\
  } while (0)
  count_args (argv, argc);
  count_args (envp, envc);
#undef count_args

  /* Structure of this allocation:
     -----------------------
     | arg length pointers |
     ----------------------- size: Max(argc,1) * sizeof(*)
     | env length pointers |
     ----------------------- size: Max(envc,1) * sizeof(*)
     | arg pointers        |
     ----------------------- size: Max(argc,1) * sizeof(*)
     | env pointers        |
     ----------------------- size: Max(envc,1) * sizeof(*)
     | arg lengths         |
     ----------------------- size: argc * sizeof (uint32_t)
     | env lengths         |
     ----------------------- size: envc * sizeof (uint32_t)
     Plus padding for storage obtain.  */

  uint32_t adj_argc = argc ?: 1, adj_envc = envc ?: 1;

  size_t eptrs_off  = (sizeof (*arglen_ptrs) * adj_argc);
  size_t args_off   = (sizeof (*envlen_ptrs) * adj_envc  + eptrs_off);
  size_t envs_off   = (sizeof (*args) * adj_argc + args_off);
  size_t arglen_off = (sizeof (*envs) * adj_envc + envs_off);
  size_t envlen_off = (sizeof (*arglens) * argc + arglen_off);
  size_t lens_total = (sizeof (*envlens) * envc +
		       envlen_off + 7UL) & ~7UL;

  lens = __storage_obtain_simple (lens_total);
  if (lens == NULL)
    {
      *errcode = ENOMEM;
      return -1;
    }

  arglen_ptrs = lens;
  envlen_ptrs = (uint32_t **) ((uintptr_t) lens + eptrs_off);
  args = (char **) ((uintptr_t) lens + args_off);
  envs = (char **) ((uintptr_t) lens + envs_off);
  arglens = (uint32_t *) ((uintptr_t) lens + arglen_off);
  envlens = (uint32_t *) ((uintptr_t) lens + envlen_off);

  total_size = 0;
#define count_lengths(list, count, lengths, len_ptrs)			\
  do {									\
    for (uint32_t i = 0; i < count; i++)				\
      {									\
	lengths[i] = strnlen (list[i], UINT32_MAX - 1) + 1;		\
	if (lengths[i] == UINT32_MAX && list[i][UINT32_MAX] != '\0')	\
	  {								\
	    /* z/OS TODO: free lens.  */				\
	    *errcode = E2BIG;						\
	    return -1;							\
	  }								\
	len_ptrs[i] = &lengths[i];					\
	total_size += lengths[i];					\
      }									\
    if (count == 0)							\
      len_ptrs[0] = NULL;						\
  } while (0)
  count_lengths (argv, argc, arglens, arglen_ptrs);
  count_lengths (envp, envc, envlens, envlen_ptrs);
#undef count_lengths

  translated = __storage_obtain_simple ((total_size + 7UL) & ~7UL);
  if (translated == NULL)
    {
      /* z/OS TODO: free lens.  */
      *errcode = ENOMEM;
      return -1;
    }

#define copy_and_translate(list, count, lengths, argptrs)		\
  do {									\
    for (uint32_t i = 0; i < count; i++)				\
      {									\
        argptrs[i] = translated;					\
	tr_e_until_len (list[i], translated, lengths[i]);		\
	translated += lengths[i];					\
      }									\
    if (count == 0)							\
      argptrs[0] = NULL;						\
  } while (0)
  copy_and_translate (argv, argc, arglens, args);
  copy_and_translate (envp, envc, envlens, envs);
#undef copy_and_translate

  /* z/OS TODO: We might want to define an exit to clean up any
     resources.  */
  void *exit_addr = NULL, *exit_params = NULL;

  BPX_CALL (exec, __bpx4exc_t, &path_len, translated_path, &argc,
	    arglen_ptrs, args, &envc, envlen_ptrs, envs, &exit_addr,
	    &exit_params, &retval, errcode, &reason_code);

  /* If we returned, something has gone wrong.  */
  /* z/OS TODO: do a storage release here when that's working.  */

  return retval;
}


typedef void (*__bpx4sym_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     const uint32_t *linkname_len,
			     const char *linkname,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_symlink (int *errcode, const char *from, const char *to)
{
  int32_t retval, reason_code;
  char tr_from[__BPXK_PATH_MAX], tr_to[__BPXK_PATH_MAX];
  uint32_t from_len, to_len;
  from_len = translate_and_check_size (from, tr_from);

  if (__glibc_unlikely (from_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  to_len = translate_and_check_size (to, tr_to);

  if (__glibc_unlikely (to_len == __BPXK_PATH_MAX))
    {
      *errcode = ENAMETOOLONG;
      return -1;
    }

  BPX_CALL (symlink, __bpx4sym_t, &from_len, tr_from, &to_len, tr_to,
	    &retval, errcode, &reason_code);

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


typedef void (*__bpx4frk_t) (pid_t *pid, int32_t *retcode,
			     int32_t *reason_code);


static inline pid_t
__zos_sys_fork (int *errcode)
{
  pid_t pid;
  int32_t reason_code;

  /* For some reason, fork unsets THLIccsid. We set it back up
     manually.  */
  uint16_t parent_ccsid = get_prog_ccsid ();

  BPX_CALL (fork, __bpx4frk_t, &pid, errcode, &reason_code);

  if (pid == 0)
    set_prog_ccsid (parent_ccsid);

  return pid;
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


typedef void (*__bpx4gep_t) (const int32_t *pid,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline pid_t
__zos_sys_getpgid (int *errcode, pid_t pid)
{
  int32_t retval, reason_code;
  int32_t pid_int = pid;
  BPX_CALL (getpgid, __bpx4gep_t, &pid_int,
	    &retval, errcode, &reason_code);
  return (pid_t)retval;
}


typedef void (*__bpx4gpi_t) (int32_t *ret_pid);

static inline pid_t
__zos_sys_getpid (int *errcode __attribute__ ((unused)))
{
  int32_t ret_pid;
  BPX_CALL (getpid, __bpx4gpi_t, &ret_pid);
  return (pid_t)ret_pid;
}


typedef void (*__bpx4gpp_t) (int32_t *ret_ppid);

static inline pid_t
__zos_sys_getppid (int *errcode __attribute__ ((unused)))
{
  int32_t ret_ppid;
  BPX_CALL (getppid, __bpx4gpp_t, &ret_ppid);
  return (pid_t)ret_ppid;
}


typedef void (*__bpx4ges_t) (const int32_t *pid,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline pid_t
__zos_sys_getsid (int *errcode, pid_t pid)
{
  int32_t retval, reason_code;
  int32_t pid_int = pid;
  BPX_CALL (getsid, __bpx4ges_t, &pid_int,
	    &retval, errcode, &reason_code);
  return (pid_t)retval;
}


typedef void (*__bpx4spg_t) (const int32_t *pid,
			     const int32_t *pgid,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_setpgid (int *errcode, pid_t pid, pid_t pgid)
{
  int32_t retval, reason_code;
  int32_t pid_int = pid;
  int32_t pgid_int = pgid;
  BPX_CALL (setpgid, __bpx4spg_t, &pid_int, &pgid_int,
	    &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4ssi_t) (int32_t *ret_sid, int32_t *retcode,
			     int32_t *reason_code);

static inline pid_t
__zos_sys_setsid (int *errcode)
{
  int32_t ret_sid, reason_code;
  BPX_CALL (setsid, __bpx4ssi_t,
	    &ret_sid, errcode, &reason_code);
  return (pid_t)ret_sid;
}


typedef void (*__bpx4alr_t) (const uint32_t *seconds,
			     uint32_t *ret_sec);

static inline unsigned int
__zos_sys_alarm (int *errcode __attribute__ ((unused)), unsigned int seconds)
{
  uint32_t ret_sec;
  BPX_CALL (alarm, __bpx4alr_t, &seconds, &ret_sec);
  return ret_sec;
}


/* The getpgrp syscall is implemented via __getpgid syscall in
   posix/getpgrp.c. */
/* The setpgrp syscall is implemented via __setpgid syscall in
   posix/setpgrp.c. */


/* Signal syscalls.  */

typedef void (*__bpx4spm_t) (const int32_t *how,
			     const uint64_t * const *new_sigmask,
			     uint64_t * const *old_sigmask,
			     const uint32_t *euid,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Documentation is somewhat unclear and might suggest that params 2 and
   3 are actually 31-bit pointers to 8-byte areas, not 64-bit
   pointers.
   TODO: IMPORTANT: Test this.  */
static inline int
__zos_sys_sigprocmask (int *errcode, int how, const sigset_t *set,
		       sigset_t *oset)
{
  int32_t retval, reason_code;
  /* Convert the linux-compatible sigset_t type used by glibc right now
     to the 64-bit value used by the kernel.  */
  /* TODO: DANGER: The exact wording of the documentation suggests that
     the sigprocmask mask parameters should be pointers to
     31-bit pointers to the actual mask, which doesn't make any
     sense. It also differs with how it states the sigaction mask params
     work.  */
  uint64_t sigset;
  uint64_t osigset = 0;
  uint64_t *in_ptr;
  uint64_t *out_ptr;

  if (set)
    {
      sigset = user_to_kern_sigset (set);
      in_ptr = set ? &sigset : NULL;
    }
  else
    in_ptr = NULL;

  out_ptr = oset ? &osigset : NULL;

  /* Map linux setprocmask flags to z/OS flags.  */
  switch (how)
    {
    case SIG_BLOCK:
      how = ZOS_SYS_SIG_BLOCK;
      break;
    case SIG_UNBLOCK:
      how = ZOS_SYS_SIG_UNBLOCK;
      break;
    case SIG_SETMASK:
      how = ZOS_SYS_SIG_SETMASK;
      break;
    }

  BPX_CALL (sigprocmask, __bpx4spm_t, &how, &in_ptr, &out_ptr,
	    &retval, errcode, &reason_code);

  if (oset)
    kern_to_user_sigset (oset, osigset);

  return retval;
}

/* The signal user data is described as being 4 bytes large but
   potentially occupying a doubleword.  */

typedef union
{
  char data[4];
  unsigned long full;
} __bpxk_sig_usr_data;

typedef void (*__bpx4sia_t) (const int32_t *sig,
			     const void **new_handler,
			     const uint64_t *new_sigmask,
			     const uint32_t *new_sigflags,
			     void **old_handler,
			     uint64_t *old_sigmask,
			     uint32_t *old_sigflags,
			     __bpxk_sig_usr_data *udata,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_sigaction (int *errcode, int sig, const struct sigaction *act,
		     struct sigaction *oact)
{
  int32_t retval, reason_code;
  __sighandler_t hand, ohand;
  __sighandler_t *handptr, *ohandptr = &ohand;
  uint64_t mask, omask;
  uint32_t flags, oflags;

  sig = user_to_kern_signo (sig);

  flags = 0;
  if (act)
    {
      hand = act->sa_handler;
      handptr = &hand;
      mask = user_to_kern_sigset (&act->sa_mask);
      if (act->sa_flags & SA_NOCLDSTOP)
	flags |= ZOS_SYS_SA_NOCLDSTOP;
      if (act->sa_flags & SA_NOCLDWAIT)
	flags |= ZOS_SYS_SA_NOCLDWAIT;
      if (act->sa_flags & SA_NODEFER)
	flags |= ZOS_SYS_SA_NODEFER;
      /* z/OS TODO: right now our approach to signal stacks makes this
	 flag meaningless.  */
      if (act->sa_flags & SA_ONSTACK)
	flags |= ZOS_SYS_SA_ONSTACK;
      if (act->sa_flags & SA_RESETHAND)
	flags |= ZOS_SYS_SA_RESETHAND;
      /* z/OS TODO: We might need to do something in the SIR for
	 this.  */
      if (act->sa_flags & SA_RESTART)
	flags |= ZOS_SYS_SA_RESTART;
      /* z/OS TODO: Does this flag actualy change anything at a kernel
	 level?  */
      if (act->sa_flags & SA_SIGINFO)
	flags |= ZOS_SYS_SA_SIGINFO;
    }
  else
    {
      handptr = NULL;
      mask = 0;
    }

  oflags = 0;
  omask = 0;
  if (oact)
    {
      ohand = oact->sa_handler;
      ohandptr = &ohand;
    }
  else
    ohandptr = NULL;

  /* z/OS TODO: How should we use the user data?  */
  __bpxk_sig_usr_data udata;
  udata.full = 0;

  BPX_CALL (sigaction, __bpx4sia_t, &sig, &handptr, &mask, &flags,
	    &ohandptr, &omask, &oflags, &udata, &retval, errcode,
	    &reason_code);

  if (oact)
    {
      kern_to_user_sigset (&oact->sa_mask, omask);

      oact->sa_handler = ohand;
      oact->sa_flags = 0;
      if (oflags & ZOS_SYS_SA_NOCLDSTOP)
	oact->sa_flags |= SA_NOCLDSTOP;
      if (oflags & ZOS_SYS_SA_NOCLDWAIT)
	oact->sa_flags |= SA_NOCLDWAIT;
      if (oflags & ZOS_SYS_SA_NODEFER)
	oact->sa_flags |= SA_NODEFER;
      /* z/OS TODO: right now our approach to signal stacks makes this
	 flag meaningless.  */
      if (oflags & ZOS_SYS_SA_ONSTACK)
	oact->sa_flags |= SA_ONSTACK;
      if (oflags & ZOS_SYS_SA_RESETHAND)
	oact->sa_flags |= SA_RESETHAND;
      /* z/OS TODO: We might need to do something in the SIR for
	 this.  */
      if (oflags & ZOS_SYS_SA_RESTART)
	oact->sa_flags |= SA_RESTART;
      /* z/OS TODO: Does this flag actualy change anything at a kernel
	 level?  */
      if (oflags & ZOS_SYS_SA_SIGINFO)
	oact->sa_flags |= SA_SIGINFO;
    }

  return retval;
}


/* Terminal syscalls.  */


typedef void (*__bpx4tga_t) (const int32_t *fd,
			     struct termios *tios,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcgetattr (int *errcode, int fd, struct termios *termios_p)
{
  int32_t retval, reason_code;
  BPX_CALL (tcgetattr, __bpx4tga_t, &fd, termios_p, &retval, errcode,
	    &reason_code);
  return retval;
}

typedef void (*__bpx4tsa_t) (const int32_t *fd,
			     const int32_t *actions,
			     struct termios *tios,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcsetattr (int *errcode, int fd, int acts,
		     const struct termios *termios_p)
{
  int32_t retval, reason_code;
  struct termios real_tios;
  const struct termios *real_tios_ptr;
  speed_t ispeed = termios_p->c_cflag & 0x00ff0000;

  /* Handle ispeed == 0 special case where ispeed should be set to
     ospeed.
     z/OS TODO: this approach seems wasteful.  */
  if (ispeed == 0)
    {
      memcpy (&real_tios, termios_p, sizeof (real_tios));
      real_tios.c_cflag &= ~0x00ff0000;
      real_tios.c_cflag |= (real_tios.c_cflag & CBAUD) >> 8;
      real_tios_ptr = &real_tios;
    }
  else
    real_tios_ptr = termios_p;

  BPX_CALL (tcsetattr, __bpx4tsa_t, &fd, &acts, real_tios_ptr, &retval,
	    errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4tfw_t) (const int32_t *fd,
			     const int32_t *actions,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcflow (int *errcode, int fd, int action)
{
  int32_t retval, reason_code;
  BPX_CALL (tcflow, __bpx4tfw_t, &fd, &action, &retval, errcode,
	    &reason_code);
  return retval;
}


typedef void (*__bpx4tfh_t) (const int32_t *fd,
			     const int32_t *queue_selector,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcflush (int *errcode, int fd, int queue_selector)
{
  int32_t retval, reason_code;
  BPX_CALL (tcflush, __bpx4tfh_t, &fd, &queue_selector, &retval, errcode,
	    &reason_code);
  return retval;
}


typedef void (*__bpx4tdr_t) (const int32_t *fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcdrain (int *errcode, int fd)
{
  int32_t retval, reason_code;
  BPX_CALL (tcdrain, __bpx4tdr_t, &fd, &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4tsb_t) (const int32_t *fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcsendbreak (int *errcode, int fd, int duration)
{
  int32_t retval, reason_code;
  /* z/OS TODO: The documentation suggests this doesn't do anything for
     ptys. Also, it doesn't say what units duration is in.  */
  BPX_CALL (tcsendbreak, __bpx4tsb_t, &fd, &duration, &retval, errcode,
	    &reason_code);
  return retval;
}


typedef void (*__bpx4tgp_t) (const int32_t *fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcgetpgrp (int *errcode, int fd)
{
  int32_t retval, reason_code;
  BPX_CALL (tcgetpgrp, __bpx4tgp_t, &fd, &retval, errcode, &reason_code);
  return retval;
}


typedef void (*__bpx4tsp_t) (const int32_t *fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_tcsetpgrp (int *errcode, int fd, pid_t pgrp_id)
{
  int32_t retval, reason_code;
  BPX_CALL (tcsetpgrp, __bpx4tsp_t, &fd, &pgrp_id, &retval, errcode,
	    &reason_code);
  return retval;
}


/* Socket/networking related syscalls.  */

typedef void (*__bpx4soc_t) (const int32_t *domain,
			     const int32_t *type,
			     const int32_t *protocol,
			     const int32_t *dimension,
			     int32_t vec[2],
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static inline int
__zos_sys_socket (int *errcode, int domain, int type, int protocol)
{
  int32_t retval, reason_code;
  int32_t socks[2];
  const int dim = 1;

  BPX_CALL (socket_pair, __bpx4soc_t, &domain, &type, &protocol, &dim,
	    socks, &retval, errcode, &reason_code);

  return retval ?: socks[0];
}


#endif /* _ZOS_DECL_H */
