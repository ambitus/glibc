/* Thread pointer access for z/OS.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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

#include <stddef.h>
#include <tls.h>
#include <lock-free.h>
#include <zos-core.h>

/* The hash table in which we store thread pointers.  */
lf_hash_table *__zos_tp_table = NULL;
libc_hidden_data_def (__zos_tp_table)


void *
__zos_get_thread_pointer (void)
{
  unsigned int task_addr = __get_zos_tcb_addr ();
  return (void *) (uintptr_t) __lf_hash_table_get (task_addr,
						   __zos_tp_table);
}
libc_hidden_def (__zos_set_thread_pointer)


void
__zos_set_thread_pointer (void *addr)
{
  unsigned int task_addr = __get_zos_tcb_addr ();
  __lf_hash_table_put (task_addr, (uintptr_t) addr, __zos_tp_table);
}
libc_hidden_def (__zos_set_thread_pointer)


void
__zos_clear_thread_pointer (void)
{
  unsigned int task_addr = __get_zos_tcb_addr ();
  __lf_hash_table_pop (task_addr, __zos_tp_table);
}
libc_hidden_def (__zos_clear_thread_pointer)
