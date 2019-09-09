/* arch_fork definition for z/OS fork implementation.
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
   <http://www.gnu.org/licenses/>.  */

#ifndef __ARCH_FORK_H
#define __ARCH_FORK_H

#include <unistd.h>
#include <sysdep.h>

/* Call something that does a fork. The CTID address [SHOULD BE] used
   to store the child thread ID at its locationm, to erase it in child
   memory when the child exits, and do a wakeup on the futex at that
   address, but at the moment we don't use it properly.  */
static inline pid_t
arch_fork (void *ctid __attribute__ ((__unused__)))
{
  /* z/OS TODO: IMPORTANT: Do what the linux port expects should be
     done with ctid.  */
  return INLINE_SYSCALL_CALL (fork);
}

#endif /* !__ARCH_FORK_H  */
