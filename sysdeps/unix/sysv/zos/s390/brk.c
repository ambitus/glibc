/* brk implementation, z/OS flavor.
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
   <http://www.gnu.org/licenses/>.

   TODO: This is a dummy at the moment. We could implement brk-like
   functionality if there is some way to make an allocation on z/OS and
   then expand that allocation toward higher memory addresses. However,
   I'm not aware of any such functionality at the moment.  */

#include <errno.h>

void *__curbrk = 0;

/* Old braindamage in GCC's crtstuff.c requires this symbol in an attempt
   to work around different old braindamage in the old Linux/x86 ELF
   dynamic linker.  Sigh.  */
weak_alias (__curbrk, ___brk_addr)

int
__brk (void *addr)
{
  __set_errno (ENOSYS);
  return -1;
}
weak_alias (__brk, brk)

stub_warning (__brk)
stub_warning (brk)
