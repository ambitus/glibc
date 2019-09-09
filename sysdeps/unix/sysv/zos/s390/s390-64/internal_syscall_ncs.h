/* Definition of the INTERNAL_SYSCALL_NCS macro.
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
   <http://www.gnu.org/licenses/>.

   z/OS TODO: This is only here to get glibc working, its an absolute
   kludge. This whole mechanism might need to be reworked. The only user
   of this macro seems to be nptl, and it probably only requires a very
   small subset of sycalls. Of course, we might just end up dummying out
   the files/caller functions altogether.

   They seem to only be used to support the nptl SIGSETXID signal
   system, so only the _id syscalls need to be implemented.

   The setpgid, setfsuid, and setfsgid cases don't seem to ever get
   used.  */

#include <unimplemented.h>

#define __ZOS_NCS_NARGS_setpgid   2
#define __ZOS_NCS_NARGS_setreuid  2
#define __ZOS_NCS_NARGS_setregid  2
#define __ZOS_NCS_NARGS_setgroups 2
#define __ZOS_NCS_NARGS_setresuid 3
#define __ZOS_NCS_NARGS_setresgid 3
#define __ZOS_NCS_NARGS_setuid    1
#define __ZOS_NCS_NARGS_setgid    1
#define __ZOS_NCS_NARGS_setfsuid  1
#define __ZOS_NCS_NARGS_setfsgid  1

/* All of the arguments to these syscalls are integral, except for the
   second argument to setgroups(), which requires a cast.  */
#define __ZOS_NCS_MAYBE_CAST_setpgid
#define __ZOS_NCS_MAYBE_CAST_setreuid
#define __ZOS_NCS_MAYBE_CAST_setregid
#define __ZOS_NCS_MAYBE_CAST_setgroups (void *)
#define __ZOS_NCS_MAYBE_CAST_setresuid
#define __ZOS_NCS_MAYBE_CAST_setresgid
#define __ZOS_NCS_MAYBE_CAST_setuid
#define __ZOS_NCS_MAYBE_CAST_setgid
#define __ZOS_NCS_MAYBE_CAST_setfsuid
#define __ZOS_NCS_MAYBE_CAST_setfsgid

#define __ZOS_NCS_CAT(a, b) a##b

/* These calls will have at most 3 args and at least 1.  */
#define __ZOS_NCS_TRUNC_ARGS(name, args...)			\
  __ZOS_NCS_TRUNC_ARGS_X (name, __ZOS_NCS_NARGS_##name, args, 0, 0)
#define __ZOS_NCS_TRUNC_ARGS_X(name, num, a, b, c, rest...)	\
  __ZOS_NCS_CAT (__ZOS_NCS_TRUNC_ARGS, num)			\
    (a, (__ZOS_NCS_MAYBE_CAST_##name b), c)

#define __ZOS_NCS_TRUNC_ARGS1(a, b, c) a
#define __ZOS_NCS_TRUNC_ARGS2(a, b, c) a, b
#define __ZOS_NCS_TRUNC_ARGS3(a, b, c) a, b, c

#define __ZOS_NCS_CASE(name, err, args...)			\
  case __NR_##name: _ret =					\
    INTERNAL_SYSCALL (name, err, __ZOS_NCS_NARGS_##name,	\
		      __ZOS_NCS_TRUNC_ARGS (name, args));	\
  break;


#define INTERNAL_SYSCALL_NCS(no, err, nr, args...)		\
  ({								\
    long _ret;							\
    switch (no)							\
    {								\
    __ZOS_NCS_CASE (setpgid, err, args)				\
    __ZOS_NCS_CASE (setreuid, err, args)			\
    __ZOS_NCS_CASE (setregid, err, args)			\
    __ZOS_NCS_CASE (setgroups, err, args)			\
    __ZOS_NCS_CASE (setresuid, err, args)			\
    __ZOS_NCS_CASE (setresgid, err, args)			\
    __ZOS_NCS_CASE (setuid, err, args)				\
    __ZOS_NCS_CASE (setgid, err, args)				\
    __ZOS_NCS_CASE (setfsuid, err, args)			\
    __ZOS_NCS_CASE (setfsgid, err, args)			\
    default: _ret = -1;						\
      __GLIBC_ZOS_RUNTIME_UNIMPLEMENTED				\
	("unexpected setxid syscall");				\
      break; /* control should never reach here */		\
    }								\
    _ret; })
