/* Copyright (C) 2020-2021 Free Software Foundation, Inc.
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

/* define ZOS_DEBUG_CHECK_FDS and set env var ZOS_TAG_DEBUG to
   a non-zero value to get debug info on stderr */
#define ZOS_DEBUG_CHECK_FDS 0

#if ZOS_DEBUG_CHECK_FDS
#include <unistd.h>
#include <stdlib.h>

static const char*
zos_tag_file_type_name (struct stat *st)
{
    if (st) {
	if (S_ISCHR (st->st_mode))
	  return "CHR";
	if (S_ISDIR (st->st_mode))
	  return "DIR";
	if (S_ISBLK (st->st_mode))
	  return "BLK";
	if (S_ISREG (st->st_mode))
	  return "REG";
	if (S_ISFIFO (st->st_mode))
	  return "FIFO";
	if (S_ISLNK (st->st_mode))
	  return "LNK";
	if (S_ISSOCK (st->st_mode))
	  return "SOCK";
    }
    return "";
}

static void
write_number (long long n)
{
  char digit;
  const char *debug = getenv ("ZOS_TAG_DEBUG");
  debug = (debug == NULL) ? "0" : (debug[0] == '\0') ? "0" : debug;

  if (debug[0] == '0')
    return;
  if (n == 0)
    {
      digit = '0';
      __write (2, &digit, 1);
    }
  else
    {
      int d = n % 10;
      n = n / 10;
      digit = d + '0';
      write_number (n);
      __write (2, &digit, 1);
    }
  return;
}

static void
write_string (const char *string)
{
  const char *debug = getenv ("ZOS_TAG_DEBUG");
  debug = (debug == NULL) ? "0" : (debug[0] == '\0') ? "0" : debug;
  if (debug[0] == '0') return;

  size_t len = 0;
  while (string[len] != '\0')
    len++;

  __write (2, string, len);
}

#define zos_tag_trace(arg) \
    do {\
	write_string (__FILE__);\
	write_string (":");\
	write_number (__LINE__);\
	write_string (arg);\
    } while (0)
#define zos_tag_trace_info(arg) zos_tag_trace ("INFO: " arg)
#define zos_tag_trace_error(arg) zos_tag_trace ("ERROR: " arg)
#define zos_tag_trace_stat(rc, st)			\
  do							\
    {							\
      if (rc == -1)					\
	  {						\
	    zos_tag_trace_error ("stat failed");	\
	    break;					\
	  }						\
      zos_tag_trace_info ("stat:");			\
      zos_tag_trace_info ("  st.st_size: ");		\
      write_number ((long long) st.st_size);		\
      write_string ("\n");					\
      zos_tag_trace_info ("  st.st_tag.ft_txtflag: ");			\
      write_number (!!(st.st_ftflags & __ZOS_STAT_FT_ISTEXT));		\
      write_string ("\n");						\
      zos_tag_trace_info ("  st.st_tag.ft_ccsid: ");			\
      write_number ((int) st.st_ccsid);					\
      write_string ("\n");						\
      zos_tag_trace_info ("  st.st_tag.ft_deferred: ");			\
      write_number (!!(st.st_ftflags & __ZOS_STAT_FT_ISDEFERRED));	\
      write_string ("\n");						\
      zos_tag_trace_info ("  file type: ");				\
      write_string (zos_tag_file_type_name (&st));			\
      write_string ("\n");						\
    }									\
  while (0)
#define zos_tag_trace_query_attr(rc, cvt)		\
  do							\
    {							\
      if (rc == -1)					\
	  {						\
	    zos_tag_trace_error ("query_attr failed");	\
	    break;					\
	  }						\
      zos_tag_trace_info ("query_attr:");		\
      write_string ("\n");				\
      zos_tag_trace_info ("  cvt.file_ccsid: ");	\
      write_number ((int) cvt.file_ccsid);		\
      write_string ("\n");				\
      zos_tag_trace_info ("  cvt.prog_ccsid: ");	\
      write_number ((int) cvt.prog_ccsid);		\
      write_string ("\n");				\
      zos_tag_trace_info ("  cvt.command: ");		\
      write_number ((int) cvt.command);			\
      write_string ("\n");				\
    }							\
  while (0)
