/* mmap - map files or devices into memory.  z/OS version.
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

#include <errno.h>
#include <sys/mman.h>
#include <lowlevellock.h>
#include <zos-core.h>
#include <zos-mmap.h>
#include <sysdep.h>

void *
__mmap64 (void *addr, size_t len, int prot, int flags, int fd,
	  off64_t offset)
{
  if (offset % 4096 != 0)
    {
      __set_errno (EINVAL);
      return MAP_FAILED;
    }

  if (flags & MAP_ANONYMOUS)
    {
      void *ret;

      if ((flags & MAP_SHARED) == MAP_SHARED
	  || (flags & MAP_SHARED_VALIDATE) == MAP_SHARED_VALIDATE
	  || (~prot & (PROT_READ | PROT_WRITE)))
	{
	  /* We don't support MAP_SHARED with MAP_ANONYMOUS yet.
	     z/OS TODO: implemented shared anonymous mappings.
	     We could do it by creating a second list for shared anon
	     mappings and using IARVSERV with them.
	     z/OS TODO: We haven't implemented read-only anonymous
	     mappings yet.  */
	  __set_errno (ENOSYS);
	  return MAP_FAILED;
	}

#if IS_IN (libc)
      ret = __create_anon_mmap (addr, len, prot, flags);
#else
      /* z/OS TODO: In rltd, we don't keep track of anonymous mappings, and
         can't free them or mprotect them.
         z/OS TODO: allow prot to apply.  */
      ret = __storage_obtain_simple (len);
      if (ret == NULL)
	ret = MAP_FAILED;
#endif

      if (ret == MAP_FAILED)
	{
	  __set_errno (ENOMEM);
	  return MAP_FAILED;
	}

      return ret;
    }

  /* For MAP_FIXED requests, we need to be able to replace existing
     mappings. Our mmap implementation doesn't allow that, so we need
     to unmap first, just in case.
     z/OS TODO: Any way to do this atomically? There's a race condition
     between unmapping and mapping that could break any application that
     tries to be fancy with mappings (eg. the dynamic linker). Also if
     the request ends up failing anyway, we've still may have just
     unmapped a region, which leaves us in a perilous situation.  */
  if (flags & MAP_FIXED)
    __munmap (addr, len);

  return (void *) INLINE_SYSCALL_CALL (mmap, addr, len, prot, flags, fd, offset);
}
weak_alias (__mmap64, mmap64)
libc_hidden_def (__mmap64)

/* mmap64 is just the same as mmap for us.  */
weak_alias (__mmap64, mmap)
weak_alias (__mmap64, __mmap)
libc_hidden_def (__mmap)
