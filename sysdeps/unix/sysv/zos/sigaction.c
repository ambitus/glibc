/* Copyright (C) 2019 Free Software Foundation, Inc.
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

/* z/OS TODO: go back to leaning on the generic nptl version like the
   generic linux version does, after we override internal-signals.h and
   remove any uses of SIGCANCEL and SIGSETXID.  */

#include <signal.h>
#include <sysdep.h>
#include <internal-signals.h>

/* If ACT is not NULL, change the action for SIG to *ACT.
   If OACT is not NULL, put the old action for SIG in *OACT.  */
int
__libc_sigaction (int sig, const struct sigaction *act,
		  struct sigaction *oact)
{
  return INLINE_SYSCALL_CALL (sigaction, sig, act, oact);
}

int
__sigaction (int sig, const struct sigaction *act,
	     struct sigaction *oact)
{
  if (sig <= 0 || sig >= 64)
    {
      __set_errno (EINVAL);
      return -1;
    }

  return __libc_sigaction (sig, act, oact);
}
libc_hidden_weak (__sigaction)
weak_alias (__sigaction, sigaction)
