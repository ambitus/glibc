/* Anonymous mappings for z/OS.
   Copyright (C) 2019-2020 Free Software Foundation, Inc.
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

/* Since MAP_ANONYMOUS mappings are not provided by the operating
   system, but other memory allocation primitives are. We see fit to
   attempt to implement anonymous private mappings in terms of those
   primitives.

   Bookkeeping for anonymous mappings is tricky.

   We need one structure per mapping to keep track of where it starts,
   where it ends, etc. The problematic part is allocating storage for
   those structures. Normally, we would just use malloc, but mmap
   MAP_ANONYMOUS might get called from within malloc, so using malloc
   to allocate storage from within the mmap call risks infinite mutual
   recursion or putting the heap into an invalid state. We can't use
   brk, since that also might confuse malloc. That leaves us with
   z/OS storage primitives to manage our storage.

   Furthermore, mmap() must be AS-Safe and AC-Safe, so our implementation
   must be lock-free.

   Instead of allocating each structure one by one, we allocate in
   pools, partly for debuggability and partly for performance reasons.
   A single pool will be big enough for most programs, which very rarely
   use more than a few dozen anonymous mmaps, but we allocate more pools
   if needed.

   We use a combination of two allocation primitives provided by the
   operating system, STORAGE and IARV64. The first allocates memory
   below the 31-bit bar with an 8-byte resolution and either 8-byte or
   page alignment, though we always request storage starting on a page
   boundary and round all requests up to the nearest page for
   compatibility with other platforms. The second facility allocates
   memory above the 31-bit bar, in 1MB increments on MB boundaries.
   31-bit memory is in short supply and is essential for certain
   operations, so we should try to use 64-bit memory whenever
   possible.  */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/param.h>
#include <atomic.h>
#include <lock-free.h>
#include <zos-core.h>
#include <zos-mmap.h>
#include <map-info.h>
#include <sys/mman.h>
#include <assert.h>

/* Define this to 1 to avoid an extremely unlikely memory leak, at the
   cost of an indeterminitely long period where the application will not
   be able to see signals.  */
#define BE_SIGNAL_SAFE  0
#if BE_SIGNAL_SAFE
# include <sysdep.h>
# include <internal-signals.h>
#endif

#define MB  (1024UL * 1024UL)

/* List of allocations that we are keeping track of.  */
lfl_list_t __zos_tracked_allocs = {0};

/* Object pool for map infos.  */
object_pool __alloc_info_pool = {0};

static inline struct map_info *
allocate_info (void)
{
  struct map_info *info = __obj_pool_alloc_block (&__alloc_info_pool);
  return info;
}


static inline void
free_info (struct map_info *info)
{
  /* Ignore return value. We don't have a meaningful recourse
     for something going wrong here, other than potentially
     aborting, which might not be what we want, depending on the
     situation.  */
  __obj_pool_free_block (info, &__alloc_info_pool);
}


/* Increment the given info's refcount.  */
static inline lfl_status
info_incref (struct map_info *info)
{
  uint64_t refcount = atomic_load_acquire (&info->u.refcount);
  do
    {
      /* The low bit of refcount is a flag to show it is valid, so the
	 field is incremented/decremented by 2 for each reference to
	 avoid a doing a shift. This means that refcount == 1 represents
	 no existing references (refcount == 0 shouldn't be valid).

	 When an info has no referents, it is either deleted or being
	 deleted. That also means that any path we took to get to the
	 info was itself deleted, so we must restart the search and get
	 the lay of the land again. If the low bit of refcount is not 1,
	 then the info we are examining must have been deallocated and
	 reused, and we must again restart.  */
      if (refcount <= 1 || (refcount & 1) == 0)
	return RESTART;
    }
  while (!__atomic_compare_exchange_n (&info->u.refcount, &refcount,
				       refcount + 2, true,
				       __ATOMIC_ACQ_REL,
				       __ATOMIC_ACQUIRE));
  return CONTINUE;
}


