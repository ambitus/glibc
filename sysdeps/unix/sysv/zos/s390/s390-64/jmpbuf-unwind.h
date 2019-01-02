/* Copyright (C) 2018 Free Software Foundation, Inc.
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

#include <stdint.h>
#include <jmpbuf-offsets.h>

/* Test if longjmp to JMPBUF would unwind the frame
   containing a local variable at ADDRESS.

   TODO: Is this right?
   TODO: This will not work if the stack ever splits. (If we are being
   called by foreign code).
   FIXME: We probably shouldn't examine the memory around the address,
   but without checking the NAB (which is the stack pointer), we don't
   know where the given frame ends.  */
#define _JMPBUF_UNWINDS(jmpbuf, address, demangle)			\
  ({									\
    uintptr_t _frame_dsa = demangle ((jmpbuf)->__gregs[__JB_GPR13]);	\
    void *_nab = ((void **) _frame_dsa)[17];				\
    ((void *) (address) > _nab);					\
  })


/* On 64-bit z/OS, CFA is always %r13.  */
#define _JMPBUF_CFA_UNWINDS_ADJ(_jmpbuf, _context, _adj)		\
  _JMPBUF_UNWINDS_ADJ (_jmpbuf,						\
		       ((void **) _Unwind_GetCFA (_context))[17], _adj)

static inline uintptr_t __attribute__ ((unused))
_jmpbuf_sp (__jmp_buf regs)
{
  void *fp = (void *) (uintptr_t) regs[0].__gregs[__JB_GPR13];
#ifdef PTR_DEMANGLE
  PTR_DEMANGLE (fp);
#endif
  return ((uintptr_t *) fp)[17];
}

#define _JMPBUF_UNWINDS_ADJ(_jmpbuf, _address, _adj)			\
  ((uintptr_t) (_address) - (_adj) > _jmpbuf_sp (_jmpbuf) - (_adj))

/* We use the normal longjmp for unwinding.  */
#define __libc_unwind_longjmp(buf, val) __libc_longjmp (buf, val)
