/* ESTAEX handler routine.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Michael Colavita <mcolavita@rocketsoftware.com>.

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

   /* TODO: Make this a hidden symbol.  */

#include <sysdep.h>
#include <zos-core.h>

	.text
ENTRY (__estaex_handler_wrapper)
	/* When this routine is entered, registers are allocated as
	   follows:
	     r0: equal to 12 if there is no SDWA
	     r1: SDWA
	     r2: Address of ESTAEX PARM parameter
	     r14: Return address (must be preserved)  */

	/* Use linkage stack to save status on entry.  */
	bakr	%r14, %r0

	/* TODO: We should not outright return without explaining why
	   here when errors occur. Handle this better.  */

	/* Validate presence of SDWA and save.  */
	chi	%r0, 12
	je	.Lestaex_abort
	lgr	%r3, %r1

	/* Validate handler data.  */
	ltgr	%r2, %r2
	jz	.Lestaex_abort

	/* Load and validate user handler.  */
	lg	%r4, 0(%r2)
	ltgr	%r4, %r4
	jz	.Lestaex_abort

	/* TODO: eyecatcher validation? PSW key work?  */
	/* Save r14 */
	lgr	%r5, %r14

	larl	%r15, .Lestaex_stack_length
	/* Load storage amount into r0 and flags into r15.  */
	l	%r0, 0(%r15)
	l	%r15, .Lestaex_stack_flags-.Lestaex_stack_length(%r15)
	llgt	%r14, 16
	l	%r14, 772(%r14)
	l	%r14, 160(%r14)
	pc	0(%r14)
	/* r15 has the return code and r1 has the address.  */

	/* Check if it failed.  */
	ltgr	%r15, %r15
	jnz	.Lestaex_abort

	/* Set up a stack and arguments.  */
	lgr	%r13, %r1
	la	%r1, 144(%r13)
	stg	%r3, 144(%r13) /* Parameter 1: SDWA  */
	lg	%r3, 8(%r2)
	stg	%r3, 152(%r13) /* Parameter 2: User data  */
	llilh	%r14, 50932
	oill	%r14, 58049
	st	%r14, 4(%r13)
	la	%r14, 160(%r13)
	stg	%r14, 136(%r13)

	/* Invoke the user handler.  */
	basr	%r14, %r4

	/* TODO: Important: release storage.  */

	/* Restore r14.  */
	lgr	%r14, %r5

.Lestaex_abort:
	/* End ESTAEX processing.  */
	eregg	%r0, %r1
	pr


	/* TODO: This is pretty much a duplication of what's in start.S.
	   Factor it out somehow.  */
	.section .rodata
	.align 8
.Lestaex_stack_length:
	.long (1 << 20)	/* 1 MiB  */
.Lestaex_stack_flags:
	.long REGULAR_OBTAIN_FLAGS  /* Flags for STORAGE OBTAIN.  */

	.text
END (__estaex_handler_wrapper)
