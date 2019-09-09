/* Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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

#ifndef _MAP_INFO_H
#define _MAP_INFO_H 1

#include <stdint.h>
#include <stdbool.h>

/* An anonymous mapping.  */
struct map_info
{
  /* Start of the mapping.  */
  void *start;

  /* Length.  */
  uint64_t length;

  /* 31-bit addr of the associated TCB.  */
  uint32_t tcbaddr;

  /* EXECUTABLE=NO specified.  */
  bool noexec;
};

#endif /* !_MAP_INFO_H  */
