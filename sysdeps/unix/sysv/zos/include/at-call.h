/* *at function helpers for z/OS.
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
   <http://www.gnu.org/licenses/>.  */

/* z/OS doesn't have *at syscalls (openat, mkdirat, fchmodat, etc.),
   so we need to emulate them.
   TODO: complete *at functionality implemntation.  */

#ifndef _ZOS_AT_CALL_H
#define _ZOS_AT_CALL_H 1

#include <sysdep-cancel.h>  /* for SINGLE_THREAD_P  */
#include <not-cancel.h>
#include <unimplemented.h>

#define ZOS_AT_BODY(base_call)						\
  ({									\
    if (SINGLE_THREAD_P)						\
      {									\
	/* This is not a perfect implementation. There are some		\
	   problems:							\
	    1. Race Conditions. A thread could be created between	\
	       the single thread check and the mkdir.			\
	    2. Mount points and chroots. fd could potentially point	\
	       to a directory outside of our current filesystem root	\
	       if the user did a chroot or switched into a different	\
	       mount namespace between the aquisition of the fd and	\
	       here.							\
	    3. If the second fchdir ever fails for whatever reason,	\
	       we leave the program in an invalid state that it		\
	       almost certainly will not be able to recover from.	\
	    4. Difficult to tell the difference between errors		\
	       stemming from the above problems and unrelated		\
	       problems.  */						\
	int res, cwdfd = __open_nocancel (".", O_RDONLY);		\
	/* If we can't chdir back into our original dir, fail.  */	\
	if (cwdfd != -1)						\
	  {								\
	    if (__fchdir (fd) == 0)					\
	      {								\
		res = (base_call);					\
		/* Hope that the following doesn't fail.  */		\
		if (__fchdir (cwdfd) == 0)				\
		  {							\
		    __close_nocancel (cwdfd);				\
		    return res;						\
		  }							\
	      }								\
	    __close_nocancel (cwdfd);					\
	  }								\
      }									\
    else								\
      {									\
	/* TODO: multithreaded case.  */				\
      }									\
									\
    __GLIBC_ZOS_RUNTIME_UNIMPLEMENTED("*at calls unimplemented");	\
    __set_errno (ENOSYS);						\
    -1;									\
  })

#endif /* _ZOS_AT_CALL_H  */
