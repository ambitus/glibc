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

#ifndef __OPENAT64_H
#define __OPENAT64_H

#include "openat64-generic.h"
/* Must be included to use the function implictly used
   by this instance of the generic openat. */
#include "get-path-at-fd-cancel.h"

#define DO_OPENAT64(fd, file, oflag, mode)	\
  DO_OPENAT64_AS (, cancel, fd, file, oflag, mode)

#endif /* __OPENAT64_H */
