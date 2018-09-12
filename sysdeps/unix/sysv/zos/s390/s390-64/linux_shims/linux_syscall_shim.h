/* (C) 2018 Rocket Software
   Written by Giancarlo Frix.

   Common declarations for linux syscall wrappers.
   */
#ifndef _ZOS_LINUX_SYSCALL_SHIM_H
#define _ZOS_LINUX_SYSCALL_SHIM_H 1
#ifndef __ASSEMBLER__

#include <zos-syscall-table.h>

#include <stdint.h>

uintptr_t bpx_call_table;

/* a regular (64-bit) pointer to a 31-bit pointer */
typedef uintptr_t ptr31ptr_t;

/* get a reference to bpx call table
   use a variant of Michael's macros for now
   Follow a series of 31-bit pointers, which is a bit awkward since we
   are in 64-bit mode. The high bits are all 0 so we don't need to mask it
   off. */

#define GET_PTR31_UNSAFE(x) ((uintptr_t)(*(uint32_t *)(x)))
#define GET_PTR31_SAFE(x) ((uintptr_t)(~(1UL << 31) & *(uint32_t *)(x)))

#define GET_BPX_FUNCTION_TABLE \
  GET_PTR31_UNSAFE (GET_PTR31_UNSAFE (GET_PTR31_UNSAFE ( \
  (volatile uintptr_t)(0x10)) + 544) + 24)

/* z/OS TODO: FIXME: this is recomputed for every call right now. */
#define BPX_FUNCTION_UNTYPED(offset)			  \
  ({							  \
    _Static_assert(offset && offset % 4 == 0,		  \
		   "incorrect offset to BPX service");	  \
    ((void *)GET_PTR31_UNSAFE (bpx_call_table + offset)); \
  })

/* generic utility macros */
#define _SHIM_CAT(a, b) _SHIM_INDIR_CAT (a, b)
#define _SHIM_INDIR_CAT(a, b) a##b
#define _SHIM_STRINGIFY(a) _SHIM_INDIR_STRINGIFY (a)
#define _SHIM_INDIR_STRINGIFY(a) #a
/* end generic utility macros */



#define SHIM_NAME(syscall_name) _SHIM_CAT (__linux_compat_, syscall_name)
/* Look up proto from our unholy table of protos */
#define SHIM_DECL(syscall_name) \
  _SHIM_CAT (__sys_proto_, syscall_name) (SHIM_NAME(syscall_name))

/*************************************************************
 * macros and inline functions useful for implementing shims
 **************************************************************/
#include <errno.h>  /* for __set_errno */
#include <sys/cdefs.h>	/* for __glibc_likely/unlikely */

extern void __libc_fatal (const char *__message) __attribute__ ((__noreturn__));
#define BPX_CALL(name, ftype, args...) \
  ((ftype) (BPX_FUNCTION_UNTYPED (_SHIM_CAT (__BPX_off_, name)))) (args)

/* emit an error at runtime */
#define _SHIM_NOT_YET_IMPLEMENTED			\
  __libc_fatal("not yet implemented in the z/OS port:"	\
	       " file: " _SHIM_STRINGIFY(__FILE__)	\
	       " func: " _SHIM_STRINGIFY(__FUNCTION__)	\
	       " line: " _SHIM_STRINGIFY(__LINE__));

/* z/OS TODO: PATH_MAX isn't sufficient. It's historically unreliable.
   Check to see what the maximum possible path that can be made on z/OS
   through any method is.

   pathconf -a shows PATH_MAX as 1023 (PATH_MAX should include the
   terminating null character). It's possible they just read the
   standard wrong, but use it anyway for now. */

/* get the length of a path, stopping at PATH_MAX, setting errno, and
   returning a specified value if PATH_MAX - 1 isn't the end of the
   string. */
#define SAFE_PATHLEN_OR_FAIL_WITH(pathname, return_value_on_error)  \
  ({								    \
    const char *__pathname = (pathname);			    \
    int __pathname_len = strnlen(__pathname, PATH_MAX - 1);	    \
    if (__glibc_unlikely(__pathname_len == PATH_MAX - 1) &&	    \
	__glibc_likely(__pathname[PATH_MAX - 1] != '\0'))	    \
      {								    \
	__set_errno(ENAMETOOLONG);				    \
	return (return_value_on_error);				    \
      }								    \
    __pathname_len;						    \
  })

#include <sys/stat.h>

#endif /* __ASSEMBLER__ */
#endif /* _ZOS_LINUX_SYSCALL_SHIM_H */
