/* mmap - map files or devices into memory.  z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

   This file is necessary to override linux/s390/mmap_internal.h.  */

#ifndef MMAP_ZOS_INTERNAL_H
#define MMAP_ZOS_INTERNAL_H 1

#include <sysdeps/unix/sysv/linux/mmap_internal.h>

#endif /* !MMAP_ZOS_INTERNAL_H */
