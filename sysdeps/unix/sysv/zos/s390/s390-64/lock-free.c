/* C implementation of lock-free data structures and related things.
   Copyright (C) 2018 Rocket Software.  */

/* All data structures implemented herein are based around a lock-free
   linked list implementation that generally follows the algorithm
   laid out by Michael [1], which is a generalized improvement on
   Harris's [2] linked list.

   We modifiy it a bit to allow us to treat each hash bucket as a wait
   queue, and to work with a slab allocator.

   This allows the memory used for list nodes to be reused. It does not,
   right now, allow the any memory ever allocated for a list node to be
   freed.

   [1] Michael. High performance dynamic lock-free hash tables
       and list-based sets. In SPAA ’02, pages 73–82, New York, NY, USA,
       2002. ACM.
   [2] Harris. A Pragmatic Implementation of Non-Blocking
       Linked Lists. In Proceedings of the 15th International
       Symposium on Distributed Computing, pages 300–314,
       October 2001.

   Note that there are better lock-free hash table implementations,
   notably ones using recursive split ordering, and such an
   implementation could be built on top of the same lock-free lists.
   We use this approach because it simplifies the implementation of
   hashed wait queues.  */

/* This list will work best when concurrent accesses (especially writes)
   to the list are relatively infrequent. While it should generally
   preform quite well compared to a lock-based list, the main advantage
   over a lock-based list is that if a thread is interrupted while using
   the list, other threads (or the same thread) will still be able to use
   it. In fact, this list is not just MT-safe, but AS-safe. It is also
   nominally AC-safe, however some small bounded memory leaks are
   possible in that situation.

   While this list cannot deadlock, it will waste many cycles when
   removals from the list or additions to it are extremely
   frequent.  */

/* Restrictions of this implementation:

   Each list node has two slots for data, `key` and `val`, both
   uint64_t's. `key` is used as a unique value to identify the node.
   `val` is meaningless, arbitrary data. For both `key` and `val`,
   value 0 is reserved and used to indicate failure. Any attempt to
   insert a node with a `key` or `val` of 0 will fail.

   Whether or not multiple nodes with the same key can exist in the
   same list depends on whether it is acting as a list-based set or a
   hashed wait queue bucket.

   While it is possible to directly traverse the list as long as tagged
   nodes are properly handeled, it is HIGHLY recommended that you only
   interact with it through the functions supplied in this file.
   Lock-free programming is tricky.

   The slab allocation strategy may not be ideal for z/arch, since
   our cache lines are huge (256 bytes, which is 8 nodes worth of
   storage).  */

/* TODO: Use hazard pointers to truely free some memory.
   Specifically, using hazard pointers would make the size of unreusable
   memory a function of the maximum number of threads that have ever
   concurrently used all lists/hashtables, rather than a function
   of the maximum number of entries ever valid at once.
   TODO: Use GCC builtin atomics.
   TODO: Use constrained transactions where possible.  */

/* PORTME: At the moment, the code only works for 64-bit mode
   on z/Arch, however it could be easily modified to work in
   31-bit mode on z/Arch, and it could be ported to work on any
   arch by conditionalizing some of the following macros.  */

/* Be very careful with the dependencies here. We would prefer to
   have as few dependencies as possible for this code, since this
   code will be a dependency of anything that uses TLS, at all.
   We would rather not force everything that uses TLS to link
   against other parts of the library that it may never use.
   Furthermore, parts of this code will get called during very early
   initialization, so some things may not be set up properly.  */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <atomic.h>
#include <abort-instr.h>

#include <lock-free.h>
#include <zos-core.h>


/* We know that alignment of list nodes > 1.  */
#define LFL_MARK 0x1

/* A full memory fence. `bcr 14, 0` is only valid on (I think) z196
   and up. On older machines, `bcr 15, 0` should be used. However, it's
   important to note that on z/Arch, which this code was originally
   written for, CAS is also a full barrier in their own right, so
   we did not need to add explicit fences many places.  */
