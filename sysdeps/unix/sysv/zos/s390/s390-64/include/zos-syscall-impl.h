/* z/OS syscall wrapper prototypes.
   Copyright (C) 2018 Rocket Software
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
   <http://www.gnu.org/licenses/>.

   Each prototype should be followed by a corresponding hidden_proto().

   Prototypes should be __restrict qualified if appropriate. For some
   idea of what that means, look at include/stdio.h.  */

#include <bits/types.h>
#include <sys/cdefs.h>
#define __need_size_t
#include <stddef.h>

extern int __zos_sys_open (int *errcode, const char *pathname,
			   int flags, __mode_t mode);
hidden_proto (__zos_sys_open)

extern int __zos_sys_openat (int *errcode, int dirfd,
			     const char *pathname, int flags,
			     __mode_t mode);
hidden_proto (__zos_sys_openat)

extern __ssize_t __zos_sys_write (int *errcode, int fd, const void *buf,
				  size_t nbytes);
hidden_proto (__zos_sys_write)
