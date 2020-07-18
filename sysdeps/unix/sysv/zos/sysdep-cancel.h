/* Single-thread optimization definitions.  z/OS version.
   Copyright (C) 2020 Free Software Foundation, Inc.

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

#include <sysdep.h>
#include <tls.h>
#include <nptl/pthreadP.h>

#include <sysdeps/unix/sysv/linux/sysdep-cancel.h>

#define BPX_CALL_CANCEL(...)				\
  do							\
    {							\
      if (SINGLE_THREAD_P)				\
	BPX_CALL (__VA_ARGS__);				\
      else						\
	{						\
	  int sc_cancel_oldtype = LIBC_CANCEL_ASYNC ();	\
	  BPX_CALL (__VA_ARGS__);			\
	  LIBC_CANCEL_RESET (sc_cancel_oldtype);	\
	}						\
    }							\
  while (0)
