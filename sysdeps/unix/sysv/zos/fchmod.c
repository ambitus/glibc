/* Copyright (C) 2018 Rocket Software
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
#include <sys/types.h>

/* Change the permissions of the file referenced by FD to MODE.  */
int
__fchmod (int fd, mode_t mode)
{
  return INLINE_SYSCALL_CALL (fchmod, fd, mode);
}
stub_warning (fchmod)

weak_alias (__fchmod, fchmod)
