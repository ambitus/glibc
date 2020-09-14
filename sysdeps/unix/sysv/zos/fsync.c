/* Synchronize a file's in-core state with storage device. z/OS
   implementation.
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

#include <unistd.h>
#include <sysdep-cancel.h>

typedef void (*__bpx4fsy_t) (const int32_t *fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Make all changes done to FD actually appear on disk.  */

int
fsync (int fd)
{
  int32_t retval, retcode, reason_code;

  BPX_CALL_CANCEL (fsync, __bpx4fsy_t, &fd, &retval, &retcode,
		   &reason_code);

  if (retval != 0)
    __set_errno (retcode);

  return retval;
}
