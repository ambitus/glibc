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

#include <fcntl.h>
#include <sys/stat.h>
#include <not-cancel.h>

static void
tag_one_fd (int fd)
{
  struct stat64 st;
  struct zos_fconvert fcvt;

  if (__fstat64 (fd, &st) != 0)
    return;

  /* If the file is an empty untagged file open for writing, try
     to tag the underlying file (in addition to enabling conversion on
     the file descriptor). Never modify stdin, even if it is writeable.
     z/OS TODO: This should obey the environment variable controlling
     new file tags.  */
  if ((S_ISREG (st.st_mode) || S_ISFIFO (st.st_mode))
      && st.st_size == 0
      && ((st.st_ccsid == 0)
	  || (st.st_ftflags & FT_PURETXT
	      && st.st_ftflags & FT_DEFER))
      && fd != STDIN_FILENO)
    {
      int accmode = (O_ACCMODE & __fcntl64_nocancel (fd, F_GETFL));
      if (accmode == O_WRONLY || accmode == O_RDWR)
	{
	  struct zos_file_tag ft;
	  ft.ft_ccsid = 819;
	  ft.ft_flags = FT_PURETXT;
	  /* z/OS TODO: Race condition here.  */
	  __fcntl64_nocancel (fd, F_SETTAG, &ft);
	  st.st_ccsid = ft.ft_ccsid;
	  st.st_ftflags = ft.ft_flags;
	}
    }

  /* z/OS TODO: This needs to obey first the override env vars, then
     the env var controlling whether to treat untagged files as EBCDIC
     or binary.  */

  /* Enable conversion, and treat untagged files as EBCDIC text.
     NOTE: It is important that we never disable conversion, because
     other processes may rely on the conversion state of the standard
     streams being enabled.  */
  fcvt.prog_ccsid = 0;
  fcvt.command = F_CVT_ON;
  if (st.st_ccsid == 0)
    fcvt.file_ccsid = 1047;
  else if ((st.st_ccsid == 1047 || st.st_ccsid == 819)
	   && (st.st_ftflags & FT_PURETXT) != 0)
    fcvt.file_ccsid = st.st_ccsid;
  else
    {
      /* Conversion will not occur for us, but we do not explicitly
	 disable because doing so may invalidate assumptions made by
	 other programs.  */
      fcvt.file_ccsid = FT_BINARY;
    }

  __fcntl64_nocancel (fd, F_CONTROL_CVT, &fcvt);
}

/* Because z/OS is such a mess of encodings, we have to guess what it
   means when the standard streams don't indicate their encoding. Without
   guessing that untagged input and output should really be treated as
   EBCDIC text, even the most basic of executables will not run correctly
   under the system shell.  */

void
__libc_set_conv_standard_fds (void)
{
  tag_one_fd (STDIN_FILENO);
  tag_one_fd (STDOUT_FILENO);
  tag_one_fd (STDERR_FILENO);
}

#include <csu/check_fds.c>
