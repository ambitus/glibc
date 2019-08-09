/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sysdep.h>

/* z/OS TODO: We can make it so the termios structure used in the
   kernel is not the same as we use in the libc. See termios.h for
   why we might do that.  */

/* Put the state of FD into *TERMIOS_P.  */
int
__tcgetattr (int fd, struct termios *termios_p)
{
  return INLINE_SYSCALL_CALL (tcgetattr, fd, termios_p);
}

libc_hidden_def (__tcgetattr)
weak_alias (__tcgetattr, tcgetattr)
