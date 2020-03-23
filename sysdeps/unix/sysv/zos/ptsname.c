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

#include <errno.h>
#include <paths.h>

/* Static buffer for `ptsname'. Length of pty name (ptypNNNN, ttypNNNN)
   is 8 characters. */
static char buffer[sizeof(_PATH_DEV) + 8];


/* Return the pathname of the slave pseudo terminal associated with
   the master FD is open on, or NULL on errors.
   The returned storage is good until the next call to this function.  */
char *
ptsname (int fd)
{
  return __ptsname_r (fd, buffer, sizeof (buffer)) != 0 ? NULL : buffer;
}


/* Store at most BUFLEN characters of the pathname of the slave pseudo
   terminal associated with the master FD is open on in BUF.
   Return 0 on success, otherwise an error number.  */
int
__ptsname_r (int fd, char *buf, size_t buflen)
{
  int save_errno = errno;

  if (buf == NULL)
    {
      __set_errno (EINVAL);
      return EINVAL;
    }

  /* Test whether 'fd' file descriptor refers to a terminal. */
  if (!__isatty (fd))
    {
      __set_errno (ENOTTY);
      return ENOTTY;
    }

  /* Test whether buffer 'buf' has necessary size. */
  if (buflen < (sizeof (_PATH_DEV) + 8))
    {
      __set_errno (ERANGE);
      return ERANGE;
    }

  /* Get pathname of the master pseudo terminal associated
     with the master FD is open on. */
  if (__ttyname_r (fd, buf, buflen))
    return errno;

  /* Get pathname of the slave pseudo terminal - just replace first letter
     of the master pseudo terminal name 'p' with 't'. */
  buf[sizeof (_PATH_DEV) - 1] = 't';

  __set_errno (save_errno);
  return 0;
}
weak_alias (__ptsname_r, ptsname_r)
