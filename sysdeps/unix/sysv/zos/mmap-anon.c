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
   brk, since that also might confuse malloc. That leaves us with so we
   z/OS storage primitives to manage our storage.

   Instead of allocating each structure one by one, we allocate in
   slabs, partly for debuggability and partly for performance reasons.
   A single slab will be big enough for most programs, which very rarely
   use more than a few dozen anonymous mmaps, but we allocate more slabs
   if needed. We link them together in a linked list. We don't try to
   manage fragmentation within a slab, but we do free slabs that become
   empty.  */


#include <stddef.h>
#include <stdbool.h>
#include <sys/param.h>
#include <list.h>
#include <lowlevellock.h>
#include <zos-core.h>
#include <zos-mmap.h>


/* List of storage slabs.  */
static LIST_HEAD (anon_map_slabs);

/* Lock for slabs.  */
static int slab_lock = LLL_LOCK_INITIALIZER;


/* An anonymous mapping.  */
struct map_info
{
  /* Start of the mapping.  */
  void *start;

  /* Length.  */
  uint64_t length;

  /* 31-bit addr of the associated TCB.  */
  uint32_t tcbaddr;

  /* EXECUTABLE=NO specified.  */
  bool noexec;
};


struct slab_slot
{
  /* The actual node storage.  */
  struct map_info data;

  /* Index of this map_info slot in the containing storage slab.  */
  size_t slot_num;
};


/* A slab.  */
struct map_slab
{
  /* One bit per anon map slot. Set the corresponding bit to indicate
     the slot is allocated.  */
  size_t usage;

  /* Slab slots.  */
  struct slab_slot slots[sizeof (size_t) * 8];

  /* This mapping's link on the anon_map_slabs list.  */
  list_t slab_list;

  /* This struct should be 8-byte aligned, even on 31-bit, because of
     STORAGE OBTAIN's rounding.  */
} __attribute__ ((__aligned__ (8)));


/* Allocate storage for a new map_info.
   Caller should hold slab_lock.  */
static inline struct map_info *
alloc_slab_slot (void)
{
  size_t end;
  list_t *entry;
  struct map_info *node;
  struct map_slab *new_slab;

  lll_lock (slab_lock, LLL_PRIVATE);

  /* Find a slab with free slots.  */
  list_for_each (entry, &anon_map_slabs)
    {
      struct map_slab *slab;

      slab = list_entry (entry, struct map_slab, slab_list);

      /* Slab is full if all bits in usage are set.  */
      if (~slab->usage != 0)
	{
	  for (size_t pos = 0; pos < sizeof (size_t); ++pos)
	    if ((slab->usage & 1UL << pos) == 0)
	      {
		/* Return the node space address and mark it as
		   allocated.  */
		node = &slab->slots[pos].data;
		slab->usage |= 1UL << pos;

		return node;
	      }
	}
    }

  /* All slabs are full, allocate a new one.  */
  new_slab = __storage_obtain (sizeof (struct map_slab), __ipt_zos_tcb,
			       true, false);
  if (new_slab == NULL)
    return NULL;

  /* Initialize the slot_num field for all slots.  */
  end = sizeof (new_slab->slots) / sizeof (__typeof (new_slab->slots));
  for (size_t n = 0; n < end; ++n)
    new_slab->slots[n].slot_num = n;

  /* Add new slab to anon_map_slabs.  */
  new_slab->usage = 0x1;
  node = &new_slab->slots[0].data;
  list_add (&new_slab->slab_list, anon_map_slabs.prev);

  return node;
}


/* Release storage for a map_info node.
   Caller should hold slab_lock.  */
static inline void
free_slab_slot (struct map_info *slot)
{
  size_t pos, offset;
  struct map_slab *slab;

  /* Get the address of the containing slab. Avoid UB.  */
  pos = ((struct slab_slot *) slot)->slot_num;
  offset = offsetof (struct map_slab, slots[pos]);
  slab = (struct map_slab *) ((char *) slot - offset);

  /* Mark slot as deallocated.  */
  slab->usage &= ~(1UL << pos);

  /* Free the storage if the slab is now empty and it isn't the only
     slab we have.  */
  if (slab->usage == 0 && anon_map_slabs.next != anon_map_slabs.prev)
    {
      if ((uintptr_t) slab > PTR31_BAR)
	/* Something has gone very wrong somewhere.  */
	return;

      list_del (&slab->slab_list);
      /* We don't check the return code, because we don't have any
	 recourse if the storage release fails.  */
      __storage_release ((uint32_t) (uintptr_t) slab,
			 sizeof (struct map_slab), __ipt_zos_tcb, false);
    }
}


