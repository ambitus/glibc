/* System-specific malloc support functions.  z/OS version.
   Copyright (C) 2018 Rocket Software.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>

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

#include <unistd.h>

/* TODO: Is there anything else we need to do here?  */

static inline bool
check_may_shrink_heap (void)
{
  static int may_shrink_heap = -1;

  if (__builtin_expect (may_shrink_heap >= 0, 1))
    return may_shrink_heap;

  may_shrink_heap = __libc_enable_secure;

  return may_shrink_heap;
}

#define HAVE_MREMAP 0
