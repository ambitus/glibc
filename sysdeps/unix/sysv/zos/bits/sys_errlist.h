/* Declare sys_errlist and sys_nerr, or don't.  z/OS version.
   Copyright (C) 2020 Free Software Foundation, Inc.
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

#ifndef _STDIO_H
# error "Never include <bits/sys_errlist.h> directly; use <stdio.h> instead."
#endif

/* sys_errlist and sys_nerr are deprecated.  Use strerror instead.  */

#ifdef  __USE_MISC
extern int sys_nerr;
extern const char *const sys_errlist[];
#endif
#ifdef  __USE_GNU
extern int _sys_nerr;
extern const char *const _sys_errlist[];
#endif

#ifdef	__USE_POSIX
/* z/OS TODO: This is an absolute ugly hack, but we need a declaration
   for _fileno, setmode, and _setmode. Figure out a better place to put
   these things.  */

/* Return the system file descriptor for STREAM.  */
extern int _fileno (FILE *__stream) __THROW __wur;
#include <io.h>
#endif /* Use POSIX.  */
