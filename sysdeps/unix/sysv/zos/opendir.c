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
#include <stdio.h>	/* For BUFSIZ.  */
#include <sysdep.h>

#include <not-cancel.h>


typedef void (*__bpx4opd_t) (const uint32_t *pathname_len,
			     const char *pathname,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

typedef void (*__bpx4cld_t) (const int32_t *dirfd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);


/* Close the directory referred to by the open file descriptor.
   Return 0 if successful, -1 if not. */
static int
__closedir2 (int fd)
{
  int32_t dfd = fd;
  int32_t retval, reason_code;
  INTERNAL_SYSCALL_DECL (retcode);

  /* Close directory with BPX4CLD z/OS callable service. */
  BPX_CALL (closedir, __bpx4cld_t, &dfd,
	    &retval, &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));

  return retval;
}


/* Open a directory stream on NAME.  */
DIR *
__opendir (const char *name)
{
  int32_t retval, reason_code;
  INTERNAL_SYSCALL_DECL (retcode);
  char translated_path[__BPXK_PATH_MAX];

  /* Check for empty name. */
  if (__glibc_unlikely (name[0] == '\0'))
    {
      __set_errno (ENOENT);
      return NULL;
    }

  /* Translate directory name from ASCII to EBCDIC. */
  uint32_t path_len = translate_and_check_size (name,
						translated_path);

  /* Open directory with BPX4OPD z/OS callable service. */
  BPX_CALL (opendir, __bpx4opd_t, &path_len, translated_path,
	    &retval, &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    {
      __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));
      return NULL;
    }

  /* Save directory file descriptor that describes the specified directory. */
  int fd = retval;

  if (fd < 0)
    return NULL;

  /* Now we need to allocate necessary storage size for the DIR structure and
     for the buffer that is used to store directory entries. */

  /* Get the storage size for the buffer with directory entries.
     Use the same method as in 'sysdeps/posix/opendir.c' */
  size_t allocation = (4 * BUFSIZ < sizeof (struct dirent64)
		       ? sizeof (struct dirent64) : 4 * BUFSIZ);

  /* Get total storage size for DIR structure and for the internal buffer. */
  size_t storage_size = (sizeof (DIR) + allocation + 7UL) & ~7UL;
  /* Allocate storage. */
  DIR *dirp = (DIR *) __storage_obtain_simple (storage_size);
  if (dirp == NULL)
    {
      /* To close directory in case of error local __closedir2() function
       is used instead of __closedir() system call as we don't have DIR
       structure at the moment. */
      __closedir2(fd);
      __set_errno (ENOMEM);
      return NULL;
    }

  /* Init 'dirp' structure. */
  dirp->fd = fd;
#if IS_IN (libc)
  __libc_lock_init (dirp->lock);
#endif
  dirp->allocation = allocation;
  dirp->size = 0;
  dirp->offset = 0;
  dirp->filepos = 0;
  dirp->errcode = 0;

  return dirp;
}
weak_alias (__opendir, opendir)


#if IS_IN (libc)
/* z/OS TODO: Handle opendirat like the rest of the *at calls.  */
DIR *
__opendirat (int dfd, const char *name)
{
  __set_errno (ENOSYS);
  return NULL;
}
#endif
