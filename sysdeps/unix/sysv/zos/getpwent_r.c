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
#include <libc-lock.h>
#include <zos-getpw.h>
#include <sysdep.h>


#define NEW(name) NEW1 (name)
#define NEW1(name) __new_##name

typedef void (*__bpx4spe_t) (int32_t *retval);

void
setpwent (void)
{
  int save;
  int32_t retval;

  __libc_lock_lock (__libc_zos_pw_lock);

  BPX_CALL (setpwent, __bpx4spe_t, &retval);

  save = errno;
  __libc_lock_unlock (__libc_zos_pw_lock);
  __set_errno (save);
}


void
endpwent (void)
{
  /* We have no equivalent syscall. The best we can do
     is a setpwent.  */
  setpwent ();
}

typedef void (*__bpx4gpe_t) (int32_t *retval, int32_t *retcode,
                             int32_t *reason_code);

int
__getpwent_r (struct passwd *resbuf, char *buffer, size_t buflen,
	      struct passwd **result)
{
  int32_t retval, retcode, reason_code;
  int save, status;

  __libc_lock_lock (__libc_zos_pw_lock);

  BPX_CALL (getpwent, __bpx4gpe_t, &retval, &retcode, &reason_code);

  if (retval != 0)
    {
      if (!try_copy_sysbuf (retval, resbuf, buffer, buflen))
	status = ERANGE;  /* The provided buffer is too small.  */
      else
	status = 0;
    }
  else
    status = retcode;

  if (status != 0)
    {
      *result = NULL;
      save = status;
    }
  else
    {
      *result = resbuf;
      save = errno;
    }

  __libc_lock_unlock (__libc_zos_pw_lock);
  __set_errno (save);
  return status;
}


#include <shlib-compat.h>
#if SHLIB_COMPAT (libc, GLIBC_2_0, GLIBC_2_1_2)
# define OLD(name) OLD1 (name)
# define OLD1(name) __old_##name

int
attribute_compat_text_section
OLD (getpwent_r) (struct passwd *resbuf, char *buffer, size_t buflen,
			 struct passwd **result)
{
  int ret = __getpwent_r (resbuf, buffer, buflen,
			  result);

  if (ret != 0)
    ret = -1;

  return ret;
}

# define do_symbol_version(real, name, version) \
  compat_symbol (libc, real, name, version)
do_symbol_version (OLD (getpwent_r), getpwent_r, GLIBC_2_0);
#endif

/* As INTERNAL (REENTRANT_GETNAME) may be hidden, we need an alias
   in between so that the REENTRANT_GETNAME@@GLIBC_2.1.2 is not
   hidden too.  */
strong_alias (__getpwent_r, NEW (getpwent_r));

#define do_default_symbol_version(real, name, version) \
  versioned_symbol (libc, real, name, version)
do_default_symbol_version (NEW (getpwent_r),
			   getpwent_r, GLIBC_2_1_2);
