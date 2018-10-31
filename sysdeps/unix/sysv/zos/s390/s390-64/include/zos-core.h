/* C interface to core z/OS functionality.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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

#ifndef _ZOS_CORE_H
#define _ZOS_CORE_H 1

#include <stdbool.h>
#include <sys/cdefs.h>
#include <zos-utils.h>

/* The 31-bit bar.  */
#define PTR31_BAR 2147483647

/* The address of the Initial Pthread-creating Task's Task Control
   Block.  */
extern unsigned int __ipt_zos_tcb attribute_hidden;
hidden_proto (__ipt_zos_tcb)

void *__storage_obtain (unsigned int length, unsigned int tcbaddr,
			bool noexec,
			bool on_page_boundary) attribute_hidden
  __attribute_malloc__ __attribute_alloc_size__ ((1));

int __storage_release (unsigned int storage_addr, unsigned int length,
		       unsigned int tcbaddr,
		       bool noexec) attribute_hidden;

/* Get the address of the current thread's Task Control Block.  */
static inline unsigned int
__get_zos_tcb_addr (void)
{
  return GET_PTR31_SAFE ((volatile uintptr_t) (540));
}

#endif /* _ZOS_CORE_H  */
