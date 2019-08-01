/* Implementation limits related to sys/uio.h - z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
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

#ifndef _BITS_UIO_LIM_H
#define _BITS_UIO_LIM_H 1

/* Maximum length of the 'struct iovec' array in a single call to
   readv or writev.

   Since the C library implementation of readv/writev is able to emulate
   the functionality even if the currently running kernel does not
   support this large value the readv/writev call will not fail because
   of this.  */
#define __IOV_MAX	120

#endif
