/* Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <at-call.h>

/* Create a directory named PATH relative to FD with protections MODE.  */
/* TODO: Finish mkdirat. Remove the link warning when it's complete.  */
int
mkdirat (int fd, const char *path, mode_t mode)
{
  /* z/OS has no mkdirat syscall, so we do our best to emulate one.  */
  struct stat64 st;

  if (fd == AT_FDCWD || path[0] == '/')
    return __mkdir (path, mode);

  if (path == NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  /* Check FD is associated with a directory.  */
  if (__fxstat64 (_STAT_VER, fd, &st) != 0)
    return -1;

  if (!S_ISDIR (st.st_mode))
    {
      __set_errno (ENOTDIR);
      return -1;
    }

  return ZOS_AT_BODY (__mkdir (path, mode));
}
link_warning (mkdirat, "mkdirat is not fully implemented for z/OS.")