/* Helper function to make a new map_info out of a portion of an old one.
   The caller must lock anon_maps_lock before calling this.  */
static int add_split_map (struct map_info *old, uintptr_t addr,
			  uint64_t len)
{
  /* Allocate a map_info.  */
  struct map_info *new_info = alloc_slab_slot ();

  if (!new_info)
    return -1;

  /* New map_info is same as old except for start and length.  */
  *new_info = *old;
  new_info->start = (void *) addr;
  new_info->length = len;
  return 0;
}


/* Create an anonymous mapping and add it to the list.

   TODO: Right now we don't abide by addr at all, which is problematic
   since some applications depend on being able to allocate multiple
   contiguous mappings.  */
void *
__create_anon_mmap (void *addr __attribute__ ((unused)), size_t len,
		    int prot, int flags)
{
  /* TODO: Right now we completely ignore addr. Use EXPLICIT storage
     requests to implement MAP_FIXED, and maybe take addr into
     consideration in the general cases as well.  */

  uintptr_t mapping;
  struct map_info *info;
  bool noexec;

  /* Allocate pages at a time. Round up to the nearest page.  */
  len = (len + 4096UL - 1) & ~(4096UL - 1);

  /* If the request is too large to fit in 31-bit space, fail.  */
  if (len > PTR31_BAR)
    return MAP_FAILED;

  noexec = flags & PROT_EXEC;

  /* Allocate the mapping itself.  */
  mapping = (uintptr_t) __storage_obtain (len, __ipt_zos_tcb, noexec,
					  true);

  if (!mapping)
    return MAP_FAILED;

  /* Allocate and set up the mmap accounting information.  */
  lll_lock (slab_lock, LLL_PRIVATE);
  info = alloc_slab_slot ();

  if (info == NULL)
    {
      __storage_release (mapping, len, __ipt_zos_tcb, noexec);
      lll_unlock (slab_lock, LLL_PRIVATE);
      return MAP_FAILED;
    }

  info->start = (void *) mapping;
  info->length = len;
  info->tcbaddr = __ipt_zos_tcb;
  info->noexec = noexec;

  lll_unlock (slab_lock, LLL_PRIVATE);
  return (void *) mapping;
}
libc_hidden_def (__create_anon_mmap)


/* Remove parts of mappings that intersect with the
   range [addr, addr+len).

   We don't check whether all pages in [addr, addr+len) refer
   to memory accessable by the process.  */
int
__unmap_anon_mmap (void *addr, size_t length)
{
  list_t *entry;
  uintptr_t start0 = (uintptr_t) addr, end0 = start0 + length;
  int retcode = 0;

  if (!length || !addr)
    return 0;

  /* We need to lock before touching the slabs.  */
  lll_lock (slab_lock, LLL_PRIVATE);

  /* Iterate over slab slots.  */
  list_for_each (entry, &anon_map_slabs)
    {
      struct map_slab *slab;

      slab = list_entry (entry, struct map_slab, slab_list);

      for (size_t pos = 0; pos < sizeof (slab->usage) * 8; ++pos)
	{
	  struct map_info *info;
	  uintptr_t start1, end1;
	  uintptr_t dealloc_start, dealloc_len;

	  if (!(slab->usage & (size_t) 1 << pos))
	    continue;

	  info = &slab->slots[pos].data;

	  start1 = (uintptr_t) info->start;
	  end1 = start1 + info->length;

	  /* If the deallocation range is outside this mapping entirely,
	     skip it.  */
	  if (end0 <= start1 || start0 >= end1)
	    continue;

	  /* Find start of part to unmap. Split map if necessary.  */
	  if (start0 > start1)
	    {
	      dealloc_start = start0;

	      /* We continue even if we run into errors.  */
	      retcode |= add_split_map (info, start0, start1 - start0);
	    }
	  else
	    dealloc_start = start1;

	  /* Find end of part to unmap. Split map if necessary.  */
	  if (end0 < end1)
	    {
	      dealloc_len = end0 - dealloc_start;

	      /* We continue even if we run into errors.  */
	      retcode |= add_split_map (info, end0, end1 - end0);
	    }
	  else
	    dealloc_len = end1 - dealloc_start;

	  /* Release the unmapped part of this mapping.  */
	  retcode |= __storage_release ((uint32_t) dealloc_start,
					(uint32_t) dealloc_len,
					info->tcbaddr, info->noexec);

	  /* Remove the accounting information from the list.  */
	  free_slab_slot (info);
	}
    }

  lll_unlock (slab_lock, LLL_PRIVATE);
  return retcode;
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
libc_hidden_def (__mlock_anon_mmap)
