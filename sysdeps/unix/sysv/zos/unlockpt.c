/* Copyright (C) 2020 Free Software Foundation, Inc.
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
#include <sysdep.h>


typedef void (*__bpx4upt_t) (const int32_t *fd,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Change the ownership and access permission of the slave pseudo
   terminal associated with the master pseudo terminal specified
   by FD.  */
int
unlockpt (int fd)
{
  int32_t retval, rsncode;
  INTERNAL_SYSCALL_DECL (retcode);
  int32_t file_desc = fd;

  BPX_CALL (unlockpt, __bpx4upt_t, &file_desc,
	    &retval, &retcode, &rsncode);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));

  return retval;
}
