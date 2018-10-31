/* Common z/OS utilities.
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

#ifndef _ZOS_UTILS_H
#define _ZOS_UTILS_H 1
#ifndef __ASSEMBLER__

#include <stdint.h>

#define GET_PTR31_UNSAFE(x) ((uintptr_t)(*(uint32_t *)(x)))
#define GET_PTR31_SAFE(x) ((uintptr_t)(~(1UL << 31) & *(uint32_t *)(x)))

#endif /* __ASSEMBLER__  */
#endif /* _ZOS_UTILS_H  */
