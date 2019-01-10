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

/* The 31-bit bar.  */
#define PTR31_BAR 2147483647

/* The subpool we use for everything.

   We want a private, fetch-protected, task-owned subpool. There are
   many available, the choice of specific subpool number is
   arbitrary.

   The storage key is ignored for the subpools we are using.

   TODO: We want a subpool no one else is likely to use.
   TODO: If we change this, change the storage flags in sir_entry
   also.  */
#define STORAGE_SUBPOOL 72


/* STORAGE flags  */
/* r15 flags  */
#define STORAGE_REQ_OBTAIN     0x00000000
#define STORAGE_REQ_RELEASE    0x00000001
#define STORAGE_COND_NO	       0x00000002
#define STORAGE_BNDRY_PAGE     0x00000004
#define STORAGE_MAX_MIN_LEN    0x00000008
#define STORAGE_LOC_VIRT_31_64 0x00000030
#define STORAGE_LOC_REAL_31_64 0x00000040
#define STORAGE_USE_ALET       0x00000080
#define STORAGE_SUBPOOL_MASK   0x0000ff00
#define STORAGE_KEY_MASK       0x00f00000
#define STORAGE_USE_TCBADDR    0x04000000
#define STORAGE_CHECKZERO_YES  0x08000000
#define STORAGE_LOC_REAL_64    0x10000000
#define STORAGE_USE_AR15       0x20000000
#define STORAGE_CALLRKY_YES    0x40000000

#define STORAGE_SUBPOOL_SHIFT  8

/* a15 flags  */
#define STORAGE_EXECUTABLE_NO  0x20000000


/* The standard storage flags that we use for most everything.  */
#define REGULAR_OBTAIN_FLAGS						\
  (STORAGE_SUBPOOL << STORAGE_SUBPOOL_SHIFT | STORAGE_REQ_OBTAIN	\
  | STORAGE_LOC_VIRT_31_64 | STORAGE_LOC_REAL_31_64)

#define REGULAR_RELEASE_FLAGS						\
  (STORAGE_SUBPOOL << STORAGE_SUBPOOL_SHIFT | STORAGE_REQ_RELEASE)	\


#ifndef __ASSEMBLER__
# include <stdbool.h>
# include <sys/cdefs.h>
# include <zos-utils.h>

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
#endif  /* __ASSEMBLER__  */
#endif /* _ZOS_CORE_H  */
