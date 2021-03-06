/* Signal Interface Routine declarations for z/OS.
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

/* z/OS TODO: make sure symbols are properly hidden.  */

#ifndef _ZOS_SIR_H
#define _ZOS_SIR_H 1

#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <zos-core.h>
#include <bpxk-constants.h>
#include <zos-syscall-base.h>

#define __sigflag(signo) ((uint64_t) 1 << (64 - (signo)))

/* Mask of signals with default action terminate.  */
#define TERMINATE_SIGS							\
  (__sigflag (ZOS_SYS_SIGHUP) | __sigflag (ZOS_SYS_SIGINT)		\
   | __sigflag (ZOS_SYS_SIGABRT) | __sigflag (ZOS_SYS_SIGILL)		\
   | __sigflag (ZOS_SYS_SIGPOLL) | __sigflag (ZOS_SYS_SIGFPE)		\
   | __sigflag (ZOS_SYS_SIGKILL) | __sigflag (ZOS_SYS_SIGBUS)		\
   | __sigflag (ZOS_SYS_SIGSEGV) | __sigflag (ZOS_SYS_SIGSYS)		\
   | __sigflag (ZOS_SYS_SIGPIPE) | __sigflag (ZOS_SYS_SIGALRM)		\
   | __sigflag (ZOS_SYS_SIGTERM) | __sigflag (ZOS_SYS_SIGUSR1)		\
   | __sigflag (ZOS_SYS_SIGUSR2) | __sigflag (ZOS_SYS_SIGABND)		\
   | __sigflag (ZOS_SYS_SIGQUIT) | __sigflag (ZOS_SYS_SIGTRAP)		\
   | __sigflag (ZOS_SYS_SIGXCPU) | __sigflag (ZOS_SYS_SIGXFSZ)		\
   | __sigflag (ZOS_SYS_SIGVTALRM) | __sigflag (ZOS_SYS_SIGPROF)	\
   | __sigflag (ZOS_SYS_SIGDANGER))

typedef void (*real_sighandler_t)(int signum, struct sigcontext *sc);

struct sir_stack
{
  /* We store the length of allocated signal stacks in their first
     8 bytes in case sig_stack_size changes in the middle of a run.  */
  uint64_t length;
  struct sigcontext ppsd;
};

struct sir_data
{
  volatile unsigned long run_counter;

  /* Keep track of how many times the ppsd gets overwritten before we can
     finish copying it. Read it immediately on entry, write it
     immediately when we finish copying.  */
  volatile unsigned long bad_runs;

  /* In an attempt to reduce the raciness (and improve the performance)
     of the implementation, we keep a single signal stack around.

     -1 = cached stack not in use.
     0 = cached stack not yet allocated.
     1 = cached stack in use.  */
  struct sir_stack *volatile cached_stack;
  volatile int cached_stack_in_use;
  volatile unsigned int new_stack_size;
};

struct sig_tdata
{
  bool in_sighandler;
  bool is_not_cached_stack;
  struct sir_stack *stack_start;
};

/* The assembly routine that recives control from signals and sets up for
   the main body of the Signal Interface Routine.  */
extern void __sir_entry (struct sigcontext *);

/* The main body of the Signal Interface Routine.  */
extern void __sir_body (struct sir_stack *, int);

/* A collection of data used in the SIR that must be global.  */
extern volatile struct sir_data __sir_data;

/* TLS variable to mark thread as currently executing a signal handler.
   Checked in longjmp to see whether or not it is jumping out of a
   signal handler.  */
extern __thread struct sig_tdata __sig_tdata attribute_tls_model_ie;

libc_hidden_proto (__sir_data)
libc_hidden_tls_proto (__sig_tdata)


/* This should not be a function, since it deallocates its own stack.  */
#define free_current_sig_stack()					\
  do {									\
    if (__sig_tdata.is_not_cached_stack)				\
      {									\
	struct sir_stack *stack = __sig_tdata.stack_start;		\
	__storage_release ((unsigned int) (uintptr_t) stack,		\
			   (unsigned int) stack->length);		\
      }									\
    else								\
      atomic_store_release (&__sir_data.cached_stack_in_use, -1);	\
  } while (0)

/* The user-facing signal numbers and the ones the kernel uses are
   slightly out of sync. We swap them around so that they more
   closely match the expected ones. 3 and 6 are the important ones,
   they should be QUIT and ABRT, respectively. Remapping those
   requires us to remap SIGURG and also the currently unused
   signal number 41 (to map 24 to and avoid duplicate signal
   number for SIGQUIT).  */

