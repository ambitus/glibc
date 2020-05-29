/* Stack executability handling for GNU dynamic linker.  z/OS version.
   Copyright (C) 2020 Free Software Foundation, Inc.
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

#include <ldsodefs.h>
#include <errno.h>
#include <stdbool.h>
#include <stackinfo.h>
#include <sysdep.h>

extern int __stack_prot attribute_relro attribute_hidden;

/* z/OS TODO: Make our stack non-executable by default. Also figure out
   how to make the stack executable later on if we need it.  */
int
_dl_make_stack_executable (void **stack_endp)
{
  /* z/OS TODO: This is a no-op right now.  */
  int result = 0;

  /* Clear the address.  */
  *stack_endp = NULL;

#ifdef check_consistency
  check_consistency ();
#endif

  return result;
}
rtld_hidden_def (_dl_make_stack_executable)
