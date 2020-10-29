/* Thread pointer access for z/OS.
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

#if ((defined (SHARED) && !IS_IN (rtld))	\
     || (!defined (SHARED) && !IS_IN (libc)))
# error "tls-lookup used in the wrong library"
#endif
/* For the shared library __zos_get_thread_pointer must only be defined
   in rtld. For the static library, it must only be defined in libc.  */

#include <stddef.h>
#include <tls.h>
#include <lock-free.h>
#include <zos-core.h>

#define ZOS_THREAD_BUCKETS 64
#define ZOS_THREAD_TABLE_SIZE						\
  (sizeof (lf_hash_table) + sizeof (lfl_list_t) * ZOS_THREAD_BUCKETS)

/* The hash table in which we store thread pointers.  */
lf_hash_table *__zos_tp_table;
rtld_hidden_data_def (__zos_tp_table)


void *
__zos_get_thread_pointer (void)
{
  unsigned int task_addr = TCB_PTR;
  return (void *) (uintptr_t) __lf_hash_table_get (task_addr,
						   __zos_tp_table);
}


void
__zos_set_thread_pointer (void *addr)
{
  unsigned int task_addr = TCB_PTR;
  __lf_hash_table_put (task_addr, (uintptr_t) addr, __zos_tp_table);
}


void
__zos_clear_thread_pointer (void)
{
  unsigned int task_addr = TCB_PTR;
  __lf_hash_table_pop (task_addr, __zos_tp_table);
}

/* Initializer for object pool elements.  */

static void
node_init (void *addr)
{
  ((lfl_node_t *) addr)->next.tag = 0;
}

/* Do initial thread pointer setup.
   Should only be called once per process.  */

void
__zos_initialize_thread_pointer (void *addr)
{
  object_pool *node_pool;
  size_t alloc_size =
    (subpool_size (lfl_node_t) + ZOS_THREAD_TABLE_SIZE + 32);

  /* Allocate storage for the lock-free list node pool and the thread
     pointer hashtable, plus extra so we can align them both to 16
     bytes.  */
  node_pool = __storage_obtain_simple ((unsigned int) alloc_size);
  node_pool = (object_pool *) (((uintptr_t) node_pool + 15) & ~15);
  __zos_tp_table =
    (lf_hash_table *) (((uintptr_t) node_pool
			+ subpool_size (lfl_node_t) + 15) & ~15);

  __obj_pool_initialize (node_pool, sizeof (lfl_node_t),
			 _Alignof (lfl_node_t), false, node_init);

  __lf_hash_table_initialize (__zos_tp_table, ZOS_THREAD_BUCKETS,
			      lfl_set, node_pool);

  __zos_set_thread_pointer (addr);
}

static lfl_status
find_a_thread (uint64_t key, uint64_t val,
	       uint64_t tag, volatile uint64_t *tagptr,
	       void *callback_args, lfl_list_t *list)
{
  uint64_t *data = (uint64_t *) callback_args;
  data[0] = key;
  return CONTINUE;
}

/* Clean up parent's thread pointer setup.
   Should only be called once per process, right after fork()
   in the child.  */

void
__zos_cleanup_thread_pointer (void *addr)
{
  uint64_t data[1];

  if (addr == NULL) /* this call is just to resolve this function */
    return;

  for (int i = 0; i < __zos_tp_table->size; i++)
    {
      lfl_list_t *list = __zos_tp_table->buckets + i;
      while (1)
	{
	  data[0] = 0;
	  __lfl_for_each (find_a_thread, data, UINT64_MAX, list);
	  if (data[0] == 0)
	    break;
	  __lf_hash_table_pop (data[0], __zos_tp_table);
	}
    }
  __zos_set_thread_pointer (addr);
}
