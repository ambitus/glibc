/* Definitions for S/390 syntax variations.
   Copyright (C) 1992-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.  Its master source is NOT part of
   the C library, however.  The master source lives in the GNU MP Library.

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

#undef ALIGN
#define ALIGN(log) .align 1<<log

#undef L
#define L(body) .L##body

/* z/OS TODO: Should I move this stuff into the s390-64 subdir?  */

/* z/OS TODO: decide once and for all if we're treating r1 as volatile or
   not.  */

/* Given an assembly program meeting the following criteria:
     1. It obeys the z/Linux ABI
     2. It has 5 or fewer arguments
     3. All of its arguments are 8 bytes in width
     4. It does not manipulate the stack pointer
     5. It does not access any stack space that could not be derived
	from value and meaning of its arguments.
     6. It does not access any structured data with a platform-dependent
	structure.
   It can be trivially wrapped to work on z/OS by using the following
   macros at its entry and exit points, respectively.  */

#ifdef __ZOS__  /* z/OS TODO: are we not using __MVS__ for sure?  */

/* Save regs then load args into the z/Linux regs.
   max_clobber_regnum is the highest numbered register that the function
   could potentially clobber. argnum is the number of arguments.  */
/* z/OS TODO: Should we have cfi here? Probably.  */
# define OS_SAVE_AND_LOAD(max_clobber_regnum, argnum)			\
	.if (max_clobber_regnum) != 0 ;					\
	.if (max_clobber_regnum) < 0 || (max_clobber_regnum) > 12 ;	\
	.error "Bad value for s/390 clobbered regs" ;			\
	.endif ;							\
									\
	stmg    1, max_clobber_regnum, 32(%r13) ;			\
									\
	.endif ;							\
									\
	.if (argnum) != 0 ;						\
	.if (max_clobber_regnum) < (argnum + 1)	;			\
	.error "Impossible value for s/390 clobbered regs" ;		\
	.endif ;							\
	.if (argnum) < 0 || (argnum) > 5 ;				\
	.error "Bad value for s/390 argnum" ;				\
	.endif ;							\
									\
	lmg     2, 1 + argnum, 0(%r1) ;					\
									\
	.endif

/* Move the return value into r15, then restore regs.
   max_clobber_regnum is the highest numbered register that the function
   could potentially clobber. return_reg is the register with the return
   address it will usually be %r14.  */
/* z/OS TODO: Should we have cfi here? Probably.  */
# define RETURN(max_clobber_regnum, return_reg)				\
	lgr     %r15, %r2 ;						\
									\
	.if (max_clobber_regnum) != 0 ;					\
	.if (max_clobber_regnum) < 0 || (max_clobber_regnum) > 12 ;	\
	.error "Bad value for s/390 clobbered regs" ;			\
	.endif ;							\
									\
	lmg     1, max_clobber_regnum, 32(%r13) ;			\
									\
	.endif ;							\
	br      return_reg

#else /* __ZOS__  */
# define OS_SAVE_AND_LOAD(max_clobber_regnum, argnum)
# define RETURN(max_clobber_regnum, return_reg) \
	br      return_reg
#endif /* __ZOS__  */
