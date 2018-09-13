/* Low-level locking access to futex facilities.  Linux version.
   Copyright (C) 2018 Rocket Software.
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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.

   This file is needed to work around a bug in the linux lowlevellock
   implementation that only gets exposed for us.  */

#ifndef _ZOS_LOWLEVELLOCK_FUTEX_H
#define _ZOS_LOWLEVELLOCK_FUTEX_H 1

#include_next <lowlevellock-futex.h>

#undef lll_futex_syscall
#define lll_futex_syscall(nargs, futexp, op, ...)			\
  ({									\
    INTERNAL_SYSCALL_DECL (__err);					\
    long int __ret = INTERNAL_SYSCALL (futex, __err, nargs,		\
				       (int *)futexp, op,		\
				       __VA_ARGS__);			\
    (__glibc_unlikely (INTERNAL_SYSCALL_ERROR_P (__ret, __err))		\
     ? -INTERNAL_SYSCALL_ERRNO (__ret, __err) : 0);			\
  })



#endif /* _ZOS_LOWLEVELLOCK_FUTEX_H */
