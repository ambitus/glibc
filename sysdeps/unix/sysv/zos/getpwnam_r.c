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

#include <stdint.h>
#include <errno.h>
#include <pwd.h>
#include <libc-lock.h>
#include <zos-getpw.h>
#include <sysdep.h>
#include <string.h>

typedef void (*__bpx4gpn_t) (const char *name,
			     const uint32_t *namelen,
                             int32_t *retval, int32_t *retcode,
                             int32_t *reason_code);


int
__getpwnam_r (const char *name, struct passwd *pwd, char *buffer,
	      size_t buflen, struct passwd **result)
{
  int32_t retval, retcode, reason_code;
  int ret;
  char translated_name[256];  /* z/OS TODO: What's a good size here?  */
  uint32_t namelen
    = (uint32_t) tr_e_until_chr_or_len (name, translated_name, '\0',
					sizeof (translated_name));

  /* Lock to avoid overwriting the buffer before we can copy its
     contents out.  */
  __libc_lock_lock (__libc_zos_pw_lock);

  BPX_CALL (getpwnam, __bpx4gpn_t, &namelen, translated_name, &retval,
	    &retcode, &reason_code);

  if (retval != 0)
    {
      if (!try_copy_sysbuf (retval, pwd, buffer, buflen))
	ret = ERANGE;  /* The provided buffer is too small.  */
      else
	ret = 0;
    }
  else
    ret = retcode;

  __libc_lock_unlock (__libc_zos_pw_lock);

  if (ret != 0)
    {
      __set_errno (ret);
      *result = NULL;
    }
  else
    {
      *result = pwd;
    }

  return ret;
}

strong_alias (__getpwnam_r, getpwnam_r)
