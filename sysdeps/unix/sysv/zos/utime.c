/* utime -- Change access and modification times of file. z/OS version.
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
#include <utime.h>


/* Set the access and modification times of FILE to those given in TIMES.
   If TIMES is NULL, set them to the current time.  */
int
utime (const char *file, const struct utimbuf *times)
{
  return INLINE_SYSCALL_CALL (utime, file, times);
}
libc_hidden_def (utime)
