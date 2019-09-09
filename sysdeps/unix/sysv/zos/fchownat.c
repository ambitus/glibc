/* Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <at-call.h>

/* Change the owner and group of FILE.  */
/* TODO: Finish fchownat. Remove the link warning when it's complete.  */
int
fchownat (int fd, const char *file, uid_t owner, gid_t group, int flag)
{
  if (file == NULL
      || (flag & ~(AT_SYMLINK_NOFOLLOW | AT_EMPTY_PATH)) != 0)
    {
      __set_errno (EINVAL);
      return -1;
    }

  if (fd < 0 && fd != AT_FDCWD)
    {
      __set_errno (EBADF);
      return -1;
    }

  if (file[0] == '\0' && (flag & AT_EMPTY_PATH))
    {
      if (fd == AT_FDCWD)
	/* TODO: Is this okay? "." can't be a symlink, right?  */
	return __chown (".", owner, group);
      return __fchown (fd, owner, group);
    }

  if (fd == AT_FDCWD || file[0] == '/')
    {
      if (flag & AT_SYMLINK_NOFOLLOW)
	return __lchown (file, owner, group);
      return __chown (file, owner, group);
    }

  return ZOS_AT_BODY (__chown (file, owner, group));
}
link_warning (fchownat, "fchownat is not fully implemented for z/OS.")
