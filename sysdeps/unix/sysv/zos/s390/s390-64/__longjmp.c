/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#include <setjmp.h>
#include <sysdep.h>
#include <tls.h>
#include <stap-probe.h>
#include <stdint.h>

/* Jump to the position specified by ENV, causing the
   setjmp call there to return VAL, or 1 if VAL is 0.  */
void
__longjmp (__jmp_buf env, int val)
{
#ifdef PTR_DEMANGLE
  uintptr_t guard = THREAD_GET_POINTER_GUARD ();
# ifdef CHECK_SP
  CHECK_SP (env, guard);
# endif
#elif defined CHECK_SP
  CHECK_SP (env, 0);
#endif

#ifdef PTR_DEMANGLE
  register uintptr_t r3 __asm__ ("%r3") = guard;
#endif
  register long int r2 __asm__ ("%r2") = val == 0 ? 1 : val;
  register void *r4 __asm__ ("%r4") = (void *) env;

  /* Restore registers and jump back.  */
  __asm__ __volatile__ (
#ifdef PTR_DEMANGLE
			/* load r13 and r14 into temp registers.  */
			"lmg	%%r8, %%r9, 96(%1) \n\t"
			/* SECURITY TODO: Demangle r13 and r14.  */
			"lgr	%%r13, %%r8 \n\t"
			"lgr	%%r14, %%r9 \n\t"
#else
			/* TODO: There's a bug in the s390x impl, it
			   uses r14 uninitialized here if PTR_DEMANGLE
			   is not defined.  */
			"lmg	%%r13, %%r14, 96(%1) \n\t"
#endif
			/* longjmp probe expects longjmp first argument, second
			   argument, and target address.  */
			LIBC_PROBE_ASM (longjmp, 8@%1 -4@%0 8@%%r14)

			/* restore fpregs  */
			"ld	%%f8, 112(%1) \n\t"
			"ld	%%f9, 120(%1) \n\t"
			"ld	%%f10, 128(%1) \n\t"
			"ld	%%f11, 136(%1) \n\t"
			"ld	%%f12, 144(%1) \n\t"
			"ld	%%f13, 152(%1) \n\t"
			"ld	%%f14, 160(%1) \n\t"
			"ld	%%f15, 168(%1) \n\t"
#ifdef HAVE_S390_VX_ASM_SUPPORT
			/* Load vector regs.
			   TODO: figure out how to make this work with
			   lower archlevels.  */
			"vlm	%v16, %v23, 176(%1), 0 \n\t"
#endif

			LIBC_PROBE_ASM (longjmp_target, 8@%1 -4@%0 8@%%r14)
			/* restore gregs and return to jmp_buf target  */
			"lmg	%%r1, %%r12, 0(%1) \n\t"
			"br	%%r14"
			: : "r" (r2), "r" (r4)
#ifdef PTR_DEMANGLE
			, "r" (r3)
#endif
			);

  /* Avoid `volatile function does return' warnings.  */
  for (;;);
}
