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

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sysdep.h>

/* Create a one-way communication channel (__pipe).  If successful,
   two file descriptors are stored in PIPEDES; bytes written on
   PIPEDES[1] can be read from PIPEDES[0].  Apply FLAGS to the new
   file descriptors.  Returns 0 if successful, -1 if not.

   NOTE: z/OS doesn't have pipe2, so this is just a racy, AS-unsafe
   emulation.  */
int
__pipe2 (int pipedes[2], int flags)
{
  int ret;

  if (flags & O_DIRECT)
    {
      /* We don't support and can't emulate Linux's peculiar packet
	 mode pipes.  */
      __set_errno (EINVAL);
      return -1;
    }

  ret = __pipe (pipedes);

  if (ret < 0)
    return ret;

  /* z/OS TODO: There's a race condition here, but we could mitigate
     it by incrementing a global hazard variable that all threads
     check before forking. If it's nonzero the forking thread waits
     for a while then checks it again, if it has been incremented
     then wait again, but if that happens too many times just go
     ahead and fork anyway. Decrement it after we've done the cloexec
     call. For total safety, we would need some way to decrement the
     value if this thread dies (maybe a secondary thread-local
     sentinel and a resource manager?).  */
  if (flags & O_CLOEXEC
      && (__fcntl (pipedes[0], F_SETFD, FD_CLOEXEC) < 0
	  || __fcntl (pipedes[1], F_SETFD, FD_CLOEXEC) < 0))
    {
      /* z/OS TODO: should we report this error to the user, or
	 silently ignore it? It might be confusing, and a failure
	 here would almost always be nonfatal, it would just be a
	 slow leak of file descriptors. For now we report it.  */
      return -1;
    }

  /* z/OS TODO: This needs to be guarded in the same manner as above.  */
  if (flags & O_NONBLOCK
      && (__fcntl (pipedes[0], F_SETFL, O_NONBLOCK) < 0
	  || __fcntl (pipedes[1], F_SETFL, O_NONBLOCK) < 0))
    {
      /* z/OS TODO: Same error considerations as above.  */
      return -1;
    }

  return 0;
}
weak_alias (__pipe2, pipe2)
