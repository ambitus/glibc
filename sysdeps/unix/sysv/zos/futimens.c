/* Change access and modification times of open file.  z/OS version.
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

#include <stddef.h>
#include <sys/time.h>


/* Change the access time of the file associated with FD to TSP[0] and
   the modification time of FILE to TSP[1]. */
int
futimens (int fd, const struct timespec tsp[2])
{
  struct timeval tv[2];
  struct timeval *ptv = NULL;

  if (tsp != NULL)
    {
      ptv = tv;
      tv[0].tv_sec = tsp[0].tv_sec;
      tv[0].tv_usec = tsp[0].tv_nsec / 1000;
      tv[1].tv_sec = tsp[1].tv_sec;
      tv[1].tv_usec = tsp[1].tv_nsec / 1000;
    }

  return futimes(fd, ptv);
}
