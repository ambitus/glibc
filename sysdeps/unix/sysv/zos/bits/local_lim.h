/* Minimum guaranteed maximum values for system limits.  z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, see <http://www.gnu.org/licenses/>.  */

/* The maximum number of groups that any one user can have. */
#define NGROUPS_MAX    300

/* z/OS TODO: For whatever reason, MAX_CANON is configurable so we
   shouldn't define it, however we might just want to pretend it itsn't,
   since every other major system seems to use a flat limit.  */

/* z/OS TODO: For whatever reason, MAX_INPUT is configurable so we
   shouldn't define it, however we might just want to pretend it itsn't,
   since every other major system seems to use a flat limit.  */

/* z/OS TODO: For whatever reason, PATH_MAX is configurable so we
   shoundn't define it. However, too many things depend on it being
   available and set to a meaningful value. So in the interest of
   compatibility, we define it here anyway.  */
#define PATH_MAX	1024

/* z/OS TODO: For whatever reason, NAME_MAX is configurable so we
   shoundn't define it. However, too many things depend on it being
   available and set to a meaningful value. So in the interest of
   compatibility, we define it here anyway.  */
#define NAME_MAX	255

/* z/OS TODO: For whatever reason, PIPE_BUF is configurable so we
   shouldn't define it. It also seems like most code that actually uses
   it has a proper pathconf fallback.  */

/* The number of data keys per process.  */
#define _POSIX_THREAD_KEYS_MAX	128
/* This is the value this implementation supports.  */
#define PTHREAD_KEYS_MAX	1024

/* Controlling the iterations of destructors for thread-specific data.  */
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS	4
/* Number of iterations this implementation does.  */
#define PTHREAD_DESTRUCTOR_ITERATIONS	_POSIX_THREAD_DESTRUCTOR_ITERATIONS

/* The number of threads per process.  */
#define _POSIX_THREAD_THREADS_MAX	64
/* We have no predefined limit on the number of threads.  */
#undef PTHREAD_THREADS_MAX

/* z/OS TODO: Is AIO_PRIO_DELTA_MAX relevant to us?  */

/* Minimum size for a thread.  We are free to choose a reasonable value.  */
#define PTHREAD_STACK_MIN	16384

/* z/OS TODO: DELAYTIMER_MAX can't be relevant, our timers don't work
   like that.  */

/* Maximum tty name length.  */
#define TTY_NAME_MAX		9

/* Maximum login name length.  This is arbitrary.  */
#define LOGIN_NAME_MAX		9

/* z/OS TODO: We can configure max host name length? If so we shouldn't
   define HOST_NAME_MAX.  */

/* z/OS TODO: Do we have any equivalent to MQ_PRIO_MAX?  */

/* Maximum value the semaphore can have.  */
/* z/OS TODO: Semaphores are a problem, this value is absolutely
   miniscule. Furthermore, sem vals are unsigned, and adjustments
   are signed, so the max and min adjustments are different.  */
#define SEM_VALUE_MAX   (32767)
