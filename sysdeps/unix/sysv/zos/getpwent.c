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

#include <pwd.h>
#include <errno.h>
#include <libc-lock.h>
#include <stdlib.h>

/* We need to protect the dynamic buffer handling.  */
__libc_lock_define_initialized (static, lock);

/* This points to the static buffer used.  */
libc_freeres_ptr (static char *buffer);


struct passwd *
getpwent (void)
{
  static size_t buffer_size;
  static struct passwd resbuf;
  struct passwd *result;
  int save;

  /* Get lock.  */
  __libc_lock_lock (lock);

  if (buffer == NULL)
    {
      buffer_size = NSS_BUFLEN_PASSWD;
      buffer = malloc (buffer_size);
    }

  while (buffer != NULL
	 && __getpwent_r (&resbuf, buffer, buffer_size, &result) == ERANGE)
    {
      char *new_buf;
      buffer_size *= 2;
      new_buf = realloc (buffer, buffer_size);
      if (new_buf == NULL)
	{
	  /* We are out of memory.  Free the current buffer so that the
	     process gets a chance for a normal termination.  */
	  int save2 = errno;
	  free (buffer);
	  __set_errno (save2);
	}
      buffer = new_buf;
    }

  if (buffer == NULL)
    result = NULL;

  save = errno;
  __libc_lock_unlock (lock);
  __set_errno (save);
  return result;
}
