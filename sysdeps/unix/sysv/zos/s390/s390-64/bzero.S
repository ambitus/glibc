/* bzero -- set a block of memory to zero.  64 bit z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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

/* Inputs:
     1: address of memory area
     2: number of bytes to fill.  */

#include "sysdep.h"
#include "asm-syntax.h"

	.text
ENTRY(__bzero)
	lg	%r0,8(%r1)
	ltgr    %r0,%r0
	jz	.Lret

	# z/OS TODO: if we aren't saving r1, change this
	stmg	%r1,%r4,32(%r13)    # Save the regs we will use
	lg	%r1,0(%r1)

	sgr	%r4,%r4		    # set pad byte to zero
	sgr	%r2,%r2		    # no source for MVCLE, only a pad byte
	sgr	%r3,%r3
.L0:	mvcle	%r0,%r2,0(%r4)	    # thats it, MVCLE is your friend
	jo	.L0

	lmg	%r1,%r4,32(%r13)
.Lret:	br	%r14
END(__bzero)

#ifndef NO_WEAK_ALIAS
weak_alias (__bzero, bzero)
#endif
