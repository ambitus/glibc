/* Extremely early init for z/OS.
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

/* z/OS TODO: figure out how much of this file needs to be adapted to
   support shared cases.  */

/* The main work is done in the generic function.  */
#define LIBC_START_MAIN generic_start_main
#define LIBC_START_DISABLE_INLINE
#undef LIBC_START_MAIN_AUXVEC_ARG
#undef MAIN_AUXVEC_ARG
#include <csu/libc-start.c>

#include <ldsodefs.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <alloca.h>
#include <zos-utils.h>
#include <zos-core.h>
#include <zos-estaex.h>
#include <zos-futex.h>
#include <zos-init.h>
#include <lock-free.h>
#include <map-info.h>
#include <zos-mmap.h>
#include <sir.h>

/* z/OS TODO: What do we need to change here to handle shared cases?
   Where is the shared env init?  */

#define roundup16(val) (((uintptr_t) (val) + 16 - 1) & ~(16 - 1))
#define ZOS_FUTEX_BUCKETS 64

#define ZOS_FUTEX_TABLE_SIZE						\
  (sizeof (lf_hash_table) + sizeof (lfl_list_t) * ZOS_FUTEX_BUCKETS)

#define PERM_STORE_CONST_SIZE						\
  /* Hash table for thread pointers.  */				\
  (/* roundup16 (ZOS_THREAD_TABLE_SIZE) + */				\
   /* Hashed wait queue for futexes.  */				\
   roundup16 (ZOS_FUTEX_TABLE_SIZE) +					\
   /* lfl_node object pool (shared between the above) header.  */	\
   roundup16 (subpool_size (lfl_node_t))				\
   /* PE object pool header.  */					\
   /* roundup16 (subpool_real_size (16, 1)) + */			\
   /* Anon map data object pool header.	*/				\
   /* roundup16 (subpool_real_size_type (struct map_info))  */)


/* A very basic fixed size non-freeing allocator that somewhat simplifies
   early initialization allocation.  */

struct storage
{
  void *next;
  size_t used;
  size_t size;
};

static inline void __attribute__ ((always_inline))
perm_store_init (struct storage *store, size_t total_size)
{
  void *res = __storage_obtain (roundup16 (total_size + 15), false, false);

  if (res == NULL)
    CRASH_NOW ();

  store->used = 0;
  store->next = (void *) roundup16 (res);
  store->size = total_size;
}

/* Align everything to 16 bytes, or not at all.  */
static inline void * __attribute__ ((always_inline))
perm_store_alloc (struct storage *store, size_t size, bool align)
{
  uintptr_t new_next, ret, prev;

  ret = prev = (uintptr_t) store->next;
  if (align)
    ret = roundup16 (prev);
  new_next = ret + size;

  size_t new_used = store->used + new_next - prev;
  if (new_used > store->size)
    CRASH_NOW ();
  store->used = new_used;
  store->next = (void *) new_next;

  return (void *) ret;
}

/* Initializer for object pool elements.  */
static void
node_init (void *addr)
{
  ((lfl_node_t *) addr)->next.tag = 0;
}

static inline void __attribute__ ((always_inline))
global_structures_init (void)
{
  struct storage store;
  object_pool *node_pool;

  perm_store_init (&store, PERM_STORE_CONST_SIZE);

  /* The lock-free list node pool, to be shared between all lists.  */
  node_pool = perm_store_alloc (&store, subpool_size (lfl_node_t), true);

  __obj_pool_initialize (node_pool, sizeof (lfl_node_t),
			 _Alignof (lfl_node_t), false, node_init);
  __obj_pool_initialize (&__wait_token_pool, sizeof (wait_token),
			 _Alignof (wait_token), false, NULL);
  __obj_pool_initialize (&__alloc_info_pool, sizeof (struct map_info),
			 _Alignof (struct map_info), false, NULL);

  /* Initialize primary data structures.  */

  /* Thread pointer hashtable is initialized elsewhere.  */
  /* Futex hashtable.  */
  __zos_futex_table = perm_store_alloc (&store, ZOS_FUTEX_TABLE_SIZE, true);
  __lf_hash_table_initialize (__zos_futex_table, ZOS_FUTEX_BUCKETS,
			      lfl_hashed_wait_queue_bucket, node_pool);

  /* mmap() MAP_ANONYMOUS accounting structures.  */
  __lfl_initialize (&__zos_tracked_allocs, lfl_set, node_pool);
}

#ifndef SHARED
/* Get load address, avoiding load-time relocations.  */

static inline Elf64_Addr
startup_load_address (void)
{
  Elf64_Addr addr;

  __asm__ (".hidden	__ehdr_start\n\t"
	   "larl	%0, __ehdr_start\n\t"
	   : "=d" (addr));

  return addr;
}
#endif

/* NOTE: arg_info is a pointer to the start of where the program args
   and envs are on the stack.  */

int
__libc_start_main (int (*main) (int, char **, char ** MAIN_AUXVEC_DECL),
		   void *arg_info,
		   void (*rtld_fini) (void),
		   __typeof (main) init,
		   void (*fini) (void),
		   void *stack_end)
{
  void *cookie;
  char **args_and_envs;
  int argc;

#ifndef SHARED
  /* We always need to record our load address for TLS initialization,
     so we need to manually record it for static programs.
     z/OS TODO: We might need other initialization stuff, should we call
     something like _dl_relocate_static_pie here?  */
  struct link_map *main_map = GL(dl_ns)[LM_ID_BASE]._ns_loaded;
  main_map->l_addr = startup_load_address ();
#endif

  /* Save the IPT Task Control Block address. This will be needed
     throughout the life of the program.  */
  __ipt_zos_tcb = TCB_PTR;

#ifdef SHARED
  /* The dynamic linker already translated our args and envs for us.  */
  cookie = arg_info;

  /* Register our SIR.  */
  set_up_signals ();
#else
  /* Process program arguments and environ, set up signals, and register
     ourselves as an ASCII program. Args and envs are copied onto the
     stack.  */
  cookie = ESSENTIAL_PROC_INIT (alloca, arg_info, set_up_signals, NULL);
  _dl_psuedo_aux_init ();
#endif

  /* Skip the unused ELF header word.  */
  argc = (int) *(long int *) ((uintptr_t) cookie + 8);
  args_and_envs = (char **) ((uintptr_t) cookie + 16);

  int disable_estaex = 0;
  for (char **envs = args_and_envs + argc + 1; *envs; envs++) {
    if (0 == strcmp(*envs, "DISABLE_ESTAEX=YES"))
      disable_estaex = 1;
  }
  /* Set up an ESTAEX handler for debugging. */
  if (!disable_estaex) {
    int estaex_set = __set_estaex_handler (__estaex_handler_dump, NULL);
    if (estaex_set != 0)
      CRASH_NOW ();
  }

  /* Obtain storage for and initialize the major global structures.  */
  global_structures_init ();

#ifndef SHARED
  /* Establish conversion for the standard file descriptors.
     We only need to do this for static programs because otherwise the
     dynamic linker should have already done it for us.  */
  __libc_set_conv_standard_fds ();
#endif

  /* Do the regular __libc_start_main stuff.  */
  generic_start_main (main, argc, args_and_envs,
		      init, fini, rtld_fini, stack_end);

  /* This should never be reached.  */
  abort ();
}