#else
#define zos_tag_trace(arg)
#define zos_tag_trace_info(arg)
#define zos_tag_trace_error(arg)
#define zos_tag_trace_stat(rc, st)
#define zos_tag_trace_query_attr(rc, cvt)
#endif

static int
query_attr (int fd, struct zos_fconvert *cvt)
{
  cvt->prog_ccsid = 0;
  cvt->file_ccsid = 0;
  cvt->command = F_CVT_QUERY;

  return __fcntl64_nocancel (fd, F_CONTROL_CVT, cvt);
}

static int
disable_conversion_fd (int fd)
{
#if ZOS_DEBUG_CHECK_FDS
  write_string (__func__);
  write_string ("(");
  write_number (fd);
  write_string (")\n");
#endif

  struct zos_fconvert cvt =
    { .prog_ccsid = 0, .file_ccsid = 0, .command = F_CVT_OFF };
  return __fcntl64_nocancel (fd, F_CONTROL_CVT, &cvt);
}

static int
enable_conversion_fd (int fd, unsigned short int ccsid)
{
#if ZOS_DEBUG_CHECK_FDS
  write_string (__func__);
  write_string ("(");
  write_number (fd);
  write_string (", ");
  write_number (ccsid);
  write_string (")\n");
#endif
  /* don't use conversion when it's useless  */
  if (ccsid == FT_BINARY || ccsid == FT_UNTAGGED || ccsid == 819)
    {
      /* This does work correctly, dispite looking like a mistake. */
      disable_conversion_fd (fd);
    }

  struct zos_fconvert cvt =
    { .prog_ccsid = 0, .file_ccsid = ccsid, .command = F_CVT_ON };
  return __fcntl64_nocancel (fd, F_CONTROL_CVT, &cvt);
}

static int
tag_one_fd (int fd)
{
#if ZOS_DEBUG_CHECK_FDS
  write_string (__func__);
  write_string ("(");
  write_number (fd);
  write_string (")\n");
#endif

  int rc = 0;
  struct zos_fconvert cvt;
  struct stat64 st;

  /* The 'deferred' flag can only be obtained via fstat(). */
  rc = __fstat64 (fd, &st);

#if ZOS_DEBUG_CHECK_FDS
  zos_tag_trace_stat (rc, st);
  write_string ("\n");
#endif
  /* We don't touch stdin pipes: under /bin/sh, they look as if they
     were untagged but the pipe is actually tagged with ft_deferred=1.
     We don't touch deferred tags because such a stream will eventually
     receive a tag (and conversion) from the reader or writer. */

  if (rc == 0
      && !(st.st_ftflags & __ZOS_STAT_FT_ISDEFERRED)
      && !(fd == STDIN_FILENO && S_ISFIFO (st.st_mode)))
    {
      /* Note: the order of fstat() and query_attr() calls DOES matter.
	 If query_attr is called first, there's a chance that the deferred
	 tagging changes stream attributes after it but before fstat().
	 Here, we've already discarded deferred tags, so it's safe to
	 check other properties.

	 Also note that st.st_tag.ft_ccs and cvt.fccsid are NOT equivalent,
	 and that's why we have to use query_attr(). */

      rc = query_attr (fd, &cvt);
#if ZOS_DEBUG_CHECK_FDS
      zos_tag_trace_query_attr (rc, cvt);
      write_string ("\n");
#endif
      /* We only set up conversion if it's not configured already,
	 e.g. by the shell.  */
      if (rc == 0)
	{
	  /* by default standard streams are tagged IBM-1047 */
	  if (cvt.file_ccsid == FT_UNTAGGED)
	    cvt.file_ccsid = 1047;

	  rc = enable_conversion_fd (fd, cvt.file_ccsid);
	}
    }
  return rc;
}

/* Because z/OS is such a mess of encodings, we have to guess what it
   means when the standard streams don't indicate their encoding. Without
   guessing that untagged input and output should really be treated as
   EBCDIC text, even the most basic of executables will not run correctly
   under the system shell.  */
void
__libc_set_conv_standard_fds (void)
{
  /* z/OS TODO: is the LE call
     __ae_autoconvert_state(_CVTSTATE_ON); from libtag significant?
     things seem to work without it. Maybe it will become important
     when we start to have threads? */
  tag_one_fd (STDIN_FILENO);
  tag_one_fd (STDOUT_FILENO);
  tag_one_fd (STDERR_FILENO);
}

#include <csu/check_fds.c>
