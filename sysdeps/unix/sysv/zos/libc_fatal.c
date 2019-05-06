/* Catastrophic failure reports.  Linux version.
   Copyright (C) 1993-2018 Free Software Foundation, Inc.
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
#include <execinfo.h>
#include <fcntl.h>
#include <not-cancel.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <stdio.h>
#include <zos-estaex.h>

/* z/OS TODO: When we have libgcc's dwarf backtrace working, use that. */

static void
zos_backtrace (enum __libc_message_action action, bool written, int fd)
{
  if (action > do_message && written)
    {
      void *r13;
#define strnsize(str) str, strlen (str)
#define writestr(str) __write_nocancel (fd, str)
      writestr (strnsize ("======= Backtrace: =========\n"));
      __asm__ ("lgr	%0, %%r13" : "=r" (r13));
      __zos_dump_stack (fd, r13);

      /* z/OS TODO: Can we give something like the memory maps they
	 do on linux?  */
    }
}
#define BEFORE_ABORT		zos_backtrace

#include <sysdeps/posix/libc_fatal.c>
