/* C interface to core z/OS functionality.
   Copyright (C) 2019 Free Software Foundation, Inc.
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

#ifndef _ZOS_CORE_H
#define _ZOS_CORE_H 1

/* The 31-bit bar.  */
#define PTR31_BAR 2147483647

/* The subpool we use for everything.

   Subpool 131 is private low, fetch-protected, pageable, and does not
   require authorization. Storage allocated from it is owned by the
   jobstep TCB instead of the task that allocates it, meaning that it
   will not be deallocated until all threads end (or it's manually
   deallocated). The storage key is ignored for this subpool.  */
#define STORAGE_SUBPOOL 131


/* STORAGE flags  */
/* r15 flags  */
#define STORAGE_REQ_OBTAIN     0x00000000
#define STORAGE_REQ_RELEASE    0x00000001
#define STORAGE_COND_NO	       0x00000002
#define STORAGE_BNDRY_PAGE     0x00000004
#define STORAGE_MAX_MIN_LEN    0x00000008
#define STORAGE_LOC_VIRT_31    0x00000030
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
   | STORAGE_CALLRKY_YES						\
   | STORAGE_LOC_VIRT_31 | STORAGE_LOC_REAL_31_64 | STORAGE_LOC_REAL_64)

#define REGULAR_RELEASE_FLAGS						\
  (STORAGE_SUBPOOL << STORAGE_SUBPOOL_SHIFT | STORAGE_REQ_RELEASE       \
  | STORAGE_CALLRKY_YES)

#ifndef __ASSEMBLER__
# include <stdbool.h>
# include <sys/cdefs.h>
# include <zos-utils.h>

/* The position of guard areas for IARV64 allocation requests. If LOW,
   guard areas start from low addresses (the start of the allocation),
   if HIGH, guard areas start from high addresses.  */
enum guardloc
{
  GUARDLOC_LOW,
  GUARDLOC_HIGH
};

/* The address of the Initial Pthread-creating Task's Task Control
   Block.  */
extern unsigned int __ipt_zos_tcb attribute_hidden;

extern void *__storage_obtain (unsigned int length, bool noexec,
			       bool on_page_boundary) attribute_hidden
  __attribute_malloc__ __attribute_alloc_size__ ((1));

extern int __storage_release (unsigned int storage_addr,
			      unsigned int length) attribute_hidden;

extern void __pgser_release (unsigned int storage_addr,
			     unsigned int length) attribute_hidden;

extern void * __iarv64_getstorage (uint64_t segments,
				   uint64_t guardsize,
				   enum guardloc loc,
				   uint32_t *rc_ptr,
				   uint32_t *reason_ptr)
  __attribute_malloc__ attribute_hidden;
extern void __iarv64_detach (void *memobjstart,
			     uint32_t *rc_ptr,
			     uint32_t *reason_ptr) attribute_hidden;
extern void __iarv64_grow_guard (void *origin,
				 uint64_t convertsize,
				 uint32_t *rc_ptr,
				 uint32_t *reason_ptr) attribute_hidden;
extern void __iarv64_set_guard (void *convertstart,
				uint64_t convertsize,
				uint32_t *rc_ptr,
				uint32_t *reason_ptr) attribute_hidden;
extern void __iarv64_shrink_guard (void *origin,
				   uint64_t convertsize,
				   uint32_t *rc_ptr,
				   uint32_t *reason_ptr)
  attribute_hidden;
extern void __iarv64_set_nonguard (void *convertstart,
				   uint64_t convertsize,
				   uint32_t *rc_ptr,
				   uint32_t *reason_ptr)
  attribute_hidden;

struct iarv64_range
{
  void *address;
  uint64_t pages;
};

extern void __iarv64_protect (uint32_t range_count,
			      struct iarv64_range *range_list,
			      uint32_t *rc_ptr,
			      uint32_t *reason_ptr) attribute_hidden;
extern void __iarv64_unprotect (uint32_t range_count,
				struct iarv64_range *range_list,
				uint32_t *rc_ptr,
				uint32_t *reason_ptr) attribute_hidden;
extern void __iarv64_discard_data (uint32_t range_count,
				   struct iarv64_range *range_list,
				   uint32_t *rc_ptr,
				   uint32_t *reason_ptr)
  attribute_hidden;

static inline void
__iarv64_protect_one (void *address, uint64_t pages,
		      uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64_range range = { address, pages };
  __iarv64_protect (1, &range, rc_ptr, reason_ptr);
}

static inline void
__iarv64_unprotect_one (void *address, uint64_t pages,
			uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64_range range = { address, pages };
  __iarv64_unprotect (1, &range, rc_ptr, reason_ptr);
}

static inline void
__iarv64_discard_data_one (void *address, uint64_t pages,
			   uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64_range range = { address, pages };
  __iarv64_discard_data (1, &range, rc_ptr, reason_ptr);
}

extern int __mvsprocclp (int status) attribute_hidden;

#ifndef ZOS_HIDDEN_SYSCALL
extern void * __loadhfs (char *path);
extern void * __load_pt_interp (void);

# if IS_IN (libc) || IS_IN (rtld)
hidden_proto (__loadhfs)
hidden_proto (__load_pt_interp)
# endif
#else
extern void * __loadhfs (char *path) attribute_hidden;
extern void * __load_pt_interp (void) attribute_hidden;
#endif


struct __gtrace_64
{
  unsigned short len;
  unsigned short fid;
  unsigned int ptr31;
  unsigned long int ptr64;
};

#define __GTF_USER_CLASS 0x0e

/* eid can be 0 through 1023 for user events. */

#define __gtrace(eid, fid_, data_ptr_, data_len_, rc)			\
  do {									\
    unsigned int rc __attribute__ ((used)) = 0;				\
    struct __gtrace_64 t = {.len = data_len_,				\
			    .fid = fid_,				\
			    .ptr31 = 0,					\
			    .ptr64 = (unsigned long int) (data_ptr_)};	\
    __asm__ __volatile__ ("la      1, %1  \n\t"				\
			  "mc      %2, %3 \n\t"				\
			  "st      15, %0 \n\t"				\
			  : "=m"(rc)					\
			  : "m"(t), "n"(eid), "n"(__GTF_USER_CLASS),	\
			    "m"(*(data_ptr_))				\
			  : "r1", "r15", "memory");			\
  } while (0)

/* Example:  __gtrace(0x312, 0, &c, sizeof(c), rc); */

/* Define this to 1 to enable tracing.  */
#define HAVE_GTRACE 0

#endif  /* !__ASSEMBLER__  */
#endif /* !_ZOS_CORE_H  */
