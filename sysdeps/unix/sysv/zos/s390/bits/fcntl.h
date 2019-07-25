/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#ifndef	_FCNTL_H
# error "Never use <bits/fcntl.h> directly; include <fcntl.h> instead."
#endif

/* F_DUPFD, F_GETFD, F_SETFD, F_GETFL, and F_SETFL are the same for linux
   and z/OS.  */

/* We always have 64-bit offsets.  */
#define F_GETLK		5	/* Get record locking info.  */
#define F_SETLK		6	/* Set record locking info (non-blocking).  */
#define F_SETLKW	7	/* Set record locking info (blocking).  */
#define F_GETLK64	F_GETLK
#define F_SETLK64	F_SETLK
#define F_SETLKW64	F_SETLKW

#define __F_SETOWN	10
#define __F_GETOWN	11

#define F_SETTAG	12	/* Set file tag.  */
#define F_CONTROL_CVT	13	/* Set/get conversion state.  */

/* We don't need to support __USE_FILE_OFFSET64.  */
struct flock
{
  short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.	*/
  short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
  __off_t l_start;	/* Offset where the lock begins.  */
  __off_t l_len;	/* Size of the locked area; zero means until EOF.  */
  __pid_t l_pid;	/* Process holding the lock.  */
};

#ifdef __USE_LARGEFILE64
struct flock64
{
  short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.	*/
  short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
  __off64_t l_start;	/* Offset where the lock begins.  */
  __off64_t l_len;	/* Size of the locked area; zero means until EOF.  */
  __pid_t l_pid;	/* Process holding the lock.  */
};
#endif

/* z/OS TODO: Decide on a name for the F_CVT structure, file tag
   structure, and constants.  */
struct zos_file_tag
{
  unsigned short int ft_ccsid;
#define FT_UNTAGGED	0
#define FT_BINARY	0xffff

  unsigned short int ft_flags;
#define FT_PURETXT	0x8000
#define FT_DEFER	0x4000
};

struct zos_fconvert
{
  int command;
#define F_CVT_OFF	0	/* Disable conversion  */
#define F_CVT_ON	1	/* Enable conversion  */
#define F_CVT_AUTO_ON	2
#define F_CVT_QUERY	3	/* Query conversion state  */
#define F_CVT_ALL	4	/* Enable unicode conversion  */
#define F_CVT_AUTO_ALL	5
  unsigned short int prog_ccsid;
  unsigned short int file_ccsid;
};

/* These are fake definitions. We don't support SET/GETSIG, but
   their default numbers conflict with SET/GETOWN.
   z/OS TODO: Should we not do this, and just leave them undefined?  */
# define __F_SETSIG	90	/* Set number of signal to be sent.  */
# define __F_GETSIG	91	/* Get number of signal to be sent.  */

/* Include generic Linux declarations.
   z/OS TODO: Should we avoid using linux's fcntl stuff and leave the
   flags that we can't support undefined, so the user has some way of
   knowing that they are not supported at compile time?  */
#include <bits/fcntl-linux.h>
