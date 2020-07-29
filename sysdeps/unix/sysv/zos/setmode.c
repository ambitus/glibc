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
   <http://www.gnu.org/licenses/>.

   As a special exception, if you link the code in this file with
   files compiled with a GNU compiler to produce an executable,
   that does not cause the resulting executable to be covered by
   the GNU Lesser General Public License.  This exception does not
   however invalidate any other reasons why the executable file
   might be covered by the GNU Lesser General Public License.
   This exception applies to code released by its copyright holders
   in files containing the exception.  */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <not-cancel.h>
#include <io.h>

/* Indicate that the file corresponding to FD should be treated as
   binary if FLAGS is O_BINARY, otherwise indicate that it should be
   treated as text. Returns -1 if an error occured, O_TEXT if the stream
   was previously being treated as text, otherwise O_BINARY.  */
int
__setmode (int fd, int flags)
{
  struct stat64 st;
  struct zos_fconvert fcvt = { F_CVT_QUERY, 0, 0 };
  int ret;

  /* NOTE: There is similar code in check_fds.c, keep this in line
    with that.  */

  /* We don't have a good error value to return here.  */
  if (__fstat64 (fd, &st) != 0
      || __fcntl64_nocancel (fd, F_CONTROL_CVT, &fcvt) != 0)
    return -1;

  ret = fcvt.command == F_CVT_OFF ? O_BINARY : O_TEXT;

  /* If the file is an empty untagged file open for writing, try
     to tag the underlying file (in addition to enabling conversion on
     the file descriptor).
     z/OS TODO: This should obey the environment variable controlling
     new file tags.  */
  if ((S_ISREG (st.st_mode) || S_ISFIFO (st.st_mode))
      && st.st_size == 0)
    {
      int accmode = (O_ACCMODE & __fcntl64_nocancel (fd, F_GETFL));
      if (accmode == O_WRONLY || accmode == O_RDWR)
	{
	  struct zos_file_tag ft;
	  int save_errno = errno;
	  if (flags == O_BINARY)
	    {
	      ft.ft_ccsid = FT_BINARY;
	      ft.ft_flags = 0;
	    }
	  else
	    {
	      ft.ft_ccsid = 819;
	      ft.ft_flags = FT_PURETXT;
	    }
	  /* z/OS TODO: Race condition here.  */
	  __fcntl64_nocancel (fd, F_SETTAG, &ft);
	  __set_errno (save_errno);
	}
    }

  /* Enable/disable conversion.  */
  fcvt.prog_ccsid = 0;
  fcvt.file_ccsid = 0;
  fcvt.command = flags == O_BINARY ? F_CVT_OFF : F_CVT_ON;

  if (__fcntl64_nocancel (fd, F_CONTROL_CVT, &fcvt) < 0)
    return -1;

  return ret;
}
libc_hidden_def (__setmode)
weak_alias (__setmode, setmode)
libc_hidden_weak (setmode)
weak_alias (__setmode, _setmode)
libc_hidden_weak (_setmode)
