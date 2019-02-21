/* Copyright (C) 2018 Rocket Software
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
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>

   /* This code defines a trampoline that is called as a normal function
      from C, with the address of a syscall routine and all necessary
      args. The trampoline wraps the syscall, smoothing over differences
      in the calling convention.

      TODO: We might end up changing the calling convention in the
      compiler. If we do, delete all this.

      TODO: Make this a hidden symbol.  */

	.text
ENTRY (__bpxk_syscall)
	# Save r1 and r14.
	lg	%r15, 136(%r13)
	stg	%r1, 24(%r15)
	stg	%r14, 8(%r15)

	lg	%r15, 0(%r1)	# load the syscall routine addr
	la	%r1, 8(%r1)	# set args to what the syscall expects
	/* TODO: Should this be a BALR?  */
	basr	%r14, %r15	# Do the syscall

	# No retval in r15

	# Restore r1 and r14.
	lg	%r15, 136(%r13)
	lg	%r1, 24(%r15)
	lg	%r14, 8(%r15)

	# return
	br	%r14
END (__bpxk_syscall)

