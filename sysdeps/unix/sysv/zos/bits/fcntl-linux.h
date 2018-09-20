/* override fcntl-linux.h in the linux port */


/* large swaths of this file are #if 0-ed out. They're just kept around
   to show what we are intentionally leaving out. They are mostly things
   that we cannot easily or efficiently provide replacements for. */

#ifndef	_FCNTL_H
# error "Never use <bits/fcntl-linux.h> directly; include <fcntl.h> instead."
#endif

#include <unimplemented.h>  /* remove this when the port is complete */

#ifdef __USE_GNU
# include <bits/types/struct_iovec.h>
#endif

/* open/fcntl.	*/
#define O_ACCMODE       0x0003
#define O_RDONLY        0x0002
#define O_WRONLY        0x0001
#define O_RDWR	        0x0003
#ifndef O_CREAT
# define O_CREAT        0x0080	/* Not fcntl.  */
#endif
#ifndef O_EXCL
# define O_EXCL	        0x0040	/* Not fcntl.  */
#endif
#ifndef O_NOCTTY
# define O_NOCTTY       0x0020	/* Not fcntl.  */
#endif
#ifndef O_TRUNC
# define O_TRUNC        0x0010	/* Not fcntl.  */
#endif
#ifndef O_APPEND
# define O_APPEND       0x0008
#endif
#ifndef O_NONBLOCK
# define O_NONBLOCK     0x0004
#endif
#ifndef O_NDELAY
# define O_NDELAY       O_NONBLOCK
#endif
#ifndef O_SYNC
# define O_SYNC	        0x0100
#endif
#define O_FSYNC		O_SYNC
#ifndef O_ASYNC
# define O_ASYNC        0x0200
#endif
#ifndef __O_LARGEFILE
# define __O_LARGEFILE  0x0400  /* this flag is ignored */
#endif

/* The following flag has no equivalent on linux */
#define O_NOLARGEFILE   0x0800

#ifndef __O_DIRECTORY
# define __O_DIRECTORY  __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
#endif
#ifndef __O_NOFOLLOW
# define __O_NOFOLLOW   __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
#endif
#ifndef __O_CLOEXEC
# define __O_CLOEXEC    __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
#endif
#ifndef __O_DIRECT
# define __O_DIRECT     __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
#endif
#ifndef __O_NOATIME
# define __O_NOATIME    __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
#endif
#ifndef __O_PATH
# define __O_PATH       __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
#endif
/* dummy this out by aliasing O_DSYNC to O_SYNC */
#ifndef __O_DSYNC
# define __O_DSYNC      O_SYNC
#endif
/* #ifndef __O_TMPFILE */
/* # define __O_TMPFILE    __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED */
/* #endif */

#ifndef F_GETLK
# define F_GETLK    5	/* Get record locking info.  */
# define F_SETLK    6	/* Set record locking info (non-blocking).  */
# define F_SETLKW   7	/* Set record locking info (blocking).	*/
#endif
#ifndef F_GETLK64
# define F_GETLK64	0	/* Get record locking info.  */
# define F_SETLK64	0	/* Set record locking info (non-blocking).  */
# define F_SETLKW64	0	/* Set record locking info (blocking).	*/
#endif

/* open file description locks.

   Usually record locks held by a process are released on *any* close and are
   not inherited across a fork.

   These cmd values will set locks that conflict with process-associated record
   locks, but are "owned" by the opened file description, not the process.
   This means that they are inherited across fork or clone with CLONE_FILES
   like BSD (flock) locks, and they are only released automatically when the
   last reference to the the file description against which they were acquired
   is put. */
#ifdef __USE_GNU
# define F_OFD_GETLK  0
# define F_OFD_SETLK	0
# define F_OFD_SETLKW	0
#endif

#ifdef __USE_LARGEFILE64
# define O_LARGEFILE __O_LARGEFILE
#endif

