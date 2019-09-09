/* Old-style Unix parameters and limits.  z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
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

#ifndef _SYS_PARAM_H
# error "Never use <bits/param.h> directly; include <sys/param.h> instead."
#endif

#include <bits/local_lim.h>

#ifndef EXEC_PAGESIZE
#define EXEC_PAGESIZE	4096
#endif

#define MAXHOSTNAMELEN	255

/* z/OS TODO: 24 would be more in line with what the kernel does, but 20
   is what the linux port uses.  */
#define	MAXSYMLINKS	20

/* z/OS TODO: Are these appropriate?  */
/* The following are not really correct but it is a value we used for a
   long time and which seems to be usable.  People should not use NOFILE
   and NCARGS anyway.  */
#define NOFILE		256
#define	NCARGS		131072
