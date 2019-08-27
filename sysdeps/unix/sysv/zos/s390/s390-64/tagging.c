/* Copyright (C) 2019 Rocket Software, Inc.
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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <fcntl.h>
#include <sys/stat.h>
#include <sysdep.h>
#include <not-cancel.h>
#include <zos-core.h>

struct zos_file_attrs
  {
    char		eyecatcher[4];
    unsigned short	version;
#define CHATTR_CURR_VER 3
    unsigned short	__res;
    unsigned int	set_flags;	/* Which fields to set.  */
#define CHATTR_SETTAG 0x00004000
    unsigned int	mode;
    unsigned int	uid;
    unsigned int	gid;
    unsigned char	__res2[3];
    unsigned char	flags2;
    unsigned char	__res3[3];
    unsigned char	flags3;
    unsigned long int	size;
    unsigned int	_bpx_atime32;
    unsigned int	_bpx_mtime32;
    unsigned int	auditoraudit;
    unsigned int	useraudit;
    unsigned int	_bpx_ctime32;
    unsigned int	_bpx_reftime32;
    unsigned char	format;
    unsigned char	__res4[3];
    struct zos_file_tag tag;
    unsigned char	__res5[8];
    __time_t		_bpx_atime64;
    __time_t		_bpx_mtime64;
    __time_t		_bpx_ctime64;
    __time_t		reftime;
    unsigned char	seclabel[8];
    unsigned char	__res6[8];
  };

typedef void (*__bpx4fcr_t) (const int32_t *fd,
			     const int32_t *attrs_len,
			     const struct zos_file_attrs *attrs,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

static int
fchattr (int fd, const struct zos_file_attrs *attrs)
{
  int32_t retval, retcode, reason_code;
  int size = sizeof (*attrs);

  BPX_CALL (fchattr, __bpx4fcr_t, &fd, &size, attrs, &retval, &retcode,
	    &reason_code);

  return retval;
}


int
__set_file_tag_if_empty_unsafe (int fd, const struct zos_file_tag *tag)
{
  /* For some reason, we need to initialize the eyecatcher and
     version fields.  */
  struct stat st;
  struct zos_file_attrs attrs = {
      .eyecatcher = { 0xC1, 0xE3, 0xE3, 0x40 },
      .version = CHATTR_CURR_VER,
      .set_flags = CHATTR_SETTAG,
      .tag = *tag
    };

  /* z/OS TODO: IMPORTANT: We can't atomically change a file's tag
     if and only if the file is empty when the tag is changed, if that
     file is already tagged.  */
  if (fstat (fd, &st) < 0)
    return -1;

  return st.st_size == 0 ? fchattr (fd, &attrs) : 0;
}
libc_hidden_def (__set_file_tag_if_empty_unsafe)
