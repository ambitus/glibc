/* Startup code for z/OS program objects.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Michael Colavita <mcolavita@rocketsoftware.com> and
   Giancarlo Frix <gfrix@rocketsoftware.com>.

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
   <http://www.gnu.org/licenses/>.

   TODO: this is all experimental right now.
   TODO: Make this work with shared stuff.  */


	.text
	.align 8  /* TODO: confirm this.  */
	.globl _start
	.type _start,@function
_start:
	/* cfi_startproc */	/* TODO: undo this when .eh_header works  */
	/* cfi_undefined (r14) */

	/* TODO: Figure out how to stop unwinding here.  */

	/* Save initial registers.  */
	stmg	%r14, %r12, 8(%r13)

	/* Obtain storage

	   TODO: This can only allocate storage below the bar right now.
	   There's no reason why we shouldn't be able to put our stack
	   above the bar, so we should use a different memory allocation
	   facility.

	   TODO: Will this get reclaimed by the OS without an explicit
	   release? What about if we exit early? What about if we abend?
	   We need to be 100% certain that whatever allocation method
	   we use the operating system will reclaim our unfreed
	   resources whenever a process ends.  */
	larl	%r15, 0f
	/* Load storage amount into r0.  */
	/* TODO: document this more.  */
	l	%r0, 4(%r15)
	l	%r15, 8(%r15)
	llgt	%r14, 16
	l	%r14, 772(%r14)
	l	%r14, 160(%r14)
	pc	0(%r14)
	/* TODO: Check if the call failed here.  */

	/* presumably, the top of our stack is in r13 */

	/* TODO: ensure 16-byte stack alignment. They do it on z/Linux so
	presumably there's a reason.  */

	/* Prepare main F4SA  */
	lgr	%r15, %r13
	lgr	%r13, %r1
	stg	%r15, 128(%r13)
	stg	%r13, 136(%r15)
	lg	%r1, 32(%r15)

	/* Set up F4SA sig  */
	llilh	%r14, 50932
	oill	%r14, 58049
	stg	%r14, 4(%r13)

	/* Initialize NAB  */
	lghi	%r14, 144
	algr	%r14, %r13
	stg	%r14, 136(%r13)

	/* Set up args for __libc_start_main */
	/* __libc_start_main (main, argc, argv, init, fini, rtld_fini,
	   stack_end) */

	/* TODO: Figure out where argc, argv, and envp are.  */

	larl	%r2, main
	stg	%r2, 152(%r13)

	llill	%r2, 0	/* TODO: this should be argc.  */
	stg	%r2, 160(%r13)
	llill	%r2, 0	/* TODO: this should be argv.  */
	stg	%r2, 168(%r13)

	larl	%r2, __libc_csu_init
	stg	%r2, 176(%r13)
	larl	%r2, __libc_csu_fini
	stg	%r2, 184(%r13)

	llill	%r2, 0	/* TODO: this should be rtld_fini.  */
	stg	%r2, 192(%r13)

	stg	%r13, 200(%r13)	/* stack_end  */

	la	%r1, 152(%r13)	/* load arg pointer into r1.  */

	/* TODO: ensure we are dubbed before calling main.  */
	brasl	%r14, __libc_start_main

	/* TODO: normal glibc start.S code doesn't return.  */

	/* Release storage  */
	lgr	%r1, %r13
	lg	%r13, 128(%r13)
	stg	%r15, 16(%r13)
	larl	%r15, 0f
	l	%r0, 4(%r15)
	lhi	%r15, 0x0003
	llgt	%r14, 16
	l	%r14, 772(%r14)
	l	%r14, 204(%r14)
	pc	0(%r14)

	/* Branch back to caller TODO: this needs work  */
	lg	%r15, 16(%r13)
	lg	%r14, 8(%r13)
	lmg	%r1, %r3, 32(%r13)
	/* TODO return value?  */
	br	%r14

	/* cfi_endproc */	/* TODO: see above.  */

0:
	/* stack size  */
	/* .long 0x001000a0 */	/* TODO: final val should be like 2-10 MB.  */
	.long 0x00010000	/* Testing stack size.  */
	.long 0x00000006	/* TODO: what is this?  */

	/* TODO: These symbols are only here because the linker doesn't
	   provide them like it should yet. Remove them when the linker
	   provides them.  */
	.globl _init
	.type _init,@function
	.globl _fini
	.type _fini,@function
_init:
	br	%r14
_fini:
	br	%r14

	/* Trick startup code into not running init array stuff.  */
	.globl __preinit_array_start
	.type __preinit_array_start,@function
	.globl __preinit_array_end
	.type __preinit_array_end,@function
	.globl __init_array_start
	.type __init_array_start,@function
	.globl __init_array_end
	.type __init_array_end,@function
	.globl __fini_array_start
	.type __fini_array_start,@function
	.globl __fini_array_end
	.type __fini_array_end,@function

__preinit_array_start:
__preinit_array_end:
__init_array_start:
__init_array_end:
__fini_array_start:
__fini_array_end:
	br	%r0


/* Define a symbol for the first piece of initialized data.  */
	.data
	.globl __data_start
__data_start:
	.long 0
	.weak data_start
	data_start = __data_start
