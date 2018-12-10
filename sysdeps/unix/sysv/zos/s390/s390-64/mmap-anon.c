/* Anonymous mappings for z/OS.
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

/* Bookkeeping for anonymous mappings is tricky.

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
   if needed.  */


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


/* List of allocations that we are keeping track of.  */
lfl_list_t __zos_tracked_allocs = {0};
libc_hidden_data_def (__zos_tracked_allocs)

/* Object pool for map infos.  */
object_pool __alloc_info_pool = {0};
libc_hidden_data_def (__alloc_info_pool)


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


/* Helper function to make a new map_info out of a portion of an old one.
   The caller must lock anon_maps_lock before calling this.  */
static int64_t
add_split_map (struct map_info *old, uintptr_t addr, uint64_t len,
	       lfl_list_t *list)
{
  /* Allocate a map_info.  */
  struct map_info *new_info =
    __obj_pool_alloc_block (&__alloc_info_pool);

  if (!new_info)
    return -1UL;

  /* New map_info is same as old except for start and length.  */
  *new_info = *old;
  new_info->start = (void *) addr;
  new_info->length = len;

  __lfl_insert (addr, (uintptr_t) new_info, list);

  return 0;
}


/* Create an anonymous mapping and add it to the list.

   TODO: Right now we don't abide by addr at all, which is problematic
   since some applications depend on being able to allocate multiple
   contiguous mappings.

   TODO: We can't mmap above 31 bits right now.  */
void *
__create_anon_mmap (void *addr __attribute__ ((unused)), size_t len,
		    int prot, int flags)
{
  /* TODO: Right now we completely ignore addr. Use EXPLICIT storage
     requests to implement MAP_FIXED, and maybe take addr into
     consideration in the general cases as well.  */

  void *mapping;
  struct map_info *info;
  bool noexec;

  /* Allocate pages at a time. Round up to the nearest page.  */
  len = (len + 4096UL - 1) & ~(4096UL - 1);

  /* If the request is too large to fit in 31-bit space, fail.
     TODO: remove this limitation.  */
  if (len > PTR31_BAR)
    return MAP_FAILED;

  noexec = flags & PROT_EXEC;

  /* Allocate the mapping itself.  */
  mapping = __storage_obtain (len, __ipt_zos_tcb, noexec, true);

  if (!mapping)
    return MAP_FAILED;

  /* Allocate and set up the mmap accounting information.  */
  info = __obj_pool_alloc_block (&__alloc_info_pool);

  if (!info)
    {
      __storage_release ((uintptr_t) mapping, len, __ipt_zos_tcb,
			 noexec);
      return MAP_FAILED;
    }

  info->start = mapping;
  info->length = len;
  info->tcbaddr = __ipt_zos_tcb;
  info->noexec = noexec;

  __lfl_insert ((uintptr_t) mapping, (uintptr_t) info,
		&__zos_tracked_allocs);

  return mapping;
}
libc_hidden_def (__create_anon_mmap)


static bool
validate_splice (uint64_t key, uint64_t val, void *args,
		 uint64_t tag, volatile uint64_t *tagptr,
		 lfl_list_t *list)
{
  /* Check if the map_info of the found node has the same address
     and the same reuse value. If it does, then this node is the
     same node as the one we previously examined.  */


  struct map_info *found_info = (struct map_info *) val;
  struct map_info *reference_info = args;

  if (reference_info != found_info
      || atomic_load_acquire (tagptr) != tag)
    return false;

  return true;
}


/* Examine a list node and determine if it needs to be split to fulfill
   the given unmap request.  Used as the body of a foreach loop. */