#define fence() __asm__ __volatile__ ("bcr 14, 0" ::: "memory")


/* A destructuring atomic load that loads a 16 byte tagged marked ptr
   into its constituent fields. Very useful for avoiding the ABA
   problem. See the principals of operation for more details.  */
static inline void
atomic_load_acquire_next (volatile lfl_atomic_tm_ptr *next,
			  uintptr_t *nextptr,
			  uint64_t *currtag)
{
  volatile lfl_atomic_tm_ptr tmp_tm_ptr;

  /* We need to guarantee to gcc that the pointer is always 16-byte
     aligned.  */
  volatile lfl_atomic_tm_ptr *next_aligned =
    __builtin_assume_aligned ((void *) next, 16);

  __atomic_load (next_aligned, &tmp_tm_ptr, __ATOMIC_ACQUIRE);

  *nextptr = tmp_tm_ptr.nextptr;
  *currtag = tmp_tm_ptr.tag;
}


/* Replace a node's next pointer and all associated mark and tag data in
   one 128-bit CAS. Return false if the CAS failed, otherwise
   return true.  */
static inline bool
atomic_cas_next (volatile lfl_atomic_tm_ptr *mem,
		 uintptr_t old_next, uint64_t old_tag,
		 uintptr_t new_next, uint64_t new_tag,
		 int success_memorder)
{
  /* We need to guarantee to gcc that the pointer is always 16-byte
     aligned.  */
  volatile lfl_atomic_tm_ptr *mem_aligned =
    __builtin_assume_aligned ((void *) mem, 16);

  volatile lfl_atomic_tm_ptr tmp_expected;
  volatile lfl_atomic_tm_ptr tmp_desired;
  tmp_expected.nextptr = old_next;
  tmp_expected.tag = old_tag;
  tmp_desired.nextptr = new_next;
  tmp_desired.tag = new_tag;

  return __atomic_compare_exchange (mem_aligned, &tmp_expected,
				    &tmp_desired, false,
                                    success_memorder, __ATOMIC_RELAXED);
}


/* 128-bit CAS is a memory barrier in its own right on s390, but for
   clarity we have these macros.  */
#define atomic_cas_next_relaxed(m, on, ot, nn, nt)			\
  atomic_cas_next (m, on, ot, nn, nt, __ATOMIC_RELAXED)
#define atomic_cas_next_release(m, on, ot, nn, nt)			\
  atomic_cas_next (m, on, ot, nn, nt, __ATOMIC_RELEASE)
#define atomic_cas_next_acq_rel(m, on, ot, nn, nt)			\
  atomic_cas_next (m, on, ot, nn, nt, __ATOMIC_ACQ_REL)


/* We use this instead of abort() or __libc_fatal() because certain
   failure states may occur during very early initialization, and we
   also do not want everything using TLS to depend on all the code that
   both of the aformentioned functions bring in.  */
static inline void
__attribute__ ((__noreturn__))
crash (void)
{
  ABORT_INSTRUCTION;
  __builtin_unreachable ();
}


/* Functions  */
/* Lock-Free Lists  */

/* Allocate space for an lfl node, increment its tag (which persists
   through deletions).  */
static inline lfl_node_t *
allocate_lfl_node (lfl_list_t *list)
{
  lfl_node_t *node = __obj_pool_alloc_block (list->mempool);

  if (!node)
    crash ();

  return node;
}


static inline void
free_lfl_node (lfl_node_t *node, lfl_list_t *list)
{
  atomic_fetch_add_release (&node->next.tag, 1);

  /* Ignore return value. We don't have a meaningful recourse
     for something going wrong here, other than potentially
     aborting, which might not be what we want, depending on the
     situation.  */
  __obj_pool_free_block (node, list->mempool);
}


void
__lfl_initialize (lfl_list_t *list, lfl_list_type type,
		  object_pool *node_pool)
{
  list->type = type;
  list->mempool = node_pool;

  list->start.next.tag = 0;

  /* These fields should never be seen, but we should initialize them
     for consistency in debugging.  */
  list->start.data.key = 0;
  list->start.data.val = 0;
  list->start.next.nextptr = 0;
}
libc_hidden_def (__lfl_initialize)


