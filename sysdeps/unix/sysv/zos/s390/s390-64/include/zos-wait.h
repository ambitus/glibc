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

#ifndef _ZOS_WAIT_H
#define _ZOS_WAIT_H

#include <stdint.h>

typedef struct
{
  int32_t k_si_signo;
  int32_t k_si_errno;
  int32_t k_si_code;
  int32_t k_si_pid;
  int32_t k_si_uid;
  int32_t k_si_res01;
  int32_t k_si_status;
  int32_t k_si_res02;
  int32_t k_si_res03;
  int32_t k_si_res04;
  void *k_si_addr;
  int64_t k_si_band;
  void *k_si_value;
} kernel_siginfo_t;

#endif  /* !_ZOS_WAIT_H  */
