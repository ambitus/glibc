/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#include <zos-syscall-base.h>
#include <zos-core.h>

/* race condition? */
static __always_inline void
__zos_early_setup (void)
{
  /* __bpx_call_table = GET_BPX_FUNCTION_TABLE; */

  /* Get the address of the current thread's Task Control Block.  */
  /* __ipt_zos_tcb = __get_zos_tcb_addr ();  */
}

/* co-opt VDSO_SETUP to do our initialization, since we aren't
   using it for anything else. */
#define VDSO_SETUP __zos_early_setup

#include <csu/init-first.c>