#ifdef __USE_XOPEN2K8
/* z/OS has no O_CLOEXEC, O_NOFOLLOW, O_DIRECTORY */
/* #define O_DIRECTORY  __O_DIRECTORY */  /* Must be a directory.  */
/* #define O_NOFOLLOW   __O_NOFOLLOW  */  /* Do not follow links.  */
/* #define O_CLOEXEC    __O_CLOEXEC   */  /* Set close_on_exec.    */
#endif

#ifdef __USE_GNU
# define O_DIRECT	__O_DIRECT	/* Direct disk access.	*/
# define O_NOATIME	__O_NOATIME	/* Do not set atime.  */
# define O_PATH		__O_PATH	/* Resolve pathname but do not open file.  */
/* # define O_TMPFILE	__O_TMPFILE */	/* Atomically create nameless file.  */
#endif

/* For now, Linux has no separate synchronicity options for read
   operations.	We define O_RSYNC therefore as the same as O_SYNC
   since this is a superset.  */
#if defined __USE_POSIX199309 || defined __USE_UNIX98
# define O_DSYNC	__O_DSYNC	/* Synchronize data.  */
# if defined __O_RSYNC
#  define O_RSYNC	__O_RSYNC	/* Synchronize read operations.	 */
# else
#  define O_RSYNC	O_SYNC		/* Synchronize read operations.	 */
# endif
#endif

/* Values for the second argument to `fcntl'.  */
#define F_DUPFD		0	/* Duplicate file descriptor.  */
#define F_GETFD		1	/* Get file descriptor flags.  */
#define F_SETFD		2	/* Set file descriptor flags.  */
#define F_GETFL		3	/* Get file status flags.  */
#define F_SETFL		4	/* Set file status flags.  */

#ifndef __F_SETOWN
# define __F_SETOWN   11
# define __F_GETOWN   10
#endif

#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
# define F_SETOWN	__F_SETOWN /* Get owner (process receiving SIGIO).  */
# define F_GETOWN	__F_GETOWN /* Set owner (process receiving SIGIO).  */
#endif

#ifndef __F_SETSIG
# define __F_SETSIG	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Set number of signal to be sent.  */
# define __F_GETSIG	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Get number of signal to be sent.  */
#endif
#ifndef __F_SETOWN_EX
# define __F_SETOWN_EX	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Get owner (thread receiving SIGIO).	*/
# define __F_GETOWN_EX	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Set owner (thread receiving SIGIO).	*/
#endif

#ifdef __USE_GNU
# define F_SETSIG	__F_SETSIG	/* Set number of signal to be sent.  */
# define F_GETSIG	__F_GETSIG	/* Get number of signal to be sent.  */
# define F_SETOWN_EX	__F_SETOWN_EX	/* Get owner (thread receiving SIGIO).	*/
# define F_GETOWN_EX	__F_GETOWN_EX	/* Set owner (thread receiving SIGIO).	*/
#endif

#ifdef __USE_GNU
# define F_SETLEASE	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Set a lease.	 */
# define F_GETLEASE	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Enquire what lease is active.  */
# define F_NOTIFY	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Request notifications on a directory.  */
# define F_SETPIPE_SZ	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Set pipe page size array.  */
# define F_GETPIPE_SZ	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Set pipe page size array.  */
# define F_ADD_SEALS	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Add seals to file.  */
# define F_GET_SEALS	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Get seals for file.	*/
/* Set / get write life time hints.  */
# define F_GET_RW_HINT	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
# define F_SET_RW_HINT	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
# define F_GET_FILE_RW_HINT	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
# define F_SET_FILE_RW_HINT	__GLIBC_ZOS_INTERNAL_UNIMPLEMENTED
#endif
#ifdef __USE_XOPEN2K8
# define F_DUPFD_CLOEXEC __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED	/* Duplicate file descriptor with
				   close-on-exit set.  */
#endif

/* For F_[GET|SET]FD.  */
#define FD_CLOEXEC	0x01000000
#define FD_CLOFORK  0x02000000

#ifndef F_RDLCK
/* For posix fcntl() and `l_type' field of a `struct flock' for lockf().  */
# define F_RDLCK		1	/* Read lock.  */
# define F_WRLCK		2	/* Write lock.	*/
# define F_UNLCK		3	/* Remove lock.	 */
#endif


