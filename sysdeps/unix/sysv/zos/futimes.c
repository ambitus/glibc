/* futimes -- change access and modification times of open file.  z/OS version.
   Copyright (C) 2019 Rocket Software
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

#include <sysdep.h>
#include <sys/time.h>


/* Change the access time of the file associated with FD to TVP[0] and
   the modification time of FILE to TVP[1]. */

int
__futimes (int fd, const struct timeval tvp[2])
{
  return INLINE_SYSCALL_CALL (futimes, fd, tvp);
}
weak_alias (__futimes, futimes)
