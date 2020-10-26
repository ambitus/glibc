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

#include <dirent.h>
#include <errno.h>
#include <dirstream.h>
#include <sysdep.h>


typedef void (*__bpx4rwd_t) (const int32_t *dirfd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


/* Rewind DIRP to the beginning of the directory.  */
void
__rewinddir (DIR *dirp)
{
  int32_t fd;
  int32_t retval, reason_code;
  INTERNAL_SYSCALL_DECL (retcode);

  /* Check for null pointer. */
  if (__glibc_unlikely (dirp == NULL))
    {
      __set_errno (EINVAL);
      return;
    }

  /* Save directory file descriptor. */
  fd = dirp->dfd;

#if IS_IN (libc)
  __libc_lock_lock (dirp->lock);
#endif

  /* Reset open directory to the beginning with BPX4RWD z/OS callable service. */
  BPX_CALL (rewinddir, __bpx4rwd_t, &fd,
	    &retval, &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));

  /* Clear state fields of directory stream. */
  dirp->filepos = 0;
  dirp->offset = 0;
  dirp->size = 0;
  dirp->errcode = 0;
#if IS_IN (libc)
  __libc_lock_unlock (dirp->lock);
#endif
}
libc_hidden_def (__rewinddir)
weak_alias (__rewinddir, rewinddir)
