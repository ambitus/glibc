/* Copyright (C) 2018 Rocket Software.
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
   <http://www.gnu.org/licenses/>.

   This file (and any #includes of it) should not be present in
   release versions of glibc. Hopefully it should be obselete when
   we get to that point.  */

#ifndef _ZOS_UNIMPLEMENTED_H
#define _ZOS_UNIMPLEMENTED_H 1

#define ___GLIBC_ZOS_STRINGIFY(a) ___GLIBC_ZOS_INDIR_STRINGIFY(a)
#define ___GLIBC_ZOS_INDIR_STRINGIFY(a) #a

#define __GLIBC_ZOS_CHOKE_COMPILER_UNIMPLEMENTED \
  (sizeof(char[-1]) && "compile-time unimplemented error: the glibc feature associated with this macro is not yet implemented for z/OS.")
#define __GLIBC_ZOS_STUB_UNIMPLEMENTED do { } while (0)

#define __GLIBC_ZOS_BUILDTIME_UNIMPLEMENTED \
  __GLIBC_ZOS_CHOKE_COMPILER_UNIMPLEMENTED

#ifndef _STDIO_H
extern void __libc_fatal (const char *__message)
  __attribute__ ((__noreturn__));
#endif /* _STDIO_H  */

#define __GLIBC_ZOS_RUNTIME_UNIMPLEMENTED			 \
  __libc_fatal("not yet implemented in the z/OS port"		 \
	       "in func: " ___GLIBC_ZOS_STRINGIFY(__FUNCTION__)	 \
	       " in file " ___GLIBC_ZOS_STRINGIFY(__FILE__)	 \
	       ":"	   ___GLIBC_ZOS_STRINGIFY(__LINE__))

#endif /* _ZOS_UNIMPLEMENTED_H  */
