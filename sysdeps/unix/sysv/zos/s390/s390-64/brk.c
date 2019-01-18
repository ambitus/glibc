/* Copyright (C) 2018 Rocket Software
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
#include <errno.h>
#include <unistd.h>
#include <sys/cdefs.h>
#include <lowlevellock.h>
#include <sysdep.h>
#include <zos-core.h>

/* This must be initialized data because commons can't have aliases.  */
void *__curbrk = 0;

/* Old braindamage in GCC's crtstuff.c requires this symbol in an attempt
   to work around different old braindamage in the old Linux ELF dynamic
   linker.  */
weak_alias (__curbrk, ___brk_addr)


#define BRK_AREA_SIZE 30 * (1024 * 1024)

extern unsigned int __ipt_zos_tcb attribute_hidden;
static void *brk_min;
static int brk_lock = LLL_LOCK_INITIALIZER;


/* We don't actually have a brk syscall, however we can fake one.
   TODO: use IARV64 with guard areas as a backend for brk instead of
   STORAGE OBTAIN, which can't overcommit memory.  */
int
__brk (void *addr)
{
  /* Get a lock.  */
  lll_lock (brk_lock, LLL_PRIVATE);

  /* A one-time large allocation associated with the IPT.  */
  if (__glibc_unlikely (!__curbrk))
    {
      /* void *brk_start = __storage_obtain (BRK_AREA_SIZE,
	 __ipt_zos_tcb, true, true); */
      /* z/OS TODO: revert this when regular storage obtain works.  */
      void *brk_start = __storage_obtain_simple (BRK_AREA_SIZE);
      /* Don't set errno here, we're early in libc startup.
         Errno might not exist yet.  */
      if (!brk_start)
	goto lose;
      __curbrk = brk_min = brk_start;
    }

  if (__glibc_unlikely (!addr))
    goto win;

  if ((uintptr_t) addr > (uintptr_t) brk_min + BRK_AREA_SIZE)
    {
      __curbrk = brk_min + BRK_AREA_SIZE;
      __set_errno (ENOMEM);
      goto lose;
    }

  if ((uintptr_t) addr < (uintptr_t) brk_min)
    {
      /* This is actually more correct than all other brk glibc
	 implementations.  */
      __set_errno (EINVAL);
      goto lose;
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
