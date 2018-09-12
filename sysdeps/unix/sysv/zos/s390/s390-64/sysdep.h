/* Assembler macros for 64 bit z/OS.
   Copyright (C) 2018 Rocket Software.
   Contributed by Giancarlo Frix (gfrix@rocketsoftware.com).
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _ZOS_SYSDEP_H
#define _ZOS_SYSDEP_H 1

#include <sysdeps/s390/s390-64/sysdep.h>
#include <sysdeps/unix/sysdep.h>
#include <sysdeps/unix/sysv/linux/sysdep.h>
#include <dl-sysdep.h>	/* For RTLD_PRIVATE_ERRNO.  */
#include <tls.h>
#include <assert.h>
#include "linux_shims/linux_syscall_shim.h"

/* this file is based on sysdeps/unix/sysv/linux/s390/s390-64/sysdep.h */

#undef SYS_ify
#define ZOS_SYSDEP_CAT(a, b) a##b
#define SYS_ify(syscall_name)	ZOS_SYSDEP_CAT(__NR_, syscall_name)
#define __NR_syscall 0

#define SINGLE_THREAD_BY_GLOBAL		1

#ifdef __XPLINK__
# error "Building glibc with XPLINK is not supported (yet...)"
#endif /* __XPLINK__ */

#ifdef __ASSEMBLER__
/* dummy decls, we dont need assembler support yet */
# undef	PSEUDO
# define PSEUDO(name, syscall_name, args)
# undef	PSEUDO_END
# define PSEUDO_END(name)
# undef	PSEUDO_NOERRNO
# define PSEUDO_NOERRNO(name, syscall_name, args)
# undef	PSEUDO_END_NOERRNO
# define PSEUDO_END_NOERRNO(name)
# undef	PSEUDO_ERRVAL
# define PSEUDO_ERRVAL(name, syscall_name, args)
# undef	PSEUDO_END_ERRVAL
# define PSEUDO_END_ERRVAL(name)
# define ret
# define ret_NOERRNO
# define ret_ERRVAL
# define SYSCALL_ERROR
# define SYSCALL_ERROR_LABEL
# define SYSCALL_ERROR_HANDLER
# undef	DO_CALL
# define DO_CALL(syscall_name, args)
# define PTR_MANGLE(reg, tmpreg)
# define PTR_MANGLE2(reg, tmpreg)
# define PTR_DEMANGLE(reg, tmpreg)

#endif /* __ASSEMBLER__ */

/* base syscall implementation */
#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...) \
  ({ \
    extern SHIM_DECL (name); \
    SHIM_NAME (name) (&(err), args);   \
  })


#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)				\
  ({									\
    INTERNAL_SYSCALL_DECL (_sc_err);					\
    long _ret = INTERNAL_SYSCALL (name, _sc_err, nr, args);		\
    if (INTERNAL_SYSCALL_ERROR_P (_ret, _sc_err))			\
      {									\
	__set_errno (INTERNAL_SYSCALL_ERRNO (_ret, _sc_err));		\
	_ret = -1;  /* always return -1 on err */			\
      }									\
    _ret; })

/* the return code field must be unset before each call, because
   it might not get set at all. */
#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) int err = 0

#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(no, err, nr, args...)  \
  not yet implemented, choke gcc

/* z/OS TODO: this should be something that is always true if
   the syscall returns an error, and can check.

   Syscalls usually return -1 on error, but sometimes they return 0.
   Since we are always providing err to each call at the moment, and
   know it will always be initialized to 0, we just check if it's still
   0 at the moment. We might change it later to use both, which might
   allow us to avoid passing in that extra parameter at times. */
#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err) \
  ((void) (val), __builtin_expect ((err), 0))

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)	((void) (val), (err))

#undef PTR_MANGLE
#define PTR_MANGLE(var) \
  (var) = (void *) ((uintptr_t) (var) ^ THREAD_GET_POINTER_GUARD ())
#undef PTR_DEMANGLE
#define PTR_DEMANGLE(var)	PTR_MANGLE (var)

#endif /* _ZOS_SYSDEP_H */
