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

#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <sysdep.h>
#include <zos-mmap.h>
#include <zos-core.h>

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
     that fall in the range then calling the system mmap.  */
#if IS_IN (libc)
  anon_ret = __unmap_anon_mmap (addr, len);
#else
  /* z/OS TODO: We can't do anything about anon mappings in rtld.  */
  anon_ret = 0;
#endif

  /* Don't run the actual munmap if the address is above the bar, because
     the real munmap doesn't use those addresses.  */
  if ((uintptr_t) addr <= PTR31_BAR)
    {
      /* If the unmap range would go over the bar, don't run the real
	 munmap on that part.  */
      if ((uintptr_t) addr + len > PTR31_BAR)
	len = PTR31_BAR - (uintptr_t) addr;
      sc_ret = INLINE_SYSCALL_CALL (munmap, addr, len);
    }
  else
    sc_ret = 0;

  if (anon_ret && !sc_ret)
    {
      __set_errno (ENOMEM);
      return -1;
    }
  else
    return anon_ret || sc_ret ? -1 : 0;
}
libc_hidden_def (__munmap)
weak_alias (__munmap, munmap)
