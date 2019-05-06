/* ESTAEX routines and SDWA mappings
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Michael Colavita <mcolavita@rocketsoftware.com>, 2018.

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

#ifndef _ZOS_ESTAEX_H
#define _ZOS_ESTAEX_H

#include <stdint.h>

#define SIZEOF_ESTAEX_PARM_LIST 24

/* TODO: expand each of these mappings */

struct sdwaptrs {
  uint32_t sdwanrc1;
  uint32_t sdwarc1;
  uint32_t sdwarc2;
  uint32_t sdwanrc2;
  uint32_t sdwarc3;
  uint32_t sdwanrc3;
  uint32_t sdwarc4;
  uint32_t sdwarc5;
};

struct sdwarc1 {
  uint8_t pad[216];
  uint64_t ars[16];
  uint8_t pad2[112];
};

struct sdwarc2 {
  uint8_t pad[16];
};

struct sdwarc3 {
  uint8_t pad[32];
};

struct sdwarc4 {
  uint64_t gprs[16];
  uint8_t pad[232];
};

struct sdwarc5 {
  uint8_t pad[144];
};

struct sdwanrc1 {
  uint8_t pad[240];
};

struct sdwanrc2 {
  uint8_t pad[16];
};

struct sdwanrc3 {
  uint8_t pad[240];
};

struct sdwa {
  uint8_t pad[24];
  uint32_t gprs[16];
  uint8_t pad2[280];
  uint32_t sdwaptrs;
  uint8_t pad3[292];
};

/* Modify with care. This struct is referenced in assembly. */
struct estaex_handler_data {
  void (*user_handler) (struct sdwa *, void *);
  void *user_data;
};

/* z/OS TODO: make these hidden.  */

extern int __set_estaex_handler (void (*) (struct sdwa *, void *),
				 void *);
extern void __estaex_handler_dump (struct sdwa *, void *);
extern void (*__estaex_handler_wrapper) (void);
extern void __zos_dump_stack (int fd, void *r13);


#endif  /* _ZOS_ESTAEX_H  */