/* Decrement the given info's refcount, and deallocate it if it falls
   to 0.  */
static inline lfl_status
info_decref (struct map_info *info)
{
  uint64_t refcount = atomic_load_acquire (&info->u.refcount);
  do
    {
      /* Decrefs should only be done when the given info
	 can be garuanteed to be a valid allocation owner.  */
      assert (!(refcount <= 1 || (refcount & 1) == 0));
    }
  while (!__atomic_compare_exchange_n (&info->u.refcount, &refcount,
				       refcount - 2, true,
				       __ATOMIC_ACQ_REL,
				       __ATOMIC_ACQUIRE));

  /* If we removed the last reference, free the node and deallocate
     the underlying storage mechanism.  */
  if (refcount == 3)
    {
      if ((uintptr_t) info->addr > PTR31_BAR)
	{
	  uint32_t ret;
	  __iarv64_detach (info->memobj_start, &ret, NULL);
	  free_info (info);
	  if (ret > 0)
	    return FAIL;
	}
      else
	{
	  /* Nothing necessary for 31-bit allocations.  */
	}
    }

  return CONTINUE;
}


/* Check if the map_info of the found node has the same address
   and the same reuse value. If it does, then this node is the
   same node as the one we previously examined.  */
static lfl_status
validate_splice (uint64_t key, uint64_t val,
		 uint64_t tag, volatile uint64_t *tagptr,
		 void *args, lfl_list_t *list)
{
  struct map_info *found_info = (struct map_info *) val;
  struct map_info *reference_info = args;

  if (found_info != reference_info
      || atomic_load_acquire (tagptr) != tag)
    return FAIL;

  return CONTINUE;
}


/* Helper function to make a new map_info out of a portion of an old
   one. Increments the reference count of the old info's owner (which
   might be the old info itself).  */
static inline lfl_status
add_split_map (struct map_info *owner, uintptr_t addr, uint64_t len,
	       lfl_list_t *list)
{
  /* Allocate a map_info.  */
  struct map_info *new_info = allocate_info ();

  if (!new_info)
    return FAIL;

  /* It's safe to increment refcount here even if the
     given info was removed from the list, deallocated, and reused,

     refcount if that was the case before we act on anything
     visible to other threads.  */
  lfl_status res = info_incref (owner);

  if (res != CONTINUE)
    {
      free_info (new_info);
      return res;
    }

  new_info->addr = (void *) addr;
  new_info->length = len;
  new_info->memobj_start = NULL;
  new_info->u.obj_owner = owner;

  __lfl_insert (addr, (uintptr_t) new_info, list);

  return CONTINUE;
}


/* Do the deallocation operation appropriate for the given allocation
   type (determined by the address). Also maybe deallocate info, if we
   can.

   This action must be idempotent.  */
static inline int64_t
dealloc_range (uint64_t start, uint64_t len)
{
  if (start > PTR31_BAR)
    {
      uint32_t ret;
      /* Discard all data on the given pages and free their backing
	 storage.  */
      __iarv64_discard_data_one ((void *) start, len / 4096, &ret, NULL);
      if (ret > 6)
	return -1;
      return 0;
    }
  else
    {
      /* z/OS TODO: Make sure this is idempotent.  */
      return __storage_release ((uint32_t) start, (uint32_t) len, 0, 0);
    }
}


struct split_map_args
{
  /* Start of unmap region.  */
  uintptr_t start;
  /* End of unmap region.  */
  uintptr_t end;
  /* Highest key seen so far (avoid re-deallocating on restarts).  */
  uintptr_t max;
  /* Length of the region to delete mappings matching the given owner.
     The region starts at max.  */
  uintptr_t prune_len;
  /* The allocation owner to check against when cleaning up.  */
  struct map_info *prune_owner;
  /* Prospective return code.  */
  int ret;
  /* Don't do the actual deallocation.  */
  bool no_dealloc;
};


/* Examine a list node and determine if it needs to be split to fulfill
   the given unmap request.  Used as the body of a foreach loop. */
