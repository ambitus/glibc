/* Generate a temporary file descriptor.  z/OS version.
   Copyright (C) 2018 Rocket Software
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

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* TODO: this whole file. We need to figure out a way to emulate
   O_TMPFILE safely and securely. See linux impl for reference.  */
int
__gen_tempfd (int flags)
{
  /* dummy out for now. */
  return 0;
}
libc_hidden_def (__gen_tempfd)
