/* Copyright (C) 2019-2020 Free Software Foundation, Inc.
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

#include <sys/types.h>
#include <sys/wait.h>
#include <sysdep.h>
#include <sysdep-cancel.h>

pid_t
__wait4 (__pid_t pid, int *stat_loc, int options, struct rusage *usage)
{
  return SYSCALL_CANCEL (wait4, pid, stat_loc, options, usage);
}

link_warning (wait4, "z/OS wait4 fails when rusage is nonnull")
weak_alias (__wait4, wait4)
