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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>
#include <zos-chattr.h>
#include <sysdep.h>

typedef void (*__bpx4fcr_t) (const int32_t *fd,
			     const int32_t *attrs_len,
			     const struct zos_file_attrs *attrs,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

int
__zos_fchattr (int fd, struct zos_file_attrs *attrs,
	       unsigned long attrs_size)
{
  int32_t retval, retcode, reason_code;
  int32_t size = (attrs_size > INT32_MAX ? INT32_MAX :
		  attrs_size == 0 ? (int32_t) sizeof (*attrs) :
		  (int32_t) attrs_size);

  if (attrs == NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  if (size >= 6)
    {
      /* Fill in required fields if not present.  */
      attrs->eyecatcher[0] = 0xc1;
      attrs->eyecatcher[1] = 0xe3;
      attrs->eyecatcher[2] = 0xe3;
      attrs->eyecatcher[3] = 0x40;
      if (attrs->version == 0)
	attrs->version = _CHATTR_CURR_VER;
    }

  BPX_CALL (fchattr, __bpx4fcr_t, &fd, &size, attrs, &retval, &retcode,
	    &reason_code);

  if (retval < 0)
    __set_errno (retcode);

  return retval;
}
weak_alias (__zos_fchattr, zos_fchattr)
