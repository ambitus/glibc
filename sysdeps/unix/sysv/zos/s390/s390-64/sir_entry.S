/* z/OS Signal Interface Routine entry point.  64 bit version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2019.

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

#include <zos-core.h>
#include <sysdep.h>

/* The code in this file cannot be reliably written in C, because
   __sir_entry is called with a nonstandard calling convention, and
   without a valid stack.

   Believe it or not, it doesn't appear that this code can be made
   AS-Safe. The documentation seems to suggest that the kernel may
   overwrite the PPSD before we are finished copying it, and we don't
   seem to have any way of preventing that.  */

/* Inputs:
     r1: address of a PPSD member.  */

/* Register usage:
     r0, r11, r15: scratch
     r1: pointer to old ppsd
     r2: cached_stack_in_use
     r3: sir run counter value
     r4: sir run counter address
     r5: start of stack once obtained / struct sir_stack (new ppsd) ptr
     r7: PPSD pointer
     r13: The DSA pointer, once we initialize it  */

	.text
ENTRY (__sir_entry)
	/* z/OS TODO: Is this necessary?  */
	sam64
	/* Atomically increment the old value of the run counter and load
	   the incremented value.  */
	larl	%r4, __sir_data + 0
	lgr	%r11, %r4
	lg	%r15, 0(%r4)
0:
	lgr	%r3, %r15
	aghi	%r3, 1
	csg	%r15, %r3, 0(%r11)
	jne	0b

	/* Atomically mark the cached stack as in use and get the old
	   usage status. Use a CAS loop.  */
	larl	%r15, __sir_data + 24
	lhi	%r0, 1
	l	%r2, 0(%r15)
0:
	cs	%r2, %r0, 0(%r15)
	jne	0b
	/* Note that cs and csg are implicit full fences.  */


	/* get a pointer to the PPSD  */
	llgt	%r7, 0(%r1)


	/* If the cached stack wasn't in use (i.e. if it was -1), load
	   its address. Otherwise, jump to the part where we get a new
	   one.  */
	ltr	%r2, %r2
	jnl	.Lalloc_new_sig_stack
	larl	%r5, __sir_data + 16
	lg	%r5, 0(%r5)

.Lcopy_ppsd:
	/* Set up the stack by setting r13 to the start of the stack +
	   sizeof (ppsd) + 8 (for stack legnth), reserving space for the
	   PPSD copy.  */
	lgr	%r15, %r5
	aghi	%r15, 608
	lgr	%r13, %r15

	/* Set up F4SA eyecatcher.  */
	llilh	%r15, 50932
	oill	%r15, 58049
	st	%r15, 4(%r13)

	/* Copy the PPSD.  */
	mvc	8 + 0(256,%r5), 0(%r7)
	mvc	8 + 256(256,%r5), 256(%r7)
	mvc	8 + 512(88,%r5), 512(%r7)

	/* Load the value of the run counter and check if it has changed
	   between the time we incremented it and now. If it hasn't,
	   continue normal execution. If it has, then the PPSD was
	   (probably*) invalidated. There's not a lot we can really do
	   besides end the program or try to continue.
	   TODO: get in touch with IBM to clarify the PPSD overwriting
	   behavior.  */

	/* load sir run counter  */
	lg	%r15, 0(%r4)

	/* compare against the old value of sir run counter  */
	cgr	%r15, %r3
	jne	.Linc_bad_runs

.Ljump_to_sir_body:
	/* Zero prev stack frame addr to mark this as a signal frame.
	   TODO: signal frame debug info?  */
	sgr	%r0, %r0
	stg	%r0, 128(%r13)

	/* call the main SIR routine written in C  */

	/* Set up NAB  */
	la	%r0, 168(%r13)
	stg	%r0, 136(%r13)

	/* Set up a 2-element parameter list with a pointer to the
	   PPSD  */
	la	%r1, 152(%r13)

	/* param 1: sir stack pointer  */
	stg	%r5, 152(%r13)

	/* param 2: 1 if the stack should be freed, 0 otherwise
	   cached_stack_in_use = -1, 0 -> 0, 1 -> 1  */
	ahi	%r2, 1
	srl	%r2, 1		/* (%r2 + 1) >> 1 does what we want  */
	st	%r2, 164(%r13)		/* old cached_stack_in_use  */

	/* Call into the C body of the SIR, which handles
	   everything else that needs to be done. This call does not
	   return.  */
	brasl	%r14, __sir_body

.Lalloc_new_sig_stack:
	/* Do a STORAGE OBTAIN to get a stack of the expected size.
	   TODO: non common cached stack should not be IPT-owned
	   TODO: How are we going to handle freeing these?!?! We can't
		 rely on the signal handler to return (we can longjmp out
		 of it).
	   TODO: there is a serious memory leak here right now.	 */
	/* Load the allocation size.  */
	larl	%r15, __sir_data + 28
	l	%r0, 0(%r15)
	llgfr	%r8, %r0

	/* Load the IPT Task Control Block  */
	/* larl	%r15, __ipt_zos_tcb
	l	%r6, 0(%r15) */

	/* Load the flags for subpool 72, 31-bit real and virtual storage,
	   owned by the IPT.  */
	larl	%r15, .Lsir_const_pool
	l	%r15, .Lstorage_flags-.Lsir_const_pool(%r15)
	/*sar	%a0, %r6*/

	/* Look up pc num and do the call  */
	llgt  %r14, 16
	l     %r14, 772(%r14)
	l     %r14, 160(%r14)
	pc    0(%r14)

	/* This clobbers r1  */

	/* check return code
	   TODO: Do something meaningful here, right now we pretty much
	   ignore it.  */
	ltr	%r15, %r15
	jnz	.Lstorage_failed

	/* Widen result  */
	llgfr	%r5, %r1

	/* Store the size of the allocation into the first 8 bytes of
	   the stack.  */
	stg	%r8, 0(%r5)

.Lcheck_cache_unallocated:
	/* Check if the cached stack had not yet been allocated.
	   If it was, set the cached stack pointer to the newly
	   allocated stack.

	   Check whether cached_stack_in_use was 0. */
	ltr	%r2, %r2
	jne	.Lcopy_ppsd

	larl	%r6, __sir_data + 16
	stg	%r5, 0(%r6)
	j	.Lcopy_ppsd

.Linc_bad_runs:
	/* Atomically increment the variable that we use to keep track of
	   how many bad runs we have had (for debugging).  */
	larl	%r11, __sir_data + 8
	lg	%r0, 0(%r11)
0:
	lgr	%r15, %r0
	aghi	%r15, 1
	csg	%r0, %r15, 0(%r11)
	je	.Ljump_to_sir_body
	j	0b

.Lstorage_failed:
	lghi	%r5, 0
	j	.Lcheck_cache_unallocated

	.section .rodata
	.align 4
.Lsir_const_pool:
.Lstorage_flags:
	/*.long	67127408  */
	.long REGULAR_OBTAIN_FLAGS  /* Flags for STORAGE OBTAIN.  */

	.text
END (__sir_entry)
hidden_def (__sir_entry)