/* TODO: if val is used to store a pointer to more data, that data must
   be loaded before the validity check is performed, otherwise it cannot
   be assumed to be valid. Unless the nodes are examined and and
   invalidated by the same thread/single consumer. Callback?  */

/* An internal helper function.

   Return enough information about two consecutive nodes `prev` and
   `curr` to either safely delete `curr` or insert a new node immediately
   after `prev` and immediately before `curr`. Return the key of the
   `curr` element.

   This algorithm is strange, we need to give our callers a fairly
   comprehensive snapshot of the local state of the list when we finish
   traversing it, which means many return values.  */
static uint64_t
lfl_find (uint64_t key,
	  lfl_node_t **ret_prev,
	  lfl_tagged_marked_ptr *ret_prev_body,
	  lfl_tagged_marked_ptr *ret_curr_body,
	  uint64_t *ret_curr_val,
	  bool first_match,
	  lfl_action examine,
	  void *cmp_val,
	  lfl_list_t *list)
{
  int curr_was_marked;
  lfl_node_t *curr, *prev, *next;
  uint64_t curr_tag, prev_tag;
  bool stop_after_first = first_match || list->type == lfl_set;

  /* Begin the search.  */
  for (;;)
    {
      /* Set up to search the list from the beginning.  */
      uintptr_t raw_curr, raw_next;

      prev = &list->start;
      next = NULL;
      /* Get the next node, don't check for marks since the head
	 node will never be marked, and will never be removed.  */
      atomic_load_acquire_next (&prev->next, &raw_curr, &prev_tag);

      curr = (lfl_node_t *) raw_curr;
      curr_tag = 0;
      curr_was_marked = 0;

      /* Traverse the list.  */
      for (;;)
	{
	  /* If the node we are currently examining is at the end of
	     the list, report back.  */
	  if (!curr)
	    {
	      *ret_prev = prev;
	      ret_prev_body->nextptr = (uintptr_t) curr;
	      ret_prev_body->tag = prev_tag;
	      ret_curr_body->nextptr = (uintptr_t) next;
	      ret_curr_body->tag = curr_tag;
	      *ret_curr_val = 0;  /* This is arbitrary, shouldn't be
				     examined.  */
	      return 0;
	    }

	  /* Get the the next node ptr, check if current node is
	     marked.  */
	  atomic_load_acquire_next (&curr->next, &raw_next, &curr_tag);

	  next = (lfl_node_t *) (raw_next & ~LFL_MARK);
	  curr_was_marked = raw_next & LFL_MARK;

	  /* Recheck prev. See if it has changed since we last examined it.  */
	  uintptr_t raw_curr2;
	  uint64_t prev_tag2;
	  atomic_load_acquire_next (&prev->next, &raw_curr2, &prev_tag2);

	  if ((raw_curr2 & ~LFL_MARK) != raw_curr
	      || prev_tag2 != prev_tag)
	    break;  /* Try again.  */

	  if (__glibc_unlikely (curr_was_marked))
	    {
	      /* If we found a marked node, try to delete it. If we fail,
		 start over.  */
	      if (atomic_cas_next_acq_rel (&prev->next,
					   (uintptr_t) curr, prev_tag,
					   (uintptr_t) next, prev_tag + 1))
		{
		  free_lfl_node (curr, list);
		  curr_tag = prev_tag + 1;
		}
	      else
		break;
	    }
	  else
	    {
	      /* load the data for the current node.
		 Relaxed is good enough for the first one since the above
		 aquire keeps it in place.  */
	      uint64_t curr_key = atomic_load_acquire (&curr->data.key);

	      /* Recheck the node tag. If it has changed, the key value
		 we just read was invalid and we have to start over. A
		 node's data is considered to change only when its tag
		 is incremented.  */
	      if (atomic_load_acquire (&curr->next.tag) != curr_tag)
		break;

	      /* If we are a hashed wait queue (or a regular list), we
		 add new elements with the same key after all other such
		 elements. If we are a list based set (hashtable bucket),
		 or we are looking to delete from the list, then return
		 after we find the first such element.  */
	      if (curr_key >= key
		  && (stop_after_first || curr_key > key))
		{
		  bool accepted = true;
		  uint64_t curr_val =
		    atomic_load_acquire (&curr->data.val);

		  if (atomic_load_acquire (&curr->next.tag) != curr_tag)
		    break;

		  /* Check acceptance criteria, if we have any.  */
		  if (examine)
		    accepted = examine (curr_key, curr_val, cmp_val,
					curr_tag, &curr->next.tag,
					list);

		  *ret_prev = prev;
		  ret_prev_body->nextptr = (uintptr_t) curr;
		  ret_prev_body->tag = prev_tag;
		  ret_curr_body->nextptr = (uintptr_t) next;
		  ret_curr_body->tag = curr_tag;
		  *ret_curr_val = curr_val;

		  /* Return false if the node fails our acceptance
		     criteria.  */
		  return accepted ? key : 0;
		}
	      prev = curr;
	    }
	  prev_tag = curr_tag;
	  curr = next;
	}
    }
}


