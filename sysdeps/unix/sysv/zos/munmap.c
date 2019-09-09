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

#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <sysdep.h>
#include <zos-mmap.h>

/* Deallocate any mapping for the region starting at ADDR and extending LEN
   bytes.  Returns 0 if successful, -1 for errors (and sets errno).  */
int
__munmap (void *addr, size_t len)
{
  int anon_ret, sc_ret;

  if (len == 0 || (uintptr_t) addr % 4096 != 0)
    {
      __set_errno (EINVAL);
      return -1;
    }

  /* This is a bit strange, since we are responsible for managing anon
     mappings, while the system is responsible for managing regular
     ones. We try to balance this by unmapping any parts of anon maps
     that fall in the range, calling the system mmap, then returning
     -1 if either call failed.  */

  anon_ret = __unmap_anon_mmap (addr, len);
  sc_ret = INLINE_SYSCALL_CALL (munmap, addr, len);

  if (anon_ret && !sc_ret)
    {
      __set_errno (ENOMEM);
      return -1;
    }
  else
    return anon_ret || sc_ret ? -1 : 0;
}
weak_alias (__munmap, munmap)
