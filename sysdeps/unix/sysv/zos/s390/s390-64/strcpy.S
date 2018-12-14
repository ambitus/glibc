/* strcpy - copy a string from source to destination.  64 bit z/OS version.
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
     1: address of destination
     2: address of source  */

#include "sysdep.h"
#include "asm-syntax.h"

        .text
ENTRY(strcpy)
	slgr  %r0,%r0		# set end char to \0

	lg    %r15,0(%r1)

	# z/OS TODO: if we aren't saving r1, remove this store
	stmg  %r1,%r2,32(%r13)
	lg    %r1,8(%r1)

	lgr   %r2,%r15
0:	mvst  %r2,%r1
	jo    0b

	# z/OS TODO: if we aren't saving r1, remove this load
	lmg   %r1,%r2,32(%r13)
	br    %r14
END(strcpy)
libc_hidden_builtin_def (strcpy)