static lfl_status
maybe_split_map (uint64_t key, uint64_t val,
		 uint64_t tag, volatile uint64_t *tagptr,
		 void *callback_args, lfl_list_t *list)
{
  uintptr_t start1, end1;
  uintptr_t dealloc_start, dealloc_len;
  lfl_list_t new_nodes;
  struct split_map_args *args = callback_args;
  uintptr_t start0 = args->start, end0 = args->end;
  struct map_info *info = (struct map_info *) val;

  start1 = (uintptr_t) key;
  end1 = start1 + info->length;

  /* If the deallocation range is outside this mapping entirely,
     or we have already processed it, skip it.  */
  if (end0 <= start1 || start0 >= end1 || end1 <= args->max)
    {
skip:
      /* If the node has been operated on since we accessed its data,
	 some part of our work may have been invalidated so we must start
	 over.  */
      if (atomic_load_acquire (tagptr) != tag)
	return RESTART;
      return CONTINUE;
    }

  struct map_info *owner = (info->memobj_start == NULL
			    ? info->u.obj_owner : info);

  /* If we're just cleaning up the results of a half-successful
     deallocation attempt, skip all maps in the prune range that
     aren't part of the mapping we are pruning.  */
  if (args->prune_len != 0)
    {
      if (args->max + args->prune_len <= start1)
	/* We've made it out of the prune range, stop pruning.  */
	args->prune_len = 0;
      else if (args->prune_owner != owner)
	goto skip;
    }

  __lfl_initialize (&new_nodes, lfl_set, __zos_tracked_allocs.mempool);

  /* Find start of part to unmap. Split map if necessary.  */
  lfl_status res;
  if (start0 > start1)
    {
      dealloc_start = start0;

      res = add_split_map (owner, start1, start0 - start1, &new_nodes);
      if (res == RESTART)
	goto restart;
      else if (res == FAIL)
	/* We continue even if we run into errors.  */
	args->ret |= -1;
    }
  else
    dealloc_start = start1;

  /* Find end of part to unmap. Split map if necessary.  */
  if (end0 < end1)
    {
      dealloc_len = end0 - dealloc_start;

      res = add_split_map (owner, end0, end1 - end0, &new_nodes);
      if (res == RESTART)
	goto restart;
      else if (res == FAIL)
	/* We continue even if we run into errors.  */
	args->ret |= -1;
    }
  else
    dealloc_len = end1 - dealloc_start;

  /* If the node has been operated on, assume some part
      of our work has been invalidated and start over.  */
  if (atomic_load_acquire (tagptr) == tag)
    {
      /* We should not access info after this point.  */

      /* If we're just pruning, we should not dealloc again.  */
      if (!args->no_dealloc
	  && (args->prune_len == 0
	      || args->max + args->prune_len <= dealloc_start))
	{
	  /* Do the deallocation operation.

	     NOTE: Doing the deallocation before removing the node from
	     the list garuantees that we can safely reuse the given
	     range at some later point. This order of operations
	     requires that the deallocation operation itself MUST be
	     idempotent.  */
	  args->ret |= dealloc_range (dealloc_start, dealloc_len);
	}

      /* Remove the node from the list.  */
      if (__lfl_remove_and_splice (key, validate_splice, info,
				   &new_nodes, list))
	{
	  /* Decrement the refcount of the allocation owner, and delete
	     the node we removed if it was not an allocation owner.  */
	  if (info != owner)
	    free_info (info);

	  lfl_status dec_status = info_decref (owner);
	  if (dec_status == FAIL)
	    args->ret |= -1;
	  else
	    assert (dec_status == CONTINUE);

	  /* Adjust the max handled address, and, if we're pruning,
	     adjust the prune range.  */
	  if (args->prune_len != 0)
	    args->prune_len -= (dealloc_start + dealloc_len - args->max);
	  args->max = dealloc_start + dealloc_len;

	  return CONTINUE;
	}

      /* If we got here, the mapping did not exist when we tried to
	 remove it, but we did deallocate it. That may mean that
         a subsequent allocation forcibly replaced or split this map
         before we got the opportunity to do so ourselves.

         In this case, we're going to restart, and we need to clean
         clean up any remenants of the deallocation range that still
         exist when we get back to this range. Those will be the parts
         of any info overlapping the range we dealloced with the same
         owner. This may happen recursively.  */
      if (args->max + args->prune_len > dealloc_start)
	{
	  uintptr_t adjlen = args->max + args->prune_len - dealloc_start;
	  args->prune_len = (dealloc_len > adjlen ?
			     dealloc_len : adjlen);
	}
      else
	args->prune_len = dealloc_len;
      args->max = dealloc_start;
      args->prune_owner = owner;
    }

  /* Restart. clean up any nodes we've created.  */
  lfl_node_t *n1, *n2;
restart:
  n1 = (lfl_node_t *) new_nodes.start.next.nextptr;
  if (n1)
    {
      struct map_info *l_info = (struct map_info *) n1->data.val;
      assert (l_info->memobj_start == NULL && !(l_info->u.refcount & 1));
      info_decref (l_info->u.obj_owner);
      free_info (l_info);
      n2 = (lfl_node_t *) n1->next.nextptr;
      __lfl_remove (n1->data.key, &new_nodes);
      if (n2)
	{
	  l_info = (struct map_info *) n2->data.val;
	  assert (l_info->memobj_start == NULL
		  && !(l_info->u.refcount & 1));
	  info_decref (l_info->u.obj_owner);
	  free_info ((struct map_info *) n2->data.val);
	  __lfl_remove (n2->data.key, &new_nodes);
	}
    }

  return RESTART;
}


