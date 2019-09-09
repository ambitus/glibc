/* strcmp - compare two string.  64 bit z/OS version.
   This file is part of the GNU C Library.
   Copyright (C) 2019 Free Software Foundation, Inc.

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
     1: address of string 1
     2: address of string 2  */

#include "sysdep.h"
#include "asm-syntax.h"

	.text
ENTRY(strcmp)
	lg    %r15,0(%r1)

	# z/OS TODO: if we aren't saving r1, remove this store
	stg   %r1,32(%r13)
	lg    %r1,8(%r1)

        slr   %r0,%r0		# set end char to \0
0:	clst  %r15,%r1
	jo    0b
	jp    1f
	jm    2f
	slgr  %r15,%r15

	# z/OS TODO: if we aren't saving r1, remove this load
	lg    %r1,32(%r13)
	br    %r14

1:	lghi  %r15,1
	# z/OS TODO: if we aren't saving r1, remove this load
	lg    %r1,32(%r13)
	br    %r14

2:	lghi  %r15,-1
	# z/OS TODO: if we aren't saving r1, remove this load
	lg    %r1,32(%r13)
	br    %r14
END(strcmp)
libc_hidden_builtin_def (strcmp)
