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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _ZOS_FUTEX_H
#define _ZOS_FUTEX_H 1

#include <stdint.h>

typedef enum
{
  ECB,
  cond_syscalls
} wait_token_type;

typedef struct
{
  wait_token_type type;
  union {
    uint32_t ecb;
    uint64_t thread_id;
  };
} wait_token;

extern lf_hash_table *__zos_futex_table;
libc_hidden_proto (__zos_futex_table)

extern object_pool __wait_token_pool;
libc_hidden_proto (__wait_token_pool)

#endif /* !_ZOS_FUTEX_H  */
