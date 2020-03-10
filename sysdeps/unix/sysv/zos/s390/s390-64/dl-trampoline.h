/* PLT trampolines.  z/OS version.
   Copyright (C) 2020 Free Software Foundation, Inc.
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

/* The PLT stubs will call _dl_runtime_resolve/_dl_runtime_profile
 * with the following linkage:
 *   r1 : parameter pointer
 *   112(r13), 120(r13) : PLT arguments PLT1, PLT2
 * The normal clobber rules for function calls apply:
 *   r0, r15 : call clobbered
 *   r1 - r14 :	 call saved
 *   r14 : return address
 *   r15 : return value
 *   f0 - f7 : call clobbered
 *   f8 - f15 : call saved
 *   v0 - v7 : bytes 0-7 overlap with f0-f7: call clobbered
               bytes 8-15: call clobbered
 *   v8 - v15 : bytes 0-7 overlap with f8-f15: call saved
                bytes 8-15: call clobbered
 *   v16 - v23 : call saved
 *   v24 - v31 : call clobbered
 */
/* z/OS TODO: Add CFI info here.  */

	.text
	.globl _dl_runtime_resolve
	.type _dl_runtime_resolve, @function
	.align 16
_dl_runtime_resolve:
	cfi_startproc

	lg	%r15, 136(%r13)
	stmg	%r1, %r2, 0(%r15)	# save %r1, %r2
	stg	%r14, 16(%r15)		# save %r14
	la	%r1, 24(%r15)		# Point arg to arg slots
	mvc	0(16, %r1), 112(%r13)	# copy args for fixup saved by PLT
	lgr	%r2, %r15		# save the arg save location
	la	%r15, 48(%r15)
	stg	%r15, 136(%r13)		# reuse this stack frame by
					# expanding its boundaries

	brasl	%r14, _dl_fixup		# call _dl_fixup
					# function addr returned in %r15

	lg	%r14, 16(%r2)		# load saved value of %r14
	lmg	%r1, %r2, 0(%r2)	# load saved values of %r1, %r2
	br	%r15
	cfi_endproc
	.size _dl_runtime_resolve, .-_dl_runtime_resolve


#ifndef PROF
	.globl _dl_runtime_profile
	.type _dl_runtime_profile, @function
	.align 16
_dl_runtime_profile:
	cfi_startproc

	lg	%r15, 136(%r13)
	stmg	%r1, %r2, 0(%r15)	# save %r1, %r2
	stg	%r14, 16(%r15)		# save %r14
	la	%r1, 24(%r15)		# Point arg to arg slots
	mvc	0(16, %r1), 112(%r13)	# copy args for fixup saved by PLT
	lgr	%r2, %r15		# save the arg save location
	la	%r15, 48(%r15)
	stg	%r15, 136(%r13)		# reuse this stack frame by
					# expanding its boundaries

	brasl	%r14, _dl_profile_fixup	# call _dl_profile_fixup
					# function addr returned in %r15

	lg	%r14, 16(%r2)		# load saved value of %r14
	lmg	%r1, %r2, 0(%r2)	# load saved values of %r1, %r2
	br	%r15
	cfi_endproc
	.size _dl_runtime_profile, .-_dl_runtime_profile
#endif  /* !PROF  */