bool
__lfl_insert (uint64_t key, uint64_t val, lfl_list_t *list)
{
  lfl_node_t *prev, *new_node;
  lfl_tagged_marked_ptr prev_body, curr_body;
  uint64_t found_val;

  if (key == 0 || val == 0)
    return false;

  new_node = allocate_lfl_node (list);

  new_node->data.key = key;
  new_node->data.val = val;

  for (;;)
    {
      /* If we find a node with the same key:
           * If we're a set, remove and replace the entry.
	   * If we're a wait queue, we're good.  */
      uint64_t res = lfl_find (key, &prev, &prev_body, &curr_body,
			       &found_val, false, NULL, NULL, list);
      if (list->type == lfl_set && res == key)
	{
	  /* We don't care about return value.  */
	  __lfl_remove (key, list);
	  continue;
	}
      atomic_store_release (&new_node->next.nextptr, prev_body.nextptr);

      /* Do the actual insertion. If it fails, start over.  */
      if (atomic_cas_next_release (&prev->next,
				   prev_body.nextptr, prev_body.tag,
				   (uintptr_t) new_node,
                                   prev_body.tag + 1))
	return true;

    }
}
libc_hidden_def (__lfl_insert)


/* If, at some point during during a traversal of the list, it contained
   a node with the same key, return the node's val. A 0 return value
   gaurantees that when the function finished traversing the list, there
   was no node with the same key in it. A true value does not imply that
   when the function returns, such a node will still be in the list.  */

uint64_t
__lfl_get (uint64_t key, lfl_list_t *list)
{
  lfl_node_t *prev;
  lfl_tagged_marked_ptr prev_body, curr_body;
  uint64_t found_val;

  if (key == 0)
    return 0;

  if (lfl_find (key, &prev, &prev_body, &curr_body, &found_val,
		true, NULL, NULL, list) == key)
    return found_val;
  return 0;
}
libc_hidden_def (__lfl_get)


