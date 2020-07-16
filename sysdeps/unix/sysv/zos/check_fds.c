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

  /* If the file is an empty untagged regular file open for writing, try
     to tag the underlying file (in addition to enabling conversion on
     the file descriptor). Never modify stdin, even if it is writeable.
     z/OS TODO: This should obey the environment variable controlling
     new file tags.  */
  if (S_ISREG (st.st_mode)
      && st.st_size == 0
      && st.st_ccsid != 0
      && fd != STDIN_FILENO)
    {
      int mode = (O_ACCMODE & __fcntl64_nocancel (fd, F_GETFL));
      if (mode == O_WRONLY || mode == O_RDWR)
	{
	  struct zos_file_tag ft;
	  ft.ft_ccsid = 819;
	  ft.ft_flags = FT_PURETXT;
	  /* z/OS TODO: Race condition here.  */
	  __fcntl64_nocancel (fd, F_SETTAG, &ft);
	}
    }

  /* z/OS TODO: This needs to obey first the override env vars, then
     the env var controlling whether to treat untagged files as EBCDIC
     or binary.  */

  /* Enable conversion unless the input file is explictly tagged as
     something other than IBM-1047, or is tagged as IBM-1047 but is
     non-text.  */
  fcvt.prog_ccsid = 0;  /* Obey Thliccsid.  */
  if (st.st_ccsid == 0
      || (st.st_ccsid == 1047
	  && (st.st_ftflags & FT_PURETXT) != 0))
    {
      /* Untagged, or explictly tagged as EBCDIC, assume EBCDIC text.  */
      fcvt.file_ccsid = 1047;
      fcvt.command = F_CVT_ON;
    }
  else
    {
      /* Treat as binary.  */
      fcvt.file_ccsid = 0;
      fcvt.command = F_CVT_OFF;
    }

  __fcntl64_nocancel (fd, F_CONTROL_CVT, &fcvt);
}

/* Because z/OS is such a mess of encodings, we have to guess what it
   means when the standard streams don't indicate their. Without guessing
   that untagged input and output should really be treated as EBCDIC
   text, even the most basic of executables will not run correctly under
   the system shell.  */

void
__libc_set_conv_standard_fds (void)
{
  tag_one_fd (STDIN_FILENO);
  tag_one_fd (STDOUT_FILENO);
  tag_one_fd (STDERR_FILENO);
}

#include <csu/check_fds.c>
