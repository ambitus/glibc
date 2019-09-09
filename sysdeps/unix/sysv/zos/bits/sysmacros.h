/* Definitions of macros to access `dev_t' values.  z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_SYSMACROS_H
#define _BITS_SYSMACROS_H 1

#ifndef _SYS_SYSMACROS_H
# error "Never include <bits/sysmacros.h> directly; use <sys/sysmacros.h> instead."
#endif

/* TODO: The dev_t we get from the bpx stat call is 32-bits, so for ease
   of implementation, we define dev_t to be 32-bits at the moment.
   However, dev_t in glibc on all other platforms is a 64-bit quantity,
   encoded strangely (see <bits/sysmacros.h> for the full description).

   If this turns out to be a meaningful incompatibility, define a
   translation function between kernel stat and userspace stat.  */

#define __SYSMACROS_DECLARE_MAJOR(DECL_TEMPL)			\
  DECL_TEMPL(unsigned int, major, (__dev_t __dev))

#define __SYSMACROS_DEFINE_MAJOR(DECL_TEMPL)			\
  __SYSMACROS_DECLARE_MAJOR (DECL_TEMPL)			\
  {								\
    unsigned int __major;					\
    __major  = ((__dev & (__dev_t) 0xffff0000u) >> 16);		\
    return __major;						\
  }

#define __SYSMACROS_DECLARE_MINOR(DECL_TEMPL)			\
  DECL_TEMPL(unsigned int, minor, (__dev_t __dev))

#define __SYSMACROS_DEFINE_MINOR(DECL_TEMPL)			\
  __SYSMACROS_DECLARE_MINOR (DECL_TEMPL)			\
  {								\
    unsigned int __minor;					\
    __minor  = ((__dev & (__dev_t) 0x0000ffffu) >>  0);		\
    return __minor;						\
  }

#define __SYSMACROS_DECLARE_MAKEDEV(DECL_TEMPL)			\
  DECL_TEMPL(__dev_t, makedev, (unsigned int __major, unsigned int __minor))

#define __SYSMACROS_DEFINE_MAKEDEV(DECL_TEMPL)			\
  __SYSMACROS_DECLARE_MAKEDEV (DECL_TEMPL)			\
  {								\
    __dev_t __dev;						\
    __dev  = (((__dev_t) __major) << 16);			\
    __dev |= (((__dev_t) __minor) <<  0);			\
    return __dev;						\
  }

#endif /* bits/sysmacros.h */
