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
#include <mode_t-translation.h>

typedef void (*bpx4cha_t) (int32_t *pathname_length,
			   const char *pathname,
			   int32_t *attrs_length,
			   struct zos_file_attrs *attrs,
			   int32_t *retval, int32_t *retcode,
			   int32_t *reason_code);

int
__zos_chattr (const char *pathname, struct zos_file_attrs *attrs,
	      unsigned long attrs_size)
{
  int32_t retval, retcode, reason_code;
  char translated_path[__BPXK_PATH_MAX];
  int32_t size = (attrs_size > INT32_MAX ? INT32_MAX :
		  attrs_size == 0 ? (int32_t) sizeof (*attrs) :
		  (int32_t) attrs_size);
  uint32_t pathname_len = translate_and_check_size (pathname,
						    translated_path);

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

  uint32_t kern_mode = user_to_kern_mode_t (attrs->mode);
  attrs->mode = kern_mode;

  BPX_CALL (chattr, bpx4cha_t, &pathname_len, translated_path,
	    &size, attrs, &retval, &retcode, &reason_code);

  uint32_t user_mode = kern_to_user_mode_t (attrs->mode);
  attrs->mode = user_mode;

  if (retval < 0)
    __set_errno (retcode);

  return retval;
}
weak_alias (__zos_chattr, zos_chattr)