static uint64_t
do_remove (uint64_t key, lfl_action action, void *cmp_val,
	   lfl_node_t *to_insert, lfl_node_t *end_node, lfl_list_t *list)
{
  lfl_node_t *prev;
  lfl_tagged_marked_ptr prev_body, curr_body;
  uint64_t found_val, removed_val;
  uintptr_t insert_ptr = (uintptr_t) to_insert;

  for (;;)
    {
      /* Return 0 if the key isn't in the set.  */
      if (lfl_find (key, &prev, &prev_body, &curr_body, &found_val,
		    true, action, cmp_val, list) != key)
	return 0;

      /* If we have been supplied a node that will be inserted into
	 the list, set up its next pointer, and use the user's value
	 for new_next. Otherwise, ignore the new_next value and use
	 the pointer to the node after curr.  */
      if (to_insert)
	atomic_store_release (&end_node->next.nextptr,
			      curr_body.nextptr);
      else
	insert_ptr = curr_body.nextptr;

      /* Try to mark the found node (and maybe insert one). If we
	 can't, then start over.  */
      if (!atomic_cas_next_release (&((lfl_node_t *)
				      curr_body.nextptr)->next,
				    curr_body.nextptr, curr_body.tag,
				    insert_ptr | LFL_MARK,
				    curr_body.tag + 1))
	continue;

      removed_val = found_val;

      /* Try to actually delete the node and free its storage (for
	 reuse).  */
      if (!atomic_cas_next_acq_rel (&prev->next,
				    prev_body.nextptr, prev_body.tag,
				    insert_ptr,
				    prev_body.tag + 1))
	free_lfl_node ((lfl_node_t *) prev_body.nextptr, list);
      else
	/* If the deletion failed, do another find which should prune
	   the node if it was not deleted by another thread.  */
	lfl_find (key, &prev, &prev_body, &curr_body, &found_val,
		  true, NULL, NULL, list);
      return removed_val;
    }
}


/* Remove the node in the set with the given key and return its val.
   Return 0 if the key isn't in the set.  */
uint64_t
__lfl_remove (uint64_t key, lfl_list_t *list)
{
  if (key == 0)
    return 0;

  return do_remove (key, NULL, NULL, NULL, NULL, list);
}
libc_hidden_def (__lfl_remove)



/* A very specialized operation to atomically remove a node with a given
   key from a list and insert one or more nodes at the location where
   that node used to be.

   This operation exists to facilitate the mmap() implementation.

   accept: a function to determine whether it is safe to preform
   the operation.
   cmp_val: a pointer used to facilitate the acceptance check, given as
   the second argument to accept. Its meaning is entirely user-
   determined.

   Restrictions:
     * The lfl must be a set.
     * Prior to the point it is inserted into the main list, the sublist
       must not be accessible by any other thread.
     * The user must be able to evaluate whether or not to perform the
       splice by examining the val of the node to be replaced.
     * The user must insure that the invariant that the lfl is sorted is
       always maintained.
     * The user must guaruantee that, if the given key exists in the set
       and its val fulfills the acceptance criteria, none of the other
       keys in the sublist may exist in the main list, and none of the
       keys in the sublist may be inserted into the main list via any
       other mechanism than a splice.
   TODO: We could remove the second to last requirement by implementing
   an unsorted set list type.  */
uint64_t
__lfl_remove_and_splice (uint64_t key, lfl_action accept,
			 void *cmp_val, lfl_list_t *sublist,
			 lfl_list_t *list)
{
  lfl_node_t *sublist_start, *sublist_end;

  if (key == 0 || list->type == lfl_set)
    return 0;

  sublist_start = (lfl_node_t *) sublist->start.next.nextptr;

  /* If sublist is empty, just do a remove.  */
  if (!sublist_start)
    return __lfl_remove (key, list);

  sublist_end = (lfl_node_t *) sublist_start;

  /* Find the end of the sublist.  */
  for (uintptr_t n = sublist_end->next.nextptr; n;
       sublist_end = (lfl_node_t *) n, n = sublist_end->next.nextptr);

  /* The trick here is to do an insert right after the node we are
     replacing, but instead of just replacing the previous next pointer
     with a pointer to the sublist, replace it with a marked pointer to
     the sublist, thereby atomically deleting the old node and inserting
     the sublist.  */
  return do_remove (key, accept, cmp_val, sublist_start,
		    sublist_end, list);
}
libc_hidden_def (__lfl_remove_and_splice)