static bool
maybe_split_map (uint64_t key, uint64_t val, void *misc,
		 uint64_t tag, volatile uint64_t *tagptr,
		 lfl_list_t *list)
{
  uintptr_t start1, end1;
  uintptr_t dealloc_start, dealloc_len;
  lfl_list_t new_nodes;
  struct map_info *info = (struct map_info *) val;
  uintptr_t *more_args = misc;
  uintptr_t start0 = more_args[0], end0 = more_args[1];

  start1 = (uintptr_t) info->start;
  end1 = start1 + info->length;

  /* If the deallocation range is outside this mapping entirely,
      skip it.  */
  if (end0 <= start1 || start0 >= end1)
    return false;

  __lfl_initialize (&new_nodes, lfl_set, __zos_tracked_allocs.mempool);

  /* Find start of part to unmap. Split map if necessary.  */
  if (start0 > start1)
    {
      dealloc_start = start0;

      /* We continue even if we run into errors.  */
      more_args[2] |= add_split_map (info, start0, start1 - start0,
				     &new_nodes);
    }
  else
    dealloc_start = start1;

  /* Find end of part to unmap. Split map if necessary.  */
  if (end0 < end1)
    {
      dealloc_len = end0 - dealloc_start;

      /* We continue even if we run into errors.  */
      more_args[2] |= add_split_map (info, end0, end1 - end0,
				     &new_nodes);
    }
  else
    dealloc_len = end1 - dealloc_start;

  /* If the node has been operated on, assume some part
      of our work has been invalidated and start over.  */
  if (atomic_load_acquire (tagptr) == tag)
    {
      /* Release the unmapped part of this mapping.  */
      more_args[2] |=
	(uintptr_t) (long) __storage_release ((uint32_t) dealloc_start,
					      (uint32_t) dealloc_len,
					      info->tcbaddr,
					      info->noexec);

      if (__lfl_remove_and_splice (key, validate_splice, info,
				   &new_nodes, list))
	{
	  /* Remove the accounting information from the list.  */
	  free_info (info);
	  return false;
	}

      /* Presumably some other thread has removed the mapping before
	  we could, which shouldn't cause problems as long as all
	  deletions from the list go through this function.  */
    }

  /* Restart. clean up any nodes we've created.  */
  lfl_node_t *n2, *n1 =
    (lfl_node_t *) new_nodes.start.next.nextptr;
  if (n1)
    {
      n2 = (lfl_node_t *) n1->next.nextptr;
      free_info ((struct map_info *) n1->data.val);
      __lfl_remove (n1->data.key, &new_nodes);
      if (n2)
	{
	  free_info ((struct map_info *) n2->data.val);
	  __lfl_remove (n2->data.key, &new_nodes);
	}
    }

  /* Increment the node's tag, forcing any operations on the
     list currently using the node (including our own enclosing foreach)
     to start over from the beginning.  */
  atomic_fetch_add_release (tagptr, 1);
  return false;
}


/* Remove parts of mappings that intersect with the
   range [addr, addr+len).

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__unmap_anon_mmap (void *addr, size_t length)
{
  /* [0] = start of unmap region
     [1] = end of unmap region
     [2] = prospective return code.  */
  uintptr_t callback_args[3] = {(uintptr_t) addr,
				(uintptr_t) addr + length, 0};

  if (!length || !addr)
    return 0;

  __lfl_for_each (maybe_split_map, callback_args, &__zos_tracked_allocs);

  return (int) callback_args[2];
}
libc_hidden_def (__unmap_anon_mmap)


/* Set the read/write permissions for the given address range.
   TODO: this is currently unimplemented.

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__set_protections_anon_mmap (void *addr, size_t length, int prot)
{
  return -1;
}
libc_hidden_def (__set_protections_anon_mmap)


/* Force all pages in range to be resident.
   TODO: this is currently unimplemented.

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__mlock_anon_mmap (void *addr, size_t length)
{
  return -1;
}
libc_hidden_def (__mlock_anon_mmap)


/* Release all pages in range from a previous mlock.
   TODO: this is currently unimplemented.

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__munlock_anon_mmap (void *addr, size_t length)
{
  return -1;
}
libc_hidden_def (__munlock_anon_mmap)
