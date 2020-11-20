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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/cdefs.h>
#include <lowlevellock.h>
#include <sysdep.h>
#include <zos-core.h>

/* The size of the fixed area we allocate, in megabytes.  */
#define BRK_AREA_SIZE  (4 * 1024)
#define ONE_MEG  (1024UL * 1024UL)
#define MEG_FLOOR(addr)  ((uintptr_t) (addr) & ~(ONE_MEG - 1))

/* This must be initialized data because commons can't have aliases.  */
void *__curbrk = 0;

/* Old braindamage in GCC's crtstuff.c requires this symbol in an attempt
   to work around different old braindamage in the old Linux ELF dynamic
   linker.  */
weak_alias (__curbrk, ___brk_addr)


static void *brk_min, *brk_max;
static bool using_iarv64 = true;
static int brk_lock = LLL_LOCK_INITIALIZER;

static inline void *
try_iarv64 (uint64_t megabytes)
{
  /* Start with one non-guard segment, as an optimization.  */
  void *addr = __iarv64_getstorage (megabytes,
				    megabytes - 1,
				    GUARDLOC_HIGH, NULL, NULL);
  if (addr != NULL)
    brk_max = (char *) addr + megabytes * ONE_MEG;

  return addr;
}

static inline void *
try_storage (unsigned int bytes)
{
  void *addr = __storage_obtain (1 * 1024 * 1024, false, true);

  if (addr != NULL)
    brk_max = (char *) addr + bytes;

  return addr;
}


/* We don't actually have a brk syscall, however we can fake one.  */
int
__brk (void *addr)
{
  /* Get a lock.  */
  lll_lock (brk_lock, LLL_PRIVATE);

  /* A one-time large allocation associated with the IPT.
     z/OS TODO: THREADING: Associate this storage with either the
     initial task or the jobstep task.  */
  if (__glibc_unlikely (!__curbrk))
    {
      void *brk_start;

      /* Try the default value for the total size of the brk area. If
	 that fails, here might be a strict limit on the number of pages
	 we can use, so try a much smaller area.  */
      if (!(brk_start = try_iarv64 (BRK_AREA_SIZE))
	  && !(brk_start = try_iarv64 (32)))
	{
	  /* This process may not be allowed to use 64-bit storage,
	     fall back to using a (smaller amount of) 32-bit storage.
	     NOTE: We can't deallocate when using STORAGE OBTAIN.  */
	  using_iarv64 = false;
	  if (!(brk_start = try_storage (1 * ONE_MEG)))
	    {
	      /* Try (roughly) the minimum size that is likely to permit
		 startup to succeed (permitting up to about 10KB of TLS
		 data).
	         z/OS TODO: Permit arbitrary sizes of TLS data here.  */
	      brk_start = try_storage (16384);
	    }
	}

      /* Don't set errno here, we're early in libc startup.
         Errno might not exist yet.  */
      if (!brk_start)
	goto lose;
      __curbrk = brk_min = brk_start;
    }

  if (__glibc_unlikely (!addr))
    goto win;

  if ((uintptr_t) addr < (uintptr_t) brk_min)
    {
      /* This is actually more correct than all other brk glibc
	 implementations.  */
      __set_errno (EINVAL);
      goto lose;
    }

  if ((uintptr_t) addr > (uintptr_t) brk_max)
    {
      __set_errno (ENOMEM);
      goto lose;
    }

  if (using_iarv64)
    {
      /* Check if we need to grow or shrink the guard area if we are
	 using IARV64-allocated memory.

	 We grow/shrink the guard area to the nearest megabyte
	 boundary at or above the requested limit. Note that zero is not
	 a valid brk value.
	 z/OS TODO: This allocation approach may not be ideal.
	 It may be best to ask the OS for more storage than we
	 immediately need under certain circumstances.  */
      uintptr_t new_floor = MEG_FLOOR (addr - 1);
      uintptr_t old_floor = MEG_FLOOR (__curbrk - 1);
      if (new_floor > old_floor)
	{
	  /* We need to grow the resident memory area.  */
	  uint64_t delta = (new_floor - old_floor) / ONE_MEG;

	  /* We pre-allocate one megabyte as an optimization, so we we
	     only actually need to allocate once that runs out. Glibc
	     uses that space internally so it will never be deallocated,
	     so we don't need to handle that situation.  */
	  if (__glibc_likely (old_floor != (uintptr_t) brk_min - ONE_MEG
			      || --delta > 0))
	    {
	      uint32_t ret;
	      __iarv64_shrink_guard (brk_min, delta, &ret, NULL);
	      if (__glibc_unlikely (ret > 4))
		{
		  __set_errno (ENOMEM);
		  goto lose;
		}
	    }
	}
      else if (new_floor < old_floor)
	{
	  /* We should shrink the resident memory area.  */
	  uint64_t delta = (old_floor - new_floor) / ONE_MEG;

	  uint32_t ret;
	  __iarv64_grow_guard (brk_min, delta, &ret, NULL);
	  if (__glibc_unlikely (ret > 4))
	    {
	      /* We're not out of memory, but something has gone
		 wrong.  */
	      __set_errno (EINVAL);
	      goto lose;
	    }
	}
    }

  __curbrk = addr;

win:
  lll_unlock (brk_lock, LLL_PRIVATE);
  return 0;

lose:
  lll_unlock (brk_lock, LLL_PRIVATE);
  return -1;
}
weak_alias (__brk, brk)
