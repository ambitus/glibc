/* Copyright (C) 2018 Free Software Foundation, Inc.
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

/* Keep this in sync with setjmp/longjmp.c  */

#include <stddef.h>
#include <signal.h>
#include <jmpbuf-offsets.h>
#include <setjmpP.h>
#include <stackinfo.h>
#include <atomic.h>
#include <sir.h>

/* Set the signal mask to the one specified in ENV, and jump
   to the position specified in ENV, causing the setjmp
   call there to return VAL, or 1 if VAL is 0.

   For z/OS, since we can have multiple stacks, and we may need to
   allocate new stacks for every signal, we need to check whether we
   are in a signal handler in every longjmp, and if so unwind
   up it then deallocate the stack itself.

   TODO: sigaltstack  */

void
__libc_siglongjmp (sigjmp_buf env, int val)
{
  /* FIXME: This breaks for nested signal handlers (when jumping out of
     one signal handler into another), but POSIX allows that case to be
     undefined behavior.  */

  /* Check if we are currently in a signal handler.  */
  if (__sig_tdata.in_sighandler)
    {
      uintptr_t dst_frame, s_top, s_bottom;

      /* Check if we are jumping out of the signal handler.  */
      dst_frame = env[0].__jmpbuf[0].__gregs[__JB_GPR13];
      s_bottom = (uintptr_t) __sig_tdata.stack_start;
      s_top = s_bottom + __sig_tdata.stack_start->length;

      if (dst_frame > s_top || s_bottom > dst_frame)
	{
	  /* We are jumping outside the signal stack. Unwind it and
	     clean up.  */

	  /* TODO: Important: unwind. This is more annoying than it looks.  */

	  /* TODO: The actual stack free should be done as late as
	     possible to prevent the compiler from using the stack.  */
	  /* Free the stack if it was not the cached stack.  */
	  atomic_store_release (&__sig_tdata.in_sighandler, false);

	  free_current_sig_stack ();
	}
    }

  /* TODO: fix _longjmp_unwind to properly unwind.  */

  /* Perform any cleanups needed by the frames being unwound.  */
  _longjmp_unwind (env, val);

  if (env[0].__mask_was_saved)
    /* Restore the saved signal mask.  */
    (void) __sigprocmask (SIG_SETMASK,
			  (sigset_t *) &env[0].__saved_mask,
			  (sigset_t *) NULL);

  /* Call the machine-dependent function to restore machine state.  */
  __longjmp (env[0].__jmpbuf, val ?: 1);
}

#ifndef __libc_siglongjmp
# ifndef __libc_longjmp
/* __libc_longjmp is a private interface for cancellation implementation
   in libpthread.  */
strong_alias (__libc_siglongjmp, __libc_longjmp)
# endif
weak_alias (__libc_siglongjmp, _longjmp)
weak_alias (__libc_siglongjmp, longjmp)
weak_alias (__libc_siglongjmp, siglongjmp)
#endif
