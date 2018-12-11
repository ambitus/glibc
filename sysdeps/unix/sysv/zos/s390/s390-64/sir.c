/* Signal interface routine for z/OS.
   Copyright (C) 2018 Rocket Software.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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

#include <atomic.h>
#include <signal.h>

/* TODO: we have many questions about what we need to do here:
     * Is the SIR responsible for blocking/ignoring signals?
       * Is it run for literally every single non-lost signal the
         program gets, and is charged with discriminating between which
	 ones should be made pending?
     * When can the SIR get invoked recursively?
     * Isn't there always a race between the time the SIR is given
       control entry and the time it finishes copying the PPSD?
     * Is the PPSD thread-local or is it shared between threads?
     * Can signals be delivered when inside of code that is in a
       different addressing mode? What PPSD format would I get then?
       What addressing mode would the SIR be in in that case?
     * How to force dump creation on signal-based termination (like
       linux)? Maybe just raise a SIGABRT?
     * What is a superkill?
     * SA_NOCLDSTOP, SA_ONSTACK, and SA_RESTART
     * Does the reg restore service restore ARs, FPRs, and VRs?
   linux research TODO:
     * What is return code for process terminated by signal?
     * does linux support the extra SIGFPE argument?
     * how is the signal mask restored after return or exit from a
       signal handler on arches that don't use sigreturn?
       * what about on arches that use sigreturn?
     * how does longjmp work with the above problem?
       * I think longjmp restores sigmask.
     * Does the kernel allocate space for sigaction's old_action, or
       is it the same one that was provided for the user?
   My TODO:
     * Hard to implement signal handlers without a stack pointer that
       is always current (unless using alternate stack). NAB must
       ALWAYS be accurate, at the instruction-by-instruction-level.
       Assume asyncronous reads.
       * Similarly r13 should NEVER be in an invalid state.
     * Increment NAB by sizeof (ppsd) + 8 * 2;
     * Copy PPSD to stack area (whichever one we are using).
     * Push sig handler arg(s) onto stack.
     * Set up floating-point environment.
     * Set r13 to top of stack area (after PPSD and args).
     * set r1 to args.
     * call signal handler
     * ONLY thing that should be done after this point in the SIR is
       restore the sig mask.
     * if signal handler returns, restore old signal mask,
       restore regs, return.
     * if signal handler exits via longjmp, longjmp restores sigmask
   My decisions to make:
   notes:
       STORAGE is effectively atomic. Cannot be interrupted by signals.
*/

/* Keep track of how many times the ppsd gets overwritten before we can
   finish copying it. Read it immediately on entry, write it immediately
   when we finish copying.  */
static unsigned long sir_run_counter = 0;
static unsigned long bad_sir_runs = 0;

/* In an attempt to reduce the raciness (and the performance) of the
   implementation, we keep a single signal stack around.  */

/* -1 = cached stack not in use.
   0 = cached stack not yet allocated.
   1 = cached stack in use.  */

static int cached_stack_in_use = 0;
static void *cached_stack = NULL;

/* TODO: this is a skeleton.  */

/* Believe it or not, it doesn't appear that this code can be made
   AS-Safe. The documentation seems to suggest that the kernel may
   overwrite the PPSD before we are finished copying it.  */

unsigned long sig_stack_size = 1024 * 1024;

void
__attribute__ ((noinline))
__zos_sir_body (struct sigcontext *sc)
{
  /* TODO:  */
}


/* TODO: A lot of this. Change regs, reserve r1 as ppsd pointer. Also
   hide sir_get_stack.  */
asm ("
.text
	.align 8
	.globl sir_get_stack
	.type sir_get_stack, @function

sir_get_stack:
	# Atomically increment the old value of the run conter and load
	# the incremented value.
	larl	%r1, sir_run_counter
	lghi	%r2, 1
	laag	%r4, %r2, 0(%r1)
	aghi	%r4, 1



	# Atomically mark the cached stack as in use and get the old
	# usage status. Use a CAS loop.
	lhi	%r1, 1
	lrl	%r2, cached_stack_in_use
0:
	larl	%r3, cached_stack_in_use
	cs	%r2, %r1,0(%r3)
	jne	0b
	# Note that cs is an implicit full fence.

	# get a pointer to the PPSD
	# TODO: this.
	lg %r1, 0(%r1)

	# If the cached stack wasn't in use, load its address. Otherwise,
	# jump to the part where we get a new one.
	cijhe	%r2, 0, 3f
	lgrl	%r3, cached_stack
	lgr	%r1, %r3

1:
	# Set up the stack by setting r13 to the start of the stack +
	# sizeof (ppsd), reserving space for the PPSD copy.
	aghi	%r3, 600
	lgr	%r13, %r3

	# Copy the PPSD.
	mvc	0(256,%r1), 0(%r5)
	mvc	256(256,%r1), 256(%r5)
	mvc	512(88,%r1), 512(%r5)

	# Load the value of the run counter and check if it has changed
	# between the time we incremented it and now. If it hasn't,
	# continue normal execution. If it has, then the # PPSD was
	# (probably*) invalidated. There's not a lot we can really do
	# besides end the program or try to continue.
	# TODO: get in touch with IBM to clarify the PPSD overwriting
	# behavior.
	lgrl	%r1, sir_run_counter
	cgrlh	%r1, %r4, 5f

2:
	# Zero prev stack frame addr to mark this as a signal frame.
	# TODO: signal frame debug info?
	lgfi	%r0, 0
	stg	%r0, 136(%r13)

	# call the main SIR routine written in C
	# TODO: The call here
	.word 0

	# TODO: If we get here, do a STORAGE RELEASE if
	# cached_stack_in_use was 1.

	# TODO: If cached_stack_in_use was 0 or -1, mark it as
	# released. Again, how do we get around longjmp problem?

	# TODO: do an MVS load 16 and return

3:
	# Do a STORAGE OBTAIN to get a stack of the expected size.
	# TODO: non common cached stack should not be IPT-owned
	# TODO: How are we going to handle freeing these?!?! We can't
	#       rely on the signal handler to return (we can longjmp out
	#       of it).
	# TODO: there is a serious memory leak here right now.

	# Load the allocation size.
	lrl	%r0, sig_stack_size

	# Load the flags for subpool 72, 31-bit real and virtual storage,
	# owned by the IPT.
	iilf	%r15, 67127408
	lrl	%r6, __ipt_zos_tcb
	sar	%a0, %r6

	# Look up pc num and do the call
	llgt	%r14, 16
	l	%r14, 772(%r14)
	l	%r14, 160(%r14)
	pc	0(%r14)

	# check return code
	# TODO: do something meaningful here, right now we pretty much
	# ignore it.
	cijlh	%r15, 0, 6b
	# Widen result
	llgfr	%r1, %r1

4:
	lgr	%r3, %r1

	# Check if the cached stack was previously unallocated.
	# If it was, set the cached stack pointer to the newly
	# allocated stack.
	cijlh	%r2, 0, 1b
	stgrl	%r1, cached_stack
	j	1b

5:
	larl	%r1, bad_sir_runs
	lghi	%r2, 1
	laag	%r1, %r2, 0(%r1)
	j	2b
6:
	lghi	%r1, 0
	j	4b
	.size	sir_get_stack, .-sir_get_stack
")
