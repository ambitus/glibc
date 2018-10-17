/* setjmp for 64 bit S/390, z/OS version.
   Copyright (C) 2018 Rocket Software
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
#define _ASM
#define _SETJMP_H
#include <bits/setjmp.h>
#include <shlib-compat.h>
#include <stap-probe.h>

/* TODO: do we need the compat stuff?  */
/* TODO: Why doesn't the z/Linux version have any cfi stuff?  */
#if !IS_IN (rtld) && defined SHARED \
  && SHLIB_COMPAT (libc, GLIBC_2_19, GLIBC_2_20)
# define NEED_COMPAT_SYMBOLS 1
/* We need a unique name in case of symbol versioning.  */
# define setjmp __v1setjmp
# define _setjmp __v1_setjmp
# define __sigsetjmp __v1__sigsetjmp
#else
# define NEED_COMPAT_SYMBOLS 0
#endif

/* r1 - arg list pointer
   r2 - Original NAB
   r0, r15 - scratch  */

#define SIGSETJMP_HEAD							\
	/* Save original r1 and r2 into jmp_buf. We use them later.  */	\
	lg	%r15, 0(%r1);						\
	stmg	%r1, %r2, 0(%r15);					\
	lg	%r2, 136(%r13);	/* Store original NAB in r2.  */	\
	lgr	%r15, %r2;	/* Increment NAB by 16.  */		\
	aghi	%r15, 16;						\
	stg	%r15, 136(%r13);

#define ALT_SIGSETJMP_ENTRY(savesigs)					\
	/* We need to add an arg, so copy first arg to stack.  */	\
	SIGSETJMP_HEAD							\
									\
	/* set up new arg list.  */					\
	mvc	0(8,%r2), 0(%r1);	/* copy arg1 to stack.  */	\
	lghi	%r15, savesigs;	/* set second arg to savesigs.  */	\
	stg	%r15, 8(%r2);						\
	la	%r1, 0(%r2);	/* set r1 to arg pointer.  */		\
									\
	/* Branch relative to __sigsetjmp.  */				\
	j	.Linternal_sigsetjmp;

	/* We include the BSD entry points here as well.  */
ENTRY (setjmp)
	ALT_SIGSETJMP_ENTRY (1)
END (setjmp)

	/* Binary compatibility entry point.  */
ENTRY(_setjmp)
.Linternal__setjmp:
	ALT_SIGSETJMP_ENTRY (0)
END (_setjmp)
#if NEED_COMPAT_SYMBOLS
strong_alias (_setjmp, __GI__setjmp)
#else
libc_hidden_def (_setjmp)
#endif

ENTRY (__setjmp)
	/* Same as _setjmp.  */
	j	.Linternal__setjmp
END (__setjmp)

ENTRY (__sigsetjmp)
	/* The other entry points save r1 and increment NAB so we do
	   it here too to simplify things.  */
	SIGSETJMP_HEAD
.Linternal_sigsetjmp:
	lg	%r15, 0(%r1)	/* Load jmp_buf addr.  */
	lgf	%r0, 12(%r1)	/* Load savesigs (for the probe).  */
	/* setjmp probe expects sig/setjmp first argument (8@%r15),
	   second argument (-4@%r0) and target address (8@%r14).  */
	LIBC_PROBE (setjmp, 3, 8@%r15, -4@%r0, 8@%r14)

	/* Store registers in jmp_buf.  */
#ifdef PTR_MANGLE
	/* SECURITY TODO: mangle r13, r14 (any others?) */
	stmg	%r3, %r14, 8(%r15)
#else
	stmg	%r3, %r14, 8(%r15)
#endif
	/* save FP regs.  */
	std	%f8, 112(%r15)
	std	%f9, 120(%r15)
	std	%f10, 128(%r15)
	std	%f11, 136(%r15)
	std	%f12, 144(%r15)
	std	%f13, 152(%r15)
	std	%f14, 160(%r15)
	std	%f15, 168(%r15)

#ifdef HAVE_S390_VX_ASM_SUPPORT
	/* TODO: How can we get this to run on lower archlevels without
	   using separate binaries?  */
	vstm	%v16, %v23, 176(%r15), 4	/* save vector regs.  */
#endif

#if IS_IN (rtld)
	/* In ld.so we never save the signal mask.  */

	stg	%r2, 136(%r13)		/* Restore original NAB.  */

	lg	%r15, 0(%r1)		/* Load jmp_buf addr.  */
	lmg	%r1, %r2, 0(%r15)	/* Load original r1 and r2.  */

	/* return 0  */
	lghi	%r15, 0
	br	%r14
#elif defined PIC
	brasl	%r14, __sigjmp_save@PLT	/* Branch to PLT of __sigsetjmp.  */
#else
	brasl	%r14, __sigjmp_save
#endif
	lgr	%r0, %r15		/* Save return value.  */

	stg	%r2, 136(%r13)		/* Restore original NAB.  */

	lg	%r15, 0(%r1)		/* Load jmp_buf addr.  */
	/* Load original r1, r2, and r14.  */
	lmg	%r1, %r2, 0(%r15)
	lg	%r14, 104(%r15)

	lgr	%r15, %r0	/* Return result of __sigjmp_save.  */
	br	%r14
END (__sigsetjmp)
#if NEED_COMPAT_SYMBOLS
strong_alias (__sigsetjmp, __GI___sigsetjmp)
#else
libc_hidden_def (__sigsetjmp)
#endif

#if NEED_COMPAT_SYMBOLS
/* In glibc release 2.19 new versions of setjmp-functions were introduced,
   but were reverted before 2.20. Thus both versions are the same function.  */

# undef setjmp
# undef _setjmp
# undef __sigsetjmp

strong_alias (__v1setjmp, __v2setjmp);
versioned_symbol (libc, __v1setjmp, setjmp, GLIBC_2_0);
compat_symbol (libc, __v2setjmp, setjmp, GLIBC_2_19);

strong_alias (__v1_setjmp, __v2_setjmp);
versioned_symbol (libc, __v1_setjmp, _setjmp, GLIBC_2_0);
compat_symbol (libc, __v2_setjmp, _setjmp, GLIBC_2_19);

strong_alias (__v1__sigsetjmp, __v2__sigsetjmp);
versioned_symbol (libc, __v1__sigsetjmp, __sigsetjmp, GLIBC_2_0);
compat_symbol (libc, __v2__sigsetjmp, __sigsetjmp, GLIBC_2_19);
#endif /* NEED_COMPAT_SYMBOLS */
