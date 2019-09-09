/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#include <sysdep.h>

   /* This code defines a wrapper that is called as a normal function
      from C, with the address of a syscall routine and all necessary
      args. The wrapper wraps the syscall, smoothing over differences
      in the calling convention.

      z/OS TODO: We might end up changing the calling convention in the
      compiler. If we do, delete some of this.

      z/OS TODO: Make this a hidden symbol.  */

	.text
ENTRY (__bpxk_syscall)
	# We don't bother sticking to the save area convention here,
	# since the syscalls don't use it.

	# Save r1 and r14.
	lg	%r15, 136(%r13)
	stg	%r1, 0(%r15)
	stg	%r14, 8(%r15)

	# Save FP regs
	std	%f8, 16(%r15)
	std	%f9, 24(%r15)
	std	%f10, 32(%r15)
	std	%f11, 40(%r15)
	std	%f12, 48(%r15)
	std	%f13, 56(%r15)
	std	%f14, 64(%r15)
	std	%f15, 72(%r15)

	# Check CVTVEF to see if VRs are available, and save them if
	# they are.
	# TODO: Check that this works.
	# TODO: This stuff is quite general, should be factored out of
	# here.
	llgt	%r14, 16
	tm	244(%r14), 128
	jz	0f

	.machine push
	.machine "z13"
	vstm	%v16, %v23, 80(%r15)
	.machine pop

0:
	lg	%r15, 0(%r1)	# load the syscall routine addr
	la	%r1, 8(%r1)	# set args to what the syscall expects

	# TODO: Should this be a BALR?
	basr	%r14, %r15	# Do the syscall

	lg	%r15, 136(%r13)

	# Restore vector registers if necessary
	llgt	%r14, 16
	tm	244(%r14), 128
	jz	0f

	.machine push
	.machine "z13"
	vlm	%v16, %v23, 80(%r15)
	.machine pop

0:
	# Restore FP regs
	ld	%f8, 16(%r15)
	ld	%f9, 24(%r15)
	ld	%f10, 32(%r15)
	ld	%f11, 40(%r15)
	ld	%f12, 48(%r15)
	ld	%f13, 56(%r15)
	ld	%f14, 64(%r15)
	ld	%f15, 72(%r15)

	# Restore r1 and r14.
	lg	%r1, 0(%r15)
	lg	%r14, 8(%r15)

	# return
	br	%r14

END (__bpxk_syscall)
