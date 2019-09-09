/* Anonymous mapping implementation header.
   Copyright (C) 2019 Free Software Foundation, Inc.
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

/* The z/OS mmap syscall doesn't support MAP_ANONYMOUS, however there's
   no good reason for that. Using the OS facilities, we can implement
   a userspace version of anonymous mappings that works well.

   TODO: implement MAP_SHARED anonymous mappings.  */

#ifndef _ZOS_MMAP_H
#define _ZOS_MMAP_H 1

#include <stddef.h>

/* TODO: implement correct msync, mprotect, madvise, etc for
   anon maps.  */

extern void *__create_anon_mmap (void *addr, size_t len, int prot,
				 int flags);
extern int __unmap_anon_mmap (void *addr, size_t length);
extern int __set_protections_anon_mmap (void *addr, size_t length,
					int prot);
extern int __mlock_anon_mmap (void *addr, size_t length);
extern int __munlock_anon_mmap (void *addr, size_t length);

libc_hidden_proto (__create_anon_mmap)
libc_hidden_proto (__unmap_anon_mmap)
libc_hidden_proto (__set_protections_anon_mmap)
libc_hidden_proto (__mlock_anon_mmap)
libc_hidden_proto (__munlock_anon_mmap)


#endif /* !_ZOS_MMAP_H  */
