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

#ifndef _LOCK_FREE_H
#define _LOCK_FREE_H 1

#include <stdbool.h>
#include <stdint.h>
#define __need_size_t
#include <stddef.h>


/* Object pools.  */

typedef struct object_pool_subpool
{
  /* A bitmap, 0 means free slot, 1 means used.  */
  uint32_t usage;

  /* Pointer to next subpool on the subpool list.  */
  struct object_pool_subpool *next;

  void *blocks;
} object_subpool;


typedef struct
{
  /* Size of the blocks for this allocator.  */
  size_t block_size;

  /* Alignment of the blocks for this allocator.  */
  unsigned int block_align;

  /* Whether or not we can deallocate empty subpools.
     TODO: This functionality isn't implemented yet.  */
  int can_delete;

  /* Function to run on each block during subpool initialiaztion.  */
  void (*initializer)(void *);

  /* Pointer the the start of the subpool list.  */
  object_subpool *start;
} object_pool;


/* Lock-Free list/set/wait queue.  */

typedef enum
{
  /* We are implementing a set (aka a hash table bucket). Insertions
    will fail if the specified key is already in the list.  */
  lfl_set,

  /* We are implementing a hashed wait queue bucket. Insertions and
    removals of data with the same key adhere to queue behavior, if
    they can be proven to have a total order.  */
  lfl_hashed_wait_queue_bucket
} lfl_list_type;


typedef struct
{
  uint64_t key;
  uint64_t val;
} lfl_data_t;


typedef struct
{
  /* The order of these two fields is significant.  */

  /* The pointer to the next element. This is not declared as an
     lfl_node_t pointer because marking the pointer violates its
     alignment requirements, which might invoke UB.  */
  uintptr_t nextptr;

  /* A tag value that is monotonically incremented at the same time as
     changes to the containing node. It persists through node reuse. Used
     to avoid ABA problems.  */
  uint64_t tag;
} lfl_tagged_marked_ptr;


/* Only 16-byte aligned 16-byte fields may be atomically accessed on
   z/Arch.  */
typedef lfl_tagged_marked_ptr __attribute__ ((__aligned__ (16)))
  lfl_atomic_tm_ptr;


/* Note that this type exceeds the alignment gaurantees of malloc
   (which gives 8-byte aligned pointers).  */
typedef struct
{
  /* The data containted in and identifying this node.
     Just a 128-bit value, the first 64 bits of which are used to sort
     the list. When multiple nodes with the same key are added to the
     list, the node added last will be added later in the list.

     While the different parts of the data are treated separately,
     they are accessed as a unit, atomically. 128-bit atomic loads on
     z/Arch require 16-byte alignment.  */
  lfl_data_t data __attribute__ ((__aligned__ (16)));

  /* The pointer to the next lfl_node_t.

     This field should be checked before the node's data is examined.
     If the next pointer is marked, the data becomes meaningles

     This field must be accessed atomically. 128-bit atomic loads on
     z/Arch require 16-byte alignment.  */
  volatile lfl_atomic_tm_ptr next;
} lfl_node_t;


/* A list head.

   Each list has a dummy head node.  */
typedef struct
{
  /* What kind of data structure we are implementing.  */
  lfl_list_type type;

  /* The object pool we are using to allocate list nodes.  */
  object_pool *mempool;

  /* list head.  */
  lfl_node_t start;
} lfl_list_t;


typedef bool (*lfl_action)(uint64_t key, uint64_t val, void *misc,
			   uint64_t tag, volatile uint64_t *tagptr,
			   lfl_list_t *list);


/* Hashtable/hashed wait queue.  */

typedef struct
{
  size_t size;
  lfl_list_t buckets[];
} lf_hash_table;


extern void __lfl_initialize (lfl_list_t *list, lfl_list_type type,
			      object_pool *node_pool);
extern bool __lfl_insert (uint64_t key, uint64_t val, lfl_list_t *list);
extern uint64_t __lfl_get (uint64_t key, lfl_list_t *list);
extern uint64_t __lfl_remove (uint64_t key, lfl_list_t *list);
extern uint64_t __lfl_remove_and_splice (uint64_t key, lfl_action action,
					 void *cmp_val,
					 lfl_list_t *sublist,
					 lfl_list_t *list);
extern void __lfl_for_each (lfl_action action, void *cmp_val,
			    lfl_list_t *list);

extern void __lf_hash_table_initialize (lf_hash_table *table,
					size_t num_buckets,
					lfl_list_type type,
					object_pool *node_pool);

extern void __obj_pool_initialize (object_pool *new_pool,
				   size_t block_size, size_t align,
				   int can_delete,
				   void (*initializer)(void *));
extern void *__obj_pool_alloc_block (object_pool *pool);
extern bool __obj_pool_free_block (void *block, object_pool *pool);

libc_hidden_proto (__lfl_initialize)
libc_hidden_proto (__lfl_insert)
libc_hidden_proto (__lfl_get)
libc_hidden_proto (__lfl_remove)
libc_hidden_proto (__lfl_remove_and_splice)
libc_hidden_proto (__lfl_for_each)
libc_hidden_proto (__lf_hash_table_initialize)
libc_hidden_proto (__obj_pool_initialize)
libc_hidden_proto (__obj_pool_alloc_block)
libc_hidden_proto (__obj_pool_free_block)


/* Size to allocate for a hash table.  */
static inline size_t
hashtable_size (size_t num_buckets)
{
  return sizeof (lf_hash_table) + sizeof (lfl_list_t) * num_buckets;
}


/* This is a totally new, unencumbered hash function that I whipped up
   just for this. It's not perfect, but it performs reasonably well,
   generally achieving avalanch. It randomizes the low order 50 bits
   quite well, especially the low order 16 bits, so it is particularly
   suitable for use in a hashtable.  */
#define tiny_hash(key)						\
  ({								\
    uint64_t _key = (uint64_t) (key);				\
    _key = (~_key) * 269;					\
    _key += (_key >> 20) + (_key << 34);			\
    _key *= 5639;						\
    _key += ((_key * 87793) >> 30);				\
    _key ^= (_key >> 20);					\
    _key;							\
  })


static inline bool
__lf_hash_table_put (uint64_t key, uint64_t val,
		     lf_hash_table *hash_table)
{
  uint64_t bucket = tiny_hash (key) % hash_table->size;
  return __lfl_insert (key, val, &hash_table->buckets[bucket]);
}


static inline uint64_t
__lf_hash_table_get (uint64_t key, lf_hash_table *hash_table)
{
  uint64_t bucket = tiny_hash (key) % hash_table->size;
  return __lfl_get (key, &hash_table->buckets[bucket]);
}


static inline uint64_t
__lf_hash_table_pop (uint64_t key, lf_hash_table *hash_table)
{
  uint64_t bucket = tiny_hash (key) % hash_table->size;
  return __lfl_remove (key, &hash_table->buckets[bucket]);
}


/* Calculate the real size of the memory area to be allocated for a
   subpool with the given block size and alignment. Get a safe lower
   bound on allocation size, then round it up. For storage obtain we
   need to round up to nearest multiple of 8.  */
#define subpool_real_size(block_size, align)			\
  (((sizeof (object_subpool) +					\
     block_size * 32 + align - 1) + 8 - 1) & ~(8 - 1))

#define subpool_size(type) \
  subpool_real_size (sizeof (type), _Alignof (type))

#endif /* !_LOCK_FREE_H  */