/* Perform an action for each element in the list.

   Iteration stops when action returns true.

   Restrictions:
   * action must be written in a lock-free manner.
   * action must be idempotent.
   * If action modifies the list, then this is an O(n^2) operation.
     TODO: this could be changed.
   * If action adds nodes to the list, those nodes may or may not
     be iterated over.
   * action must load the tag value from memory and compare it against
     the provided tag value whenever it references memory whose address
     is in some way derived from key or val. If the tag has changed from
     the provided value, action should not consider the contents of the
     referenced memory to be valid.  */
void
__lfl_for_each (lfl_action action, void *cmp_val, lfl_list_t *list)
{
  lfl_node_t *prev;
  lfl_tagged_marked_ptr prev_body, curr_body;
  uint64_t found_val;

  lfl_find (UINT64_MAX, &prev, &prev_body, &curr_body, &found_val, false,
	    action, cmp_val, list);
}
libc_hidden_def (__lfl_for_each)




/* Lock-Free hashtables / hashed wait queues  */

void
__lf_hash_table_initialize (lf_hash_table *table, size_t num_buckets,
			    lfl_list_type type, object_pool *node_pool)
{
  table->size = num_buckets;
  for (size_t bucket = 0; bucket < num_buckets; ++bucket)
    __lfl_initialize (&table->buckets[bucket], type, node_pool);
}
libc_hidden_def (__lf_hash_table_initialize)


/* A fixed-size memory pool implementation

   This is a fixed-size memory pool implementation, which allows easy
   and fast reuse of memory allocated for a single type.

   This is not an ideal fixed-size memory pool implementation. It's O(n)
   allocation and O(1) deallocation, and it's very inflexible. However
   it's the simplest and safest lock-free implementation that I can
   think of right now that doesn't make use of TLS.

   I needs to be a fixed-size allocator because the chosen algorithm
   does not allow the memory used by lfl_nodes to be split or
   apportioned after it is originaly allocated, but it does allow reuse.
   It's primary designed for lfl_nodes, but it will work for any type.

   No provision is made for freeing unsed subpools by default.

   Potential TODOs:
     * Find a better implementation.
     * allow empty slabs to be freed.
     * z/OS's massive cache lines are a problem. False sharing might
       kill performance.
     * Allow dynamically-sized subpools. 32-element subpools seems
       fine for now, but might lead to address space fragmentation
       or bad performance in some situations.  */

#define raw_allocator(size)						\
  ({									\
    __typeof (size) _size = (size);					\
    if (size >= PTR31_BAR)						\
      crash ();								\
    __storage_obtain_simple ((unsigned int) _size);			\
  })
#define raw_deallocator(addr, size) \
  __storage_release ((unsigned int) (uintptr_t) addr, \
		     (unsigned int) (size_t) size,    \
		     __ipt_zos_tcb, true)

/* The granularity with which the allocator is able to allocate.  */
#define RAW_ALLOCATOR_BLKSZ 8

/* The minimum alignment of memory the allocator gives us.  */
#define RAW_ALLOCATOR_ALIGN 8

/* Check 2 assumptions:
   1. That object_subpool's alignment requirements are met by the raw
      allocator.
   2. That the raw allocator allocates in blocks that are a power
      of 2.  */
_Static_assert (_Alignof (object_subpool) <= RAW_ALLOCATOR_ALIGN, "");
_Static_assert ((RAW_ALLOCATOR_BLKSZ & (RAW_ALLOCATOR_BLKSZ - 1)) == 0,
		"");


static object_subpool *
alloc_subpool (object_pool *pool)
{
  size_t block_size = pool->block_size;
  size_t align = pool->block_align;
  size_t size = subpool_real_size (block_size, align);

  /* Allocate.  */
  uintptr_t mempool_head_start = (uintptr_t) raw_allocator (size);

  if (!mempool_head_start)
    crash ();

  /* Find where the pool blocks should start.  */
  uintptr_t mempool_data_start =
    mempool_head_start + sizeof (object_subpool);
  mempool_data_start = (mempool_data_start + align - 1) & ~(align - 1);

  object_subpool *header = (object_subpool *) mempool_head_start;
  header->usage = 0;
  header->next = NULL;
  header->blocks = (void *) mempool_data_start;

  /* Initialize contents if necessary.  */
  if (pool->initializer)
    for (int i = 0; i < 32; ++i)
      pool->initializer ((void *) (mempool_data_start
				   + i * block_size));

  return header;
}


