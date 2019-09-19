/* Special .init and .fini section support for 64 bit z/OS.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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

/* crti.S puts a function prologue at the beginning of the .init and
   .fini sections and defines global symbols for those addresses, so
   they can be called as functions.  The symbols _init and _fini are
   magic and cause the linker to emit DT_INIT and DT_FINI.  */

#include <libc-symbols.h>
#include <sysdep.h>

#ifndef PREINIT_FUNCTION
# define PREINIT_FUNCTION __gmon_start__
#endif

#ifndef PREINIT_FUNCTION_WEAK
# define PREINIT_FUNCTION_WEAK 1
#endif

#if PREINIT_FUNCTION_WEAK
	weak_extern (PREINIT_FUNCTION)
	/* We can't do pc-relative relocs against weak symbols on
	   z/OS.  */
	.section .rodata
2:	.quad	PREINIT_FUNCTION
#else
	.hidden PREINIT_FUNCTION
#endif

	.section .init,"ax",@progbits
	.align	8, 0x07
	.globl	_init
	.hidden	_init
	.type	_init,@function
_init:
	stmg	%r14, %r12, 8(%r13)
	lg	%r15, 136(%r13)
	stg	%r13,128(%r15)
	la	%r0, 160(%r15)
	stg	%r0, 136(%r15)
	stg	%r0, 144(%r15)
	llilh	%r0, 50932
	oill	%r0, 58049
	st	%r0, 4(%r15)
	lgr	%r13, %r15

#if PREINIT_FUNCTION_WEAK
	larl	%r15, 2b
	lg	%r15, 0(%r15)
	ltgr	%r15, %r15
	je	1f
	basr	%r14, %r15
1:
	bcr	0, %r0
#else
	brasl	%r14, PREINIT_FUNCTION
#endif
	mvc	136(8,%r13), 144(%r13)
	.align	8, 0x07


	.section .fini,"ax",@progbits
	.align	8, 0x07
	.globl	_fini
	.hidden	_fini
	.type	_fini,@function
_fini:
	stmg	%r14, %r12, 8(%r13)
	lg	%r15, 136(%r13)
	stg	%r13,128(%r15)
	la	%r0, 160(%r15)
	stg	%r0, 136(%r15)
	stg	%r0, 144(%r15)
	llilh	%r0, 50932
	oill	%r0, 58049
	st	%r0, 4(%r15)
	lgr	%r13, %r15
	.align	8, 0x07
