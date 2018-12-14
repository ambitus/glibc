/* Search a character in a block of memory.  64 bit z/OS version.
   Copyright (C) 2018 Free Software Foundation, Inc.
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

/* Inputs:
     1: address to memory area
     2: character to find
     3: number of bytes to search  */

#include "sysdep.h"
#include "asm-syntax.h"

	.text
ENTRY(memchr)
	# set up r0 with the character to find
	lhi  %r0,0xff
	lg   %r15,0(%r1)
	n    %r0,12(%r1)

	# z/OS TODO: if we aren't saving r1, remove this store
	st    %r1,32(%r13)
	lg    %r1,16(%r1)

	# set up end addr
	la    %r1,0(%r1,%r15)

	# srst's implicit third arg is r0
0:	srst  %r15,%r1
	jo    0b
	brc   13,1f
	slgr  %r15,%r15

	# z/OS TODO: if we aren't saving r1, remove this load
1:	lg    %r1,32(%r13)
	br    %r14

END(memchr)
libc_hidden_builtin_def (memchr)
