/* Copyright (C) 2019-2020 Free Software Foundation, Inc.
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

#ifndef _MAP_INFO_H
#define _MAP_INFO_H 1

#include <stdint.h>
#include <stdbool.h>

/* An anonymous mapping.  */
struct map_info
{
  /* The start of the mapping is also the key of the corresponding list
     node. The only reason this is also stored in the structure is
     because the list node is not available when undoing an incref
     on a node that was clobbered immediately before the incref.  */
  void *addr;

  /* If this is map_info is an allocation owner, this is how the original
     allocation actually was (i.e. how large the corresponding memory
     object actually is). Otherwise, this is how long the user thinks this
     area is. This field should not be modified after it is initialized.  */
  uint64_t length;

  /* If this map_info is the 'owner' of a full allocation, this is the
     address of the start of of a memory object, or if it is a STORAGE
     allocation, then just the start of the original allocation. If it
     is not an allocation owner, then this is zero. This field should
     not be modified after it is initialized.
     If memobj_start is valid and zero, then obj_owner is valid, and its
     low bit is 0.
     If memobj_start is valid and nonzero, then refcount is valid, and
     its low bit is 1.  */
  void *memobj_start;

  volatile union
  {
    /* Pointer to the object owner, only valid if this map_info
       represents a strict subset of a larger memory object. This field
       should not be modified after it is initialized. This field is also
       only valid when its low bit is 0.  */
    struct map_info *obj_owner;

    /* Reference count, only valid if this map_info is the designated
       'owner' node of a memory object. The refcount is equal to the
       number of mappings that the memory object has been split into,
       and is initialized to 3. Each time a constituent map of the
       memory object is split into multiple constituent maps, the
       refcount of the map_info pointed to by the constituent mapping's
       obj_owner field is incremented by 2. Each time a constituent map
       is logically deleted, the obj_owner's refcount should be
       atomically decremented by 2. It is also decremented when the
       object owner map_info is itself logically deleted, but if the
       refcount is not 1 then the map_info may not be freed (reused).
       This field is only valid if its low bit is 1.  */
    volatile uint64_t refcount;
  } u;
};

#endif /* !_MAP_INFO_H  */
