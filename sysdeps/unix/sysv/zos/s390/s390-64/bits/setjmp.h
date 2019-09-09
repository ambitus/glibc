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

/* Define the machine-dependent type `jmp_buf'.  z/OS version.  */

#ifndef __ZOS_SETJMP_H__
#define __ZOS_SETJMP_H__

#if !defined _SETJMP_H && !defined _PTHREAD_H
# error "Never include <bits/setjmp.h> directly; use <setjmp.h> instead."
#endif

#ifndef	_ASM

typedef struct __zos_jmp_buf
{
  /* We save gprs r1 - r14.  */
  long int __gregs[14];

  /* We save fpu registers f8 - f15.  */
  long __fpregs[8];

  /* We save vector registers v16 - v23 (if present).  */
  char __vregs[128];
  /* TODO: FPC reg? */
} __jmp_buf[1];

#endif

#endif /* !__ZOS_SETJMP_H__ */
