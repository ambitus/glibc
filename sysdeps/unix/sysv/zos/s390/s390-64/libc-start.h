/* Generic definitions for libc main startup.
   Copyright (C) 2018 Rocket Software.
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

/* TODO: Signal setup. ARCH_SPECIAL_TEARDOWN.
   Dynamic version of libc-start.c stuff.  */

#ifndef _LIBC_START_H
#define _LIBC_START_H

/* TODO: The shared equivalent of this.  */

#ifndef SHARED

#include <stddef.h>
#include <stdbool.h>
#include <lock-free.h>
#include <zos-futex.h>
#include <zos-utils.h>
#include <zos-core.h>
#include <map-info.h>

/* By default we perform STT_GNU_IFUNC resolution *before* TLS
   initialization, and this means you cannot, without machine
   knowledge, access TLS from an IFUNC resolver.  */
#define ARCH_SETUP_IREL() apply_irel ()
#define ARCH_SETUP_TLS() __libc_setup_tls ()
#define ARCH_APPLY_IREL()
#define ARCH_SPECIAL_SETUP() zos_global_structure_setup ()
#define ARCH_SPECIAL_TEARDOWN(res)


/* We need to set up the following three structures at some point
   during early initialization.  */

/* The thread pointer table.  */
extern lf_hash_table *__zos_tp_table;
libc_hidden_proto (__zos_tp_table)

extern lf_hash_table *__zos_futex_table;
libc_hidden_proto (__zos_futex_table)

/* List of some memory areas allocated with memory allocation
   primitives. Used by mmap() MAP_ANONYMOUS.  */
extern lfl_list_t __zos_tracked_allocs;
libc_hidden_proto (__zos_tracked_allocs)

extern object_pool __wait_token_pool;
libc_hidden_proto (__wait_token_pool)

extern object_pool __alloc_info_pool;
libc_hidden_proto (__alloc_info_pool)


#define roundup16(val) (((val) + 16 - 1) & ~(16 - 1))
#define ZOS_THREAD_BUCKETS 64
#define ZOS_FUTEX_BUCKETS 64

#define ZOS_THREAD_TABLE_SIZE \
  (sizeof (lf_hash_table) + sizeof (lfl_list_t) * ZOS_THREAD_BUCKETS)

#define ZOS_FUTEX_TABLE_SIZE \
  (sizeof (lf_hash_table) + sizeof (lfl_list_t) * ZOS_FUTEX_BUCKETS)

#define ZOS_PERM_STORE_SIZE						\
  /* Hash table for thread pointers.  */				\
  (roundup16 (ZOS_THREAD_TABLE_SIZE) +					\
   /* Hashed wait queue for futexes.  */				\
   roundup16 (ZOS_FUTEX_TABLE_SIZE) +					\
   /* lfl_node object pool (shared between the above) header.  */	\
   roundup16 (subpool_size (lfl_node_t))				\
   /* PE object pool header.  */					\
   /* roundup16 (subpool_real_size (16, 1)) + */			\
   /* Anon map data object pool header.	*/				\
   /* roundup16 (subpool_real_size_type (struct map_info))  */)


static struct { void *next; size_t used; } __perm_store = { NULL, 0};


/* Align everything to 16 bytes.  */
static inline void *
perm_store_alloc (size_t size)
{
  uintptr_t new_next, ret, prev;

  prev = (uintptr_t) __perm_store.next;
  ret = (prev + 16 - 1) & ~(16 - 1);
  new_next = ret + size;

  size_t new_used = __perm_store.used + prev - new_next;
  if (new_used > ZOS_PERM_STORE_SIZE)
    CRASH_NOW ();
  __perm_store.used = new_used;
  __perm_store.next = (void *) new_next;

  return (void *) ret;
}

static void
zos_global_structure_setup (void)
{
  object_pool *node_pool;
  /* Do several things here.  */
  /* 1. Save the IPT Task Control Block address.  */
  __ipt_zos_tcb = __get_zos_tcb_addr ();

  /* 2. Obtain storage for the major global structures.  */
  void *res = __storage_obtain (ZOS_PERM_STORE_SIZE, __ipt_zos_tcb,
				true, false);

  if (!res)
    CRASH_NOW ();

  __perm_store.used = 0;
  __perm_store.next = res;

  __zos_tp_table = perm_store_alloc (ZOS_THREAD_TABLE_SIZE);
  __zos_futex_table = perm_store_alloc (ZOS_FUTEX_TABLE_SIZE);

  /* 3. Obtain storage for the lock-free list node pool.  */
  node_pool = perm_store_alloc (subpool_size (lfl_node_t));

  /* 4. Initialize object pools.  */
  void node_init (void *addr)
  {
    ((lfl_node_t *) addr)->next.tag = 0;
  }

  __obj_pool_initialize (node_pool, sizeof (lfl_node_t),
			 _Alignof (lfl_node_t), false, node_init);
  __obj_pool_initialize (&__wait_token_pool, sizeof (wait_token),
			 _Alignof (wait_token), false, NULL);
  __obj_pool_initialize (&__alloc_info_pool, sizeof (struct map_info),
			 _Alignof (struct map_info), false, NULL);

  /* 5. Initialize primary data structures.  */
  __lf_hash_table_initialize (__zos_tp_table, ZOS_THREAD_BUCKETS,
			      lfl_set, node_pool);
  __lf_hash_table_initialize (__zos_futex_table, ZOS_FUTEX_BUCKETS,
			      lfl_hashed_wait_queue_bucket, node_pool);
  __lfl_initialize (&__zos_tracked_allocs, lfl_set, node_pool);


}

#endif /* ! SHARED  */

#endif /* _LIBC_START_H  */
