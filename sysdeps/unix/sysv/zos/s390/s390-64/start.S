/* Startup code for z/OS program objects.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com> and
   Michael Colavita <mcolavita@rocketsoftware.com>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file with other
   programs, and to distribute those programs without any restriction
   coming from the use of this file. (The GNU Lesser General Public
   License restrictions do apply in other respects; for example, they
   cover modification of the file, and distribution when not linked
   into another program.)

   Note that people who make modified versions of this file are not
   obligated to grant this special exception for their modified
   versions; it is their choice whether to do so. The GNU Lesser
   General Public License gives permission to release a modified
   version without this exception; this exception also makes it
   possible to release a modified version which carries forward this
   exception.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* NOTE: This is different from a normal start.S, because our loader
   doesn't give control to the dynamic loader. We need to check for
   PT_INTERP ourselves, and if we find it, load the dynamic linker
   and transfer control.  */

#include <zos-core.h>
#include <sysdep.h>

/*
   This is the canonical entry point, usually the first thing in the text
   segment. Most registers' values are unspecified, except for:

   %r1		Pointer to a structure that contains the program
		arguments and environment in the kernel's format.
   %r4		rtld_fini
   %r12		?
		z/OS TODO: Anything interesting?
   %r13		Pointer to a DSA.
   %r14		Return address. Technically we should branch here on
		exit, however I'm not sure how necessary that is.
		z/OS TODO: Check how necessary that is. Looks like ret
		addr is a stub that sets %r15 to retcode then does an
		SVC 3, at least for exec()ed programs.
   %r15		?
*/

	.text
	.align 8  /* TODO: confirm this.  */
	.globl _start
	.type _start,@function
_start:
	cfi_startproc
	cfi_undefined (r14)
#ifdef PIC
	/* This is the entry point the dynamic loader sees and calls.
	   The dynamic loader sets up a stack for us and preprocesses
	   our args and envs.  */
	j	.Lskip_setup
.Los_start:
	/* This is the entry point the OS sees and gives control to.
           NOTE: This must be EXACTLY 4 bytes after _start.  */
#else
	/* Static applications get NULL for rtld_fini.  */
	lghi	%r4, 0
#endif

	/* Prevent the value we got in %r1 from being clobbered
	   by the storage call.  */
	lgr	%r3, %r1	# arg_info

	/* Obtain storage

	   z/OS TODO: This can only allocate storage below the bar right
	   now. There's no reason why we shouldn't be able to put our
	   stack above the bar, so we should use a different memory
	   allocation facility.

	   z/OS TODO: Will this get reclaimed by the OS without an
	   explicit release? What about if we exit early? What about if
	   we abend? We need to be 100% certain that whatever allocation
	   method we use the operating system will reclaim our unfreed
	   resources whenever a process ends.  */
	larl	%r15, .Lmain_stack_length

	/* Load storage amount into r0 and flags into r15.  */
	l	%r0, 0(%r15)
	l	%r15, .Lmain_stack_flags-.Lmain_stack_length(%r15)
	llgt	%r14, 16
	l	%r14, 772(%r14)
	l	%r14, 160(%r14)
	pc	0(%r14)
	/* r15 has the return code and r1 has the address.  */
	/* z/OS TODO: Check if the call failed here.  */

	/* z/OS TODO: ensure 16-byte stack alignment. They do it on the
	   other s390 port so presumably there's a reason.  */

	/* Store address of our next DSA in old DSA.  */
	stg	%r1, 136(%r13)
	/* Store address of old DSA in our next DSA.  */
	stg	%r13, 128(%r1)
	/* Swap DSAs.  */
	lgr	%r13, %r1

	/* Set up F4SA sig  */
	llilh	%r14, 50932
	oill	%r14, 58049
	st	%r14, 4(%r13)

	/* Initialize NAB  */
	la	%r14, 200(%r13)
	stg	%r14, 136(%r13)

#ifdef PIC
	/* Call a helper to load the linker (avoiding the PLT).  */
	brasl	%r14, __GI___load_pt_interp

	/* Call the linker. Pass in our original r1.  */
	lgr	%r1, %r3
	basr	%r14, %r15

	/* We should not return, trap if we do.  */
	.long	0
#endif

	/* Set up args for __libc_start_main. Our __libc_start_main
	   is not the generic one, and it takes slightly different
	   args.  */

	/* __libc_start_main (main, arg_info, rtld_fini, init, fini,
			      stack_end)  */

#ifdef PIC
.Lskip_setup:
	/* Our args and envs have been processed properly.  */
	larl	%r2, main@GOTENT
	lg	%r2,0(%r2)
#else
	larl	%r2, main		# main
#endif
	larl	%r5, __libc_csu_init	# init
	larl	%r6, __libc_csu_fini	# fini
	lgr	%r7, %r13		# stack_end
	stmg	%r2, %r7, 152(%r13)	# Store params to arglist.

	la	%r1, 152(%r13)	/* load arg pointer into r1.  */

#ifdef PIC
	brasl	%r14, __libc_start_main@plt
#else
	brasl	%r14, __libc_start_main
#endif
	cfi_endproc

.Lmain_stack_length:
	.long 10 * (1 << 20)	/* 10 MiB  */
.Lmain_stack_flags:
	.long REGULAR_OBTAIN_FLAGS  /* Flags for STORAGE OBTAIN.  */

/* Define a symbol for the first piece of initialized data.  */
	.data
	.globl __data_start
__data_start:
	.long 0
	.weak data_start
	data_start = __data_start