#if 0
/* For old implementation of BSD flock.	 */
#ifndef F_EXLCK
# define F_EXLCK		4	/* or 3 */
# define F_SHLCK		8	/* or 4 */
#endif

#ifdef __USE_MISC
/* Operations for BSD flock, also used by the kernel implementation.  */
# define LOCK_SH	1	/* Shared lock.	 */
# define LOCK_EX	2	/* Exclusive lock.  */
# define LOCK_NB	4	/* Or'd with one of the above to prevent
				   blocking.  */
# define LOCK_UN	8	/* Remove lock.	 */
#endif

#ifdef __USE_GNU
# define LOCK_MAND	32	/* This is a mandatory flock:  */
# define LOCK_READ	64	/* ... which allows concurrent read operations.	 */
# define LOCK_WRITE	128	/* ... which allows concurrent write operations.  */
# define LOCK_RW	192	/* ... Which allows concurrent read & write operations.	 */
#endif

#ifdef __USE_GNU
/* Types of directory notifications that may be requested with F_NOTIFY.  */
# define DN_ACCESS	0x00000001	/* File accessed.  */
# define DN_MODIFY	0x00000002	/* File modified.  */
# define DN_CREATE	0x00000004	/* File created.  */
# define DN_DELETE	0x00000008	/* File removed.  */
# define DN_RENAME	0x00000010	/* File renamed.  */
# define DN_ATTRIB	0x00000020	/* File changed attributes.  */
# define DN_MULTISHOT	0x80000000	/* Don't remove notifier.  */
#endif
#endif /* 0 */

/* TODO: Below this point, nothing has been adapted for z/OS yet */

#ifdef __USE_GNU
/* Owner types.	 */
enum __pid_type
  {
    F_OWNER_TID = 0,		/* Kernel thread.  */
    F_OWNER_PID,		/* Process.  */
    F_OWNER_PGRP,		/* Process group.  */
    F_OWNER_GID = F_OWNER_PGRP	/* Alternative, obsolete name.	*/
  };

/* Structure to use with F_GETOWN_EX and F_SETOWN_EX.  */
struct f_owner_ex
  {
    enum __pid_type type;	/* Owner type of ID.  */
    __pid_t pid;		/* ID of owner.	 */
  };
#endif

#if 0
#ifdef __USE_GNU
/* Types of seals.  */
# define F_SEAL_SEAL	0x0001	/* Prevent further seals from being set.  */
# define F_SEAL_SHRINK	0x0002	/* Prevent file from shrinking.	 */
# define F_SEAL_GROW	0x0004	/* Prevent file from growing.  */
# define F_SEAL_WRITE	0x0008	/* Prevent writes.  */
#endif

#ifdef __USE_GNU
/* Hint values for F_{GET,SET}_RW_HINT.	 */
# define RWF_WRITE_LIFE_NOT_SET	0
# define RWH_WRITE_LIFE_NONE	1
# define RWH_WRITE_LIFE_SHORT	2
# define RWH_WRITE_LIFE_MEDIUM	3
# define RWH_WRITE_LIFE_LONG	4
# define RWH_WRITE_LIFE_EXTREME	5
#endif

/* Define some more compatibility macros to be backward compatible with
   BSD systems which did not managed to hide these kernel macros.  */
#ifdef	__USE_MISC
# define FAPPEND	O_APPEND
# define FFSYNC		O_FSYNC
# define FASYNC		O_ASYNC
# define FNONBLOCK	O_NONBLOCK
# define FNDELAY	O_NDELAY
#endif /* Use misc.  */

