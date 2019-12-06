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

#ifndef _SYS_UTSNAME_H
# error "Never include <bits/utsname.h> directly; use <sys/utsname.h> instead."
#endif


/* Lengths of the entries in `struct utsname' in z/OS, according to
   BPXYUTSN data area mapping macros, plus one null byte.  */
#define _UTSNAME_SYSNAME_LENGTH  17
#define _UTSNAME_NODENAME_LENGTH 33
#define _UTSNAME_RELEASE_LENGTH   9
#define _UTSNAME_VERSION_LENGTH   9
#define _UTSNAME_MACHINE_LENGTH  17


/* Linux provides as additional information in the `struct utsname'
   the name of the current domain. z/OS does not provide such information.
   Define _UTSNAME_DOMAIN_LENGTH to a value 0 to disable this entry.  */
#define _UTSNAME_DOMAIN_LENGTH 0
