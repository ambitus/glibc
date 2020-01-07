/* Support for dynamic linking code in static libc.
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

#include <elf.h>
#include <zos-init.h>

#include <elf/dl-support.c>

int _dl_clktck;

extern ElfW(Ehdr) __ehdr_start __attribute__ ((visibility ("hidden")));

/* Do most of the initialization that _dl_aux_init would do.  */

void
_dl_psuedo_aux_init (void)
{
  _dl_min_init ();

  GL(dl_phdr) = (const void *) &__ehdr_start + __ehdr_start.e_phoff;
  GL(dl_phnum) = __ehdr_start.e_phnum;
}
