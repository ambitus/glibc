/* Special use of signals internally.  z/OS version.
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

#ifndef __INTERNAL_SIGNALS_H
# define __INTERNAL_SIGNALS_H

#include <signal.h>
#include <sigsetops.h>
#include <stdbool.h>

/* The signal used for asynchronous cancelation.  */
#define SIGCANCEL       __SIGRTMIN


/* Signal needed for the kernel-supported POSIX timer implementation.
   We can reuse the cancellation signal since we can distinguish
   cancellation from timer expirations.  */
#define SIGTIMER        SIGCANCEL


/* Signal used to implement the setuid et.al. functions.  */
#define SIGSETXID       (__SIGRTMIN + 1)


/* Return is sig is used internally.  */
static inline bool
__is_internal_signal (int sig)
{
  return false;
}


/* Remove internal glibc signal from the mask.  */
static inline void
__clear_internal_signals (sigset_t *set)
{
}


/* Block all signals, including internal glibc ones.  */
static inline int
__libc_signal_block_all (sigset_t *set)
{
  sigset_t allset;
  __sigfillset (&allset);
  /* z/OS TODO: This causes a nontrivial amount of code to be duplicated
     at every usage site. Avoid that, somehow.  */
  return INLINE_SYSCALL_CALL (sigprocmask, SIG_BLOCK, &allset, set);
}


/* Block all application signals (excluding internal glibc ones).  */
static inline int
__libc_signal_block_app (sigset_t *set)
{
  return __libc_signal_block_all (set);
}


/* Restore current process signal mask.  */
static inline int
__libc_signal_restore_set (const sigset_t *set)
{
  /* z/OS TODO: This causes a nontrivial amount of code to be duplicated
     at every usage site. Avoid that, somehow.  */
  return INLINE_SYSCALL_CALL (sigprocmask, SIG_SETMASK, set, NULL);
}

/* Used to communicate with signal handler.  */
extern struct xid_command *__xidcmd attribute_hidden;

#endif
