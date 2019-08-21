/* Copyright (C) 2019 Rocket Software
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


/* Duplicate FD to FD2, closing the old FD2 and making FD2 be
   open the same file as FD is which setting flags according to
   FLAGS.  Return FD2 or -1.  */
int
__dup3 (int fd, int fd2, int flags)
{
  return INLINE_SYSCALL_CALL (dup3, fd, fd2, flags);
}
libc_hidden_def (__dup3)
weak_alias (__dup3, dup3)