static inline int
do_unmap (void *addr, size_t length, bool no_dealloc)
{
  struct split_map_args callback_args = { (uintptr_t) addr,
					  (uintptr_t) addr + length,
					  0, 0, NULL, 0, no_dealloc };

  if (!length || !addr)
    return 0;

  /* Round length up to a page boundary.  */
  length = (length + 4096UL - 1) & ~(4096UL - 1);

  __lfl_for_each (maybe_split_map, &callback_args,
		  (uintptr_t) addr + length - 1, &__zos_tracked_allocs);

  return callback_args.ret;
}


/* Remove parts of mappings that intersect with the
   range [addr, addr+len).

   It preforms one pass over a given range, deallocating each
   address at most once.

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__unmap_anon_mmap (void *addr, size_t length)
{
  return do_unmap (addr, length, false);
}


static inline void *
create_iarv64_map (void *addr __attribute__ ((unused)), size_t len,
		   int prot, int flags, struct map_info *info,
		   size_t *actual_len)
{
  void *mapping;
  uint64_t real_len = (len + MB - 1) & ~(MB - 1);

  /* z/OS TODO: IARV64 inaddr. Try to support addr.  */
  /* z/OS TODO: Associate this storage with the initial task or the
     jobstep task.  */
  /* z/OS TODO: See if we can support shared memory objects.  */
  mapping = __iarv64_getstorage (real_len / MB, 0,
				 GUARDLOC_HIGH, NULL, NULL);

  if (mapping == NULL)
    return MAP_FAILED;

  info->addr = mapping;
  info->length = len;
  info->memobj_start = mapping;
  info->u.refcount = 3;

  if ((prot & PROT_WRITE) != PROT_WRITE)
    {
      /* z/OS TODO: Should this cover the whole memory object?
         Pro: Avoids demotion of large pages to 4K pages.
         Con: Prevents potential usage of excess allocation area on
	 last page.  */
      __iarv64_protect_one (mapping, len / 4096, NULL, NULL);
    }

  *actual_len = real_len;
  return mapping;
}