/* Initialize a new object pool.  */
void
__obj_pool_initialize (object_pool *new_pool, size_t block_size,
		       size_t align, int can_delete,
		       void (*initializer) (void *))
{
  new_pool->block_size = block_size;
  new_pool->block_align = (unsigned int) align;
  new_pool->can_delete = can_delete;
  new_pool->initializer = initializer;
  new_pool->start = alloc_subpool (new_pool);
}
libc_hidden_def (__obj_pool_initialize)


void *
__obj_pool_alloc_block (object_pool *pool)
{
  object_subpool *curr, *prev, *new_subpool;

  if (!pool)
    return NULL;

  for (;;)
    {
      /* Always start searching from the head. Assume pool is valid.  */
      for (prev = curr = pool->start; curr != NULL;
	   prev = curr, curr = curr->next)
	{
	  for (;;)
	    {
	      uint32_t usage = atomic_load_acquire (&curr->usage);

	      /* Move on to next subpool if this one is full. Subpool is
		 full if all bits in usage are set.  */
	      if (~usage == 0)
		break;

	      /* Find a free block. TODO: FLOGR?  */
	      for (size_t pos = 0; pos < 32; ++pos)
		if ((usage & 1UL << pos) == 0)
		  {
		    uint32_t new_usage = usage | 1UL << pos;

		    /* Mark the found slot as allocated. If usage was
		       modified, start the search over from the
		       beginning.  */
		    if (atomic_compare_and_exchange_bool_acq
			  (&curr->usage, new_usage, usage))
		      break;

		    /* Return the node space address.  */
		    return (void *) ((char *) curr->blocks
				     + pos * pool->block_size);

		  }
	      __builtin_unreachable ();
	    }
	}

      /* If we reach this point, all the pools are full. Allocate a new
	 one. We need to make sure that the allocator we use is MT-Safe,
	 AS-Safe, and AC-Safe.  */
      new_subpool = alloc_subpool (pool);
      new_subpool->usage |= 1;

      /* Add our new subpool to the list. If some other user has already
	 added one, start the search over from the beginning. We don't need
	 to worry about whether or not prev is still valid since we never
	 deallocate subpools once they've been added to the list.  */
      if (!atomic_compare_and_exchange_bool_acq (&prev->next,
						 new_subpool, NULL))
	return new_subpool->blocks;

      /* Free the subpool we just allocated and start over.  */
      raw_deallocator (new_subpool,
		       subpool_real_size (pool->block_size,
					  pool->block_align));
    }
}
libc_hidden_def (__obj_pool_alloc_block)


bool
__obj_pool_free_block (void *block, object_pool *pool)
{
  object_subpool *curr;
  uintptr_t blockptr = (uintptr_t) block;
  size_t block_size = pool->block_size;

  for (curr = pool->start; curr != NULL; curr = curr->next)
    {
      uint32_t pos, usage;
      uintptr_t offset, blocks = (uintptr_t) curr->blocks;


      if (!(blocks >= blockptr
	    && (blocks + 31 * block_size) <= blockptr))
	continue;

      /* Make sure pointer points to a valid object.  */
      offset = blockptr - blocks;
      if (offset % block_size)
	return false;

      uint32_t new_usage;
      pos = offset / block_size;
      do
	{
	  usage = atomic_load_acquire (&curr->usage);

	  /* If this block is already marked as free, return false.  */
	  if ((usage & 1UL << pos) == 0)
	    return false;

	  new_usage = usage & ~(1UL << pos);
	}
      while (atomic_compare_exchange_weak_release (&curr->usage,
						   &new_usage,
						   usage));
      return true;
    }
  return false;
}
libc_hidden_def (__obj_pool_free_block)
