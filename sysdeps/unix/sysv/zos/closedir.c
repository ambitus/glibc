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
#include <sysdep.h>


typedef void (*__bpx4cld_t) (const int32_t *dirfd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Close the directory stream DIRP.
   Return 0 if successful, -1 if not.  */
int
__closedir (DIR *dirp)
{
  int32_t fd;
  int32_t retval, reason_code;
  INTERNAL_SYSCALL_DECL (retcode);

  if (dirp == NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  /* Save directory file descriptor. */
  fd = dirp->fd;

#if IS_IN (libc)
  __libc_lock_fini (dirp->lock);
#endif

  /* Get size of the memory that was allocated in opendir() and free it. */
  size_t storage_size = (sizeof (DIR) + dirp->allocation + 7UL) & ~7UL;
  retval = __storage_release ((unsigned int) (uintptr_t) dirp,
			      (unsigned int) storage_size);
  if (retval == -1)
    {
      __set_errno (EFAULT);
      return -1;
    }

  /* Close directory with BPX4CLD z/OS callable service. */
  BPX_CALL (closedir, __bpx4cld_t, &fd,
	    &retval, &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));

  return retval;
}
weak_alias (__closedir, closedir)