static inline int
user_to_kern_signo (int signo)
{
  switch (signo)
    {
    case SIGQUIT:
      return ZOS_SYS_SIGQUIT;
    case SIGABRT:
      return ZOS_SYS_SIGABRT;
    case SIGURG:
      return ZOS_SYS_SIGURG;
    case 24:
      return 41;
    default:
      return signo;
    }
  /* TODO: Add this to all the signal-involved syscalls.  */
}

static inline int
kern_to_user_signo (int signo)
{
  switch (signo)
    {
    case ZOS_SYS_SIGQUIT:
      return SIGQUIT;
    case ZOS_SYS_SIGABRT:
      return SIGABRT;
    case ZOS_SYS_SIGURG:
      return SIGURG;
    case 41:
      return 24;
    default:
      return signo;
    }
  /* TODO: Add this to all the signal-involved syscalls.  */
}

/* z/OS TODO: Should we bother reversing the bytes, instead of just
   changing the access functions accordingly? Only reason not to
   is to make sure nothing is trying to use RT signals.  */
#define __mapflg(tgt, in_signo, out_signo)				\
  do {									\
    _Static_assert (1 <= (in_signo) && (in_signo) <= 64			\
		    && 1 <= (out_signo) && (out_signo) <= 64,		\
		    "Bad signo.");					\
    uint64_t _in_flg = 1UL << ((in_signo) - 1);				\
    uint64_t _out_flg = 1UL << ((out_signo) - 1);			\
    if ((tgt) & _in_flg)						\
      {									\
	(tgt) &= ~_in_flg;						\
	(tgt) |= _out_flg;						\
      }									\
  } while (0);

static inline uint64_t
user_to_kern_sigset (const sigset_t *set)
{
  uint64_t kernel_set = set->__val[0];
  __mapflg (kernel_set, SIGQUIT, ZOS_SYS_SIGQUIT);
  __mapflg (kernel_set, SIGABRT, ZOS_SYS_SIGABRT);
  __mapflg (kernel_set, SIGURG, ZOS_SYS_SIGURG);
  __mapflg (kernel_set, 24, 41);

  /* z/OS TODO: avoid this by having sigaddset et al. understand
     the proper sigset format.  */
  uint64_t reversed = 0;
  for (int c = 0; c < 63; c++, kernel_set >>= 1, reversed <<= 1)
    reversed |= kernel_set & 0x1;
  return reversed;
}

static inline void
kern_to_user_sigset (sigset_t *oset, uint64_t kset)
{
  memset (oset, 0, sizeof (*oset));
  /* z/OS TODO: avoid this by having sigaddset et al. understand
     the proper sigset format.  */
  for (int c = 0; c < 63; c++, kset >>= 1, oset->__val[0] <<= 1)
    oset->__val[0] |= kset & 0x1;

  __mapflg (oset->__val[0], ZOS_SYS_SIGURG, SIGURG);
  __mapflg (oset->__val[0], ZOS_SYS_SIGABRT, SIGABRT);
  __mapflg (oset->__val[0], ZOS_SYS_SIGQUIT, SIGQUIT);
  __mapflg (oset->__val[0], 41, 24);
}
#undef __mapflg

typedef void (*__bpx4mss_t) (void (**sir_addr) (struct sigcontext *),
			     const uint64_t *user_data,
			     const uint64_t *override_sigset,
			     const uint64_t *terminate_sigset,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* This is the function we use to register the SIR during early
   initialization.  */

static inline void
set_up_signals (void)
{
  int32_t retval, retcode, reason_code;
  const uint64_t nothing = 0;
  void *sir = __sir_entry;

  /* Cause the SIR to recieve control when the default action is to be
     done for the following signals.  At the moment, we only run the
     SIR for the terminating signals, mostly just because unless we
     manually take care of process termination, the system sends
     messages to the joblog which can cause problems depending on how
     the environment is set up.  */
  uint64_t termsigs = TERMINATE_SIGS;

  /* Register the SIR.  */
  BPX_CALL (mvssigsetup, __bpx4mss, &sir, &nothing, &termsigs,
	    &termsigs, &retval, &retcode, &reason_code);
  /* z/OS TODO: Check for errors here.  */
}

#endif /* !_ZOS_SIR_H  */
