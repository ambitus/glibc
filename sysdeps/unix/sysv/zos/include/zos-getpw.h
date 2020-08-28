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

#ifndef _ZOS_GETPW_H
#define _ZOS_GETPW_H 1

#include <stdint.h>
#include <stdbool.h>
#include <libc-lock.h>
#include <zos-utils.h>

/* We use the unix convention of returning "*" in pw_passwd when
   this service cannot return a password.  */
#define DUMMY_PASSWD "*"
/* z/OS TODO: Is there a way we can return useful info for pw_gecos?  */
#define DUMMY_GECOS ""

/* Lock to prevent usage of the getpw* assembler services while we
   are copying the static structure returned by one of them, which
   may be overwritten by subsequent calls to any related service.  */
__libc_lock_define (extern, __libc_zos_pw_lock attribute_hidden)

static inline uint32_t
read_word (char *buff)
{
  return (buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3] << 0);
}

/* Copy the structure returned by the pw syscalls into a struct passwd.
   Returns true if succesful and false if BUFLEN was not large
   enough.  */
static inline bool
try_copy_sysbuf (int32_t sysbuf_int, struct passwd *pwd, char *buffer,
		 size_t buflen)
{
  /* The structure we get back from the services is variable length, so
     it's a little annoying to parse.  */
  struct
  {
    char *ebcdic;
    char *ascii;
    uint32_t len;
  } strs[3];
  char *sysbuf = (char *) (uintptr_t) (uint32_t) sysbuf_int;
  char *cursor = buffer;

  /* Reserve space for dummy values for pw_passwd and pw_gecos.  */
  cursor += sizeof (DUMMY_PASSWD) + sizeof (DUMMY_GECOS);

  /* Handle pw_name.  */
  uint32_t namelen = read_word (sysbuf);
  sysbuf += 4;
  strs[0].ebcdic = sysbuf;
  if (namelen > 0)
    {
      /* Strip trailing EBCDIC spaces from the username.  */
      char *p;
      for (p = sysbuf + namelen - 1; p >= sysbuf && *p == 0x40; --p);
      strs[0].len = (uint32_t) (1 + p - sysbuf);
    }
  else
    strs[0].len = 0;
  sysbuf += namelen;

  strs[0].ascii = cursor;
  cursor += strs[0].len + 1;

  /* Parse pw_uid and pw_gid.  */
  assert (read_word (sysbuf) == 4);
  sysbuf += 4;
  pwd->pw_uid = read_word (sysbuf);
  sysbuf += 4;
  assert (read_word (sysbuf) == 4);
  sysbuf += 4;
  pwd->pw_gid = read_word (sysbuf);
  sysbuf += 4;

  /* Handle pw_dir and pw_shell.  */
  for (int n = 1; n < 3; ++n)
    {
      strs[n].len = read_word (sysbuf);
      sysbuf += 4;
      strs[n].ebcdic = sysbuf;
      sysbuf += strs[n].len;

      strs[n].ascii = cursor;
      cursor += strs[n].len + 1;
    }

  if (__glibc_unlikely (cursor >= buffer + buflen))
    return false;

  /* Translate strings from EBCDIC to ASCII.  */
  for (int i = 0; i < 3; ++i)
    {
      tr_a_until_len (strs[i].ebcdic, strs[i].ascii, strs[i].len);
      strs[i].ascii[strs[i].len] = '\0';
    }

  /* Copy dummy values.  */
  pwd->pw_passwd = memcpy (buffer, DUMMY_PASSWD, sizeof (DUMMY_PASSWD));
  pwd->pw_gecos = memcpy (buffer + sizeof (DUMMY_PASSWD),
			  DUMMY_GECOS, sizeof (DUMMY_GECOS));
  pwd->pw_name = strs[0].ascii;
  pwd->pw_dir = strs[1].ascii;
  pwd->pw_shell = strs[2].ascii;

  return true;
}

#endif
