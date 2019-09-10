/* Operating system specific code for generic dynamic loader functions.  z/OS.
   Copyright (C) 2019 Free Software Foundation, Inc.
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

/* z/OS TODO: We will need to implement OS version checking at some
   point.  */

/* Use the linux implementation, without the linux OS version
   checking.  */

#include <sysdeps/unix/sysv/linux/dl-osinfo.h>
#undef DL_SYSDEP_OSCHECK
#define DL_SYSDEP_OSCHECK(FATAL)
