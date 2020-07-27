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
   binary if FLAGS has O_BINARY set, and nothing under any other
   circumstance. Returns 0.  */
int
__setmode (int fd, int flags)
{
  struct stat64 st;
  struct zos_fconvert fcvt;

  if ((flags & O_BINARY) != O_BINARY)
    return 0;

  /* NOTE: There is similar code in check_fds.c, keep this in line
    with that.  */

  if (__fstat64 (fd, &st) != 0)
    return 0;

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
	  ft.ft_ccsid = FT_BINARY;
	  ft.ft_flags = 0;
	  /* z/OS TODO: Race condition here.  */
	  __fcntl64_nocancel (fd, F_SETTAG, &ft);
	}
    }

  /* Disable conversion.  */
  fcvt.prog_ccsid = 0;
  fcvt.file_ccsid = 0;
  fcvt.command = F_CVT_OFF;

  __fcntl64_nocancel (fd, F_CONTROL_CVT, &fcvt);

  return 0;
}
libc_hidden_def (__setmode)
weak_alias (__setmode, setmode)
libc_hidden_weak (setmode)
weak_alias (__setmode, _setmode)
libc_hidden_weak (_setmode)