static inline void *
create_storage_map (void *addr __attribute__ ((unused)), size_t len,
		    int prot, int flags, struct map_info *info,
		    size_t *actual_len)
{
  void *mapping;

  /* Allocate the mapping itself.  */
  /* mapping = __storage_obtain (len, __ipt_zos_tcb, noexec, true);  */
  /* z/OS TODO: change this to the above when the regular call is
     working.  */
  mapping = __storage_obtain_simple (len);

  if (mapping == NULL)
    return MAP_FAILED;

  info->addr = mapping;
  info->length = len;
  info->memobj_start = mapping;
  info->u.refcount = 3;

  *actual_len = len;
  /* z/OS TODO: non-PROT_WRITE and PROT_EXEC handling for STORAGE.  */
  return mapping;
}


/* Create an anonymous mapping and add it to the list.
   TODO: Right now we don't abide by addr at all, which is problematic
   since some applications depend on being able to allocate multiple
   contiguous mappings.  */
void *
__create_anon_mmap (void *addr __attribute__ ((unused)), size_t len,
		    int prot, int flags)
{
  void *mapping;
  size_t actual_len;
  struct map_info *info;
  /* TODO: Right now we completely ignore addr. Use EXPLICIT storage
     requests to implement MAP_FIXED, and maybe take addr into
     consideration in the general cases as well.  */

  /* We're going to need a new map_info.  */
  info = allocate_info ();
  if (info == NULL)
    return MAP_FAILED;

  /* Allocate pages at a time. Round up to the nearest page.  */
  len = (len + 4096UL - 1) & ~(4096UL - 1);

#if BE_SIGNAL_SAFE
  sigset_t set;
  /* We can't risk a signal or cancellation request causing this thread
     to die (without the rest of the process dying) after it has
     allocated memory but before it has had the opportunity to add its
     memory area to the list.
     z/OS TODO: Determine if the tiny potential for memory leaks is
     worth the guaranteed period of unresponsiveness to signals.  */
  __libc_signal_block_all (&set);
#endif

  /* z/OS TODO: Reuse unused parts of memory objects to satisfy smaller
     allocation requests without actually allocating more storage.  */

  /* Try to use IARV64 if the requested size is greater than a certain
     amount.  */
  if (len >= 1024 * 1024)
    {
      mapping = create_iarv64_map (addr, len, prot, flags, info,
				   &actual_len);
      if (mapping == MAP_FAILED)
	/* Fall back to storage.  */
	mapping = create_storage_map (addr, len, prot, flags, info,
				      &actual_len);
    }
  else
    {
      mapping = create_storage_map (addr, len, prot, flags, info,
				    &actual_len);
      if (mapping == MAP_FAILED)
	/* Fall back to iarv64.  */
	mapping = create_iarv64_map (addr, len, prot, flags, info,
				     &actual_len);
    }

  if (mapping == MAP_FAILED)
    {
#if BE_SIGNAL_SAFE
      __libc_signal_restore_set (&set);
#endif
      free_info (info);
      return MAP_FAILED;
    }

  /* Insert the new node into the list.

     It's possible and valid for a node (or several) to already exist
     that represent the given range. This may occur when another thread
     is currently in the process of splitting or deallocating the
     given nodes' represented memory area, and has already done the
     actual physical deallocation, but not yet removed the node from
     the list. If so, the correct thing to do is to try and
     preemptively split or remove the node, so that we can insert
     ours in the right place.  */
  do_unmap (mapping, actual_len, true);
  __lfl_insert ((uintptr_t) mapping, (uintptr_t) info,
		&__zos_tracked_allocs);

#if BE_SIGNAL_SAFE
  __libc_signal_restore_set (&set);
#endif
  return mapping;
}


/* Set the read/write permissions for the given address range.
   z/OS TODO: this is currently unimplemented.

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__set_protections_anon_mmap (void *addr, size_t length, int prot)
{
  return -1;
}


/* Force all pages in range to be resident.
   z/OS TODO: this is currently unimplemented.

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__mlock_anon_mmap (void *addr, size_t length)
{
  return -1;
}


/* Release all pages in range from a previous mlock.
   z/OS TODO: this is currently unimplemented.

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__munlock_anon_mmap (void *addr, size_t length)
{
  return -1;
}
