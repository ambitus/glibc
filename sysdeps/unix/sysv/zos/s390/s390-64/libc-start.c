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

/* TODO: figure out how much of this file needs to be adapted to
   support shared cases.  */

/* The main work is done in the generic function.  */
#define LIBC_START_MAIN generic_start_main
#define LIBC_START_DISABLE_INLINE
#undef LIBC_START_MAIN_AUXVEC_ARG
#undef MAIN_AUXVEC_ARG
#include <csu/libc-start.c>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <zos-utils.h>
#include <zos-core.h>
#include <zos-estaex.h>
#include <zos-futex.h>
#include <lock-free.h>
#include <map-info.h>

/* TODO: What to we need to change here to handle shared cases?
   Where is the shared env init?

   TODO: Signal setup.  */

#define roundup16(val) (((val) + 16 - 1) & ~(16 - 1))
#define ZOS_THREAD_BUCKETS 64
#define ZOS_FUTEX_BUCKETS 64

#define ZOS_THREAD_TABLE_SIZE						\
  (sizeof (lf_hash_table) + sizeof (lfl_list_t) * ZOS_THREAD_BUCKETS)

#define ZOS_FUTEX_TABLE_SIZE						\
  (sizeof (lf_hash_table) + sizeof (lfl_list_t) * ZOS_FUTEX_BUCKETS)

#define PERM_STORE_CONST_SIZE						\
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

struct bpxk_arg_list
  {
    uint32_t *count;
    uint32_t **lens;
    char     **vals;
  };

/* The format of %r1 when we first receive control.  */
struct bpxk_args
  {
    struct bpxk_arg_list argv;
    struct bpxk_arg_list argp;
  };

extern void (*__libc_csu_init) (int, char **, char ** MAIN_AUXVEC_DECL);
extern void (*__libc_csu_fini) (void);


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


static struct
  {
    void *next;
    size_t used;
    size_t size;
  } perm_store = { NULL, 0, 0};


static inline __attribute__ ((always_inline))
void
perm_store_init (size_t total_size)
{
  void *res;

  total_size = roundup16 (total_size);
  res = __storage_obtain_simple (total_size);

  if (!res)
    CRASH_NOW ();

  perm_store.used = 0;
  perm_store.next = res;
  perm_store.size = total_size;
}


/* Align everything to 16 bytes, or not at all.  */
static inline __attribute__ ((always_inline))
void *
perm_store_alloc (size_t size, bool align)
{
  uintptr_t new_next, ret, prev;

  ret = prev = (uintptr_t) perm_store.next;
  if (align)
    ret = roundup16 (prev);
  new_next = ret + size;

  size_t new_used = perm_store.used + new_next - prev;
  if (new_used > perm_store.size)
    CRASH_NOW ();
  perm_store.used = new_used;
  perm_store.next = (void *) new_next;

  return (void *) ret;
}


static inline __attribute__ ((always_inline))
void
global_structures_init (void)
{
  object_pool *node_pool;

  /* Initialize object pools.  */
  void node_init (void *addr)
  {
    ((lfl_node_t *) addr)->next.tag = 0;
  }

  /* The lock-free list node pool, to be shared between all lists.  */
  node_pool = perm_store_alloc (subpool_size (lfl_node_t), true);

  __obj_pool_initialize (node_pool, sizeof (lfl_node_t),
			 _Alignof (lfl_node_t), false, node_init);
  __obj_pool_initialize (&__wait_token_pool, sizeof (wait_token),
			 _Alignof (wait_token), false, NULL);
  __obj_pool_initialize (&__alloc_info_pool, sizeof (struct map_info),
			 _Alignof (struct map_info), false, NULL);

  /* Initialize primary data structures.  */

  /* Thread pointer hashtable.  */
  __zos_tp_table = perm_store_alloc (ZOS_THREAD_TABLE_SIZE, true);
  __lf_hash_table_initialize (__zos_tp_table, ZOS_THREAD_BUCKETS,
			      lfl_set, node_pool);

  /* Futex hashtable.  */
  __zos_futex_table = perm_store_alloc (ZOS_FUTEX_TABLE_SIZE, true);
  __lf_hash_table_initialize (__zos_futex_table, ZOS_FUTEX_BUCKETS,
			      lfl_hashed_wait_queue_bucket, node_pool);

  /* mmap() MAP_ANONYMOUS accounting structures.  */
  __lfl_initialize (&__zos_tracked_allocs, lfl_set, node_pool);

}


static inline __attribute__ ((always_inline))
size_t
args_min_size (struct bpxk_arg_list *arglist)
{
  size_t total = 0;
  for (uint32_t i = 0; i < *arglist->count; i++)
    total += *arglist->lens[i] + 1;

  return total;
}


static inline __attribute__ ((always_inline))
void
translate_and_copy_args (char **dest, struct bpxk_arg_list *arglist)
{
  size_t narg;
  for (narg = 0; narg < *arglist->count; narg++)
    {
      uint32_t arg_len = *arglist->lens[narg];
      char *tr_arg = perm_store_alloc (arg_len + 1, false);

      /* Translate the argument to ASCII.
	 z/OS TODO: in a pure ASCII environment, this will fail.  */
      tr_a_until_len (arglist->vals[narg], tr_arg, arg_len);

      tr_arg[arg_len] = '\0';
      dest[narg] = tr_arg;
    }
  dest[narg] = NULL;
}


int
__libc_start_main (int (*main) (int, char **, char ** MAIN_AUXVEC_DECL),
		   struct bpxk_args arg_info,
		   void (*rtld_fini) (void),
		   void *stack_end)
{

  char **args_and_envs;
  size_t total_args_size, ae_size;

  /* Do several things here.  */
  /* 1. Save the IPT Task Control Block address. This will be needed
     throughout the life of the program.  */
  __ipt_zos_tcb = __get_zos_tcb_addr ();

  /* 2. Set up an ESTAEX handler for debugging. */
  int estaex_set = __set_estaex_handler (__estaex_handler_dump, NULL);
  if (estaex_set != 0)
    CRASH_NOW ();

  /* 3. Obtain storage for and initialize the major global structures.
	We will use the same storage area for the translated
	args/environ.  */

  /*  Total size of the argv/argp array.  */
  ae_size =
    (*arg_info.argv.count + *arg_info.argp.count + 2) * sizeof (char *);

  total_args_size = roundup16 (ae_size);
  total_args_size += roundup16 (args_min_size (&arg_info.argv));
  total_args_size += roundup16 (args_min_size (&arg_info.argp));

  /* Get our storage.  */
  perm_store_init (PERM_STORE_CONST_SIZE + total_args_size);

  /* Initialize structures.  */
  global_structures_init ();

  /* 4. Convert argv and argc into a Linux-like format, and convert to
	EBCDIC. Note that argv[argc + 1] must be __environ[0].  */

  /* Allocate the argv/p array itself.  */
  args_and_envs = perm_store_alloc (ae_size, true);

  char **argp_start = &args_and_envs[*arg_info.argv.count + 1];
  translate_and_copy_args (args_and_envs, &arg_info.argv);
  translate_and_copy_args (argp_start, &arg_info.argp);

  /* 5. Do the regular __libc_start_main stuff.  */
  generic_start_main (main, *arg_info.argv.count, args_and_envs,
		      (__typeof (main)) __libc_csu_init,
		      __libc_csu_fini, rtld_fini, stack_end);

  /* This should never be reached.  */
  abort ();
}
