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


/* fdopendir() unimplemented at the moment, default stub is used.

   In Linux it is possible to open directory entry with open() call, pass
   returned file descriptor to fdopendir() call and then use such directory
   calls as readdir(), rewinddir() and closedir() with that file descriptor.
   z/OS doesn't allow that. It is possible to use 'open' callable service
   in z/OS to get directory file descriptor but such descriptor cannot be
   used with specific directory callable services - "Bad file descriptor"
   error is occured. File descriptors for the same directory that are
   returned by callable services 'open' and 'opendir' are different
   descriptors even if they have the same value.

   To implement fdopendir() system call in z/OS we maybe have to refuse
   using z/OS directory callable services and to implement our own 
   directory system calls using raw directory file descriptor. */

#include <dirent/fdopendir.c>
