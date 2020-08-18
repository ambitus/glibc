/* clock_getres -- Get the resolution of a POSIX clockid_t.  z/OS version.
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
#include <errno.h>
#include <time.h>

#define SYSCALL_GETRES \
  res->tv_sec = 0;     \
  res->tv_nsec = 1;    \
  retval = 0;          \
  break

/* The REALTIME and MONOTONIC clock are definitely supported in the
   kernel.  */
#define SYSDEP_GETRES							\
  SYSDEP_GETRES_CPUTIME							\
  case CLOCK_REALTIME:							\
  case CLOCK_MONOTONIC:							\
    SYSCALL_GETRES

/* We handled the REALTIME clock here.  */
#define HANDLED_REALTIME	1
#define HANDLED_CPUTIME		1

#define SYSDEP_GETRES_CPUTIME       \
  case CLOCK_PROCESS_CPUTIME_ID:    \
  case CLOCK_THREAD_CPUTIME_ID:

#include <sysdeps/posix/clock_getres.c>
