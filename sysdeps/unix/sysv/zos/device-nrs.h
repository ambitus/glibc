/* Device numbers of devices used in the implementation.  z/OS version.
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

#ifndef _DEVICE_NRS_H
#define _DEVICE_NRS_H	1

#include <sys/sysmacros.h>

/* /dev/null is (4,0).  */
#define DEV_NULL_MAJOR	4
#define DEV_NULL_MINOR	0

/* /dev/full does not exist.  */
#undef DEV_FULL_MAJOR
#undef DEV_FULL_MINOR
#define __GLIBC_NO_DEVFULL 1

/* /dev/zero is (4,1).  */
#define DEV_ZERO_MAJOR	4
#define DEV_ZERO_MINOR	1

/* Pseudo tty slaves.

   z/OS only really has the old BSD-style ttys, not the Unix98 ones used
   preferentially on Linux.

   We define the LOW_MAJOR and HIGH_MAJOR macros for compatibility with
   other glibc ports.

   TODO: Do we need to check both masters and slaves, or just one of them?
   We are checking for both at the moment, which might not be correct. I
   think we just need to check slaves.  */
#define DEV_TTY_LOW_MAJOR	1
#define DEV_TTY_HIGH_MAJOR	2

/* Test whether given device is a tty.  */
#define DEV_TTY_P(statp) \
  ({ int __dev_major = __gnu_dev_major ((statp)->st_rdev);		      \
     __dev_major == DEV_TTY_LOW_MAJOR || __dev_major == DEV_TTY_HIGH_MAJOR; })

#endif	/* device-nrs.h */
