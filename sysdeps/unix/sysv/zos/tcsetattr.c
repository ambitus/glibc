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

/* Set the state of FD to *TERMIOS_P.  */
int
__tcsetattr (int fd, int optional_actions, const struct termios *termios_p)
{
  return INLINE_SYSCALL_CALL (tcsetattr, fd, optional_actions, termios_p);
}
weak_alias (__tcsetattr, tcsetattr)
libc_hidden_def (tcsetattr)