#ifndef __POSIX_FADV_DONTNEED
#  define __POSIX_FADV_DONTNEED	4
#  define __POSIX_FADV_NOREUSE	5
#endif
/* Advise to `posix_fadvise'.  */
#ifdef __USE_XOPEN2K
# define POSIX_FADV_NORMAL	0 /* No further special treatment.  */
# define POSIX_FADV_RANDOM	1 /* Expect random page references.  */
# define POSIX_FADV_SEQUENTIAL	2 /* Expect sequential page references.	 */
# define POSIX_FADV_WILLNEED	3 /* Will need these pages.  */
# define POSIX_FADV_DONTNEED	__POSIX_FADV_DONTNEED /* Don't need these pages.  */
# define POSIX_FADV_NOREUSE	__POSIX_FADV_NOREUSE /* Data will be accessed once.  */
#endif
#endif /* 0 */


#ifdef __USE_GNU
#if 0
/* Flags for SYNC_FILE_RANGE.  */
# define SYNC_FILE_RANGE_WAIT_BEFORE	1 /* Wait upon writeout of all pages
					     in the range before performing the
					     write.  */
# define SYNC_FILE_RANGE_WRITE		2 /* Initiate writeout of all those
					     dirty pages in the range which are
					     not presently under writeback.  */
# define SYNC_FILE_RANGE_WAIT_AFTER	4 /* Wait upon writeout of all pages in
					     the range after performing the
					     write.  */

/* Flags for SPLICE and VMSPLICE.  */
# define SPLICE_F_MOVE		1	/* Move pages instead of copying.  */
# define SPLICE_F_NONBLOCK	2	/* Don't block on the pipe splicing
					   (but we may still block on the fd
					   we splice from/to).	*/
# define SPLICE_F_MORE		4	/* Expect more data.  */
# define SPLICE_F_GIFT		8	/* Pages passed in are a gift.	*/
#endif /* 0 */

/* File handle structure.  */
struct file_handle
{
  unsigned int handle_bytes;
  int handle_type;
  /* File identifier.  */
  unsigned char f_handle[0];
};

/* Maximum handle size (for now).  */
# define MAX_HANDLE_SZ	128
#endif

__BEGIN_DECLS

#ifdef __USE_GNU

/* Provide kernel hint to read ahead.  */
extern __ssize_t readahead (int __fd, __off64_t __offset, size_t __count)
    __THROW;


/* Selective file content synch'ing.

   This function is a possible cancellation point and therefore not
   marked with __THROW.	 */
extern int sync_file_range (int __fd, __off64_t __offset, __off64_t __count,
			    unsigned int __flags);


/* Splice address range into a pipe.

   This function is a possible cancellation point and therefore not
   marked with __THROW.	 */
extern __ssize_t vmsplice (int __fdout, const struct iovec *__iov,
			   size_t __count, unsigned int __flags);

/* Splice two files together.

   This function is a possible cancellation point and therefore not
   marked with __THROW.	 */
extern __ssize_t splice (int __fdin, __off64_t *__offin, int __fdout,
			 __off64_t *__offout, size_t __len,
			 unsigned int __flags);

/* In-kernel implementation of tee for pipe buffers.

   This function is a possible cancellation point and therefore not
   marked with __THROW.	 */
extern __ssize_t tee (int __fdin, int __fdout, size_t __len,
		      unsigned int __flags);

/* Reserve storage for the data of the file associated with FD.

   This function is a possible cancellation point and therefore not
   marked with __THROW.	 */
# ifndef __USE_FILE_OFFSET64
extern int fallocate (int __fd, int __mode, __off_t __offset, __off_t __len);
# else
#  ifdef __REDIRECT
extern int __REDIRECT (fallocate, (int __fd, int __mode, __off64_t __offset,
				   __off64_t __len),
		       fallocate64);
#  else
#   define fallocate fallocate64
#  endif
# endif
# ifdef __USE_LARGEFILE64
extern int fallocate64 (int __fd, int __mode, __off64_t __offset,
			__off64_t __len);
# endif


/* Map file name to file handle.  */
extern int name_to_handle_at (int __dfd, const char *__name,
			      struct file_handle *__handle, int *__mnt_id,
			      int __flags) __THROW;

/* Open file using the file handle.

   This function is a possible cancellation point and therefore not
   marked with __THROW.	 */
extern int open_by_handle_at (int __mountdirfd, struct file_handle *__handle,
			      int __flags);

#endif	/* use GNU */

__END_DECLS
