/* (C) 2018 Rocket Software
   Written by Giancarlo Frix.

   Common declarations for linux syscall wrappers.
   */
#ifndef _ZOS_LINUX_SYSCALL_SHIM_H
#define _ZOS_LINUX_SYSCALL_SHIM_H 1
#ifndef __ASSEMBLER__

#include <zos-syscall-table.h>

#include <stdint.h>
#include <unimplemented.h>

#include <zos-syscall-table.h>

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
/* end generic utility macros */



#define SHIM_NAME(syscall_name) _SHIM_CAT (__linux_compat_, syscall_name)
/* Look up proto from our unholy table of protos */
#define SHIM_DECL(syscall_name) \
  _SHIM_CAT (__sys_proto_, syscall_name) (SHIM_NAME(syscall_name))
/* preform action if we have implemented syscall_name, for debugging.  */
#define SHIM_IF_ENABLED(syscall_name, action_if_true, action_if_false) \
  _SHIM_CAT (__shim_enabled_, syscall_name) (action_if_true, action_if_false)

/*************************************************************
 * macros and inline functions useful for implementing shims
 **************************************************************/
#include <errno.h>  /* for __set_errno */
#include <sys/cdefs.h>	/* for __glibc_likely/unlikely */

#define BPX_CALL(name, ftype, args...) \
  (((ftype) (BPX_FUNCTION_UNTYPED (_SHIM_CAT (__BPX_off_, name)))) (args))

/* emit an error at runtime */
#define _SHIM_NOT_YET_IMPLEMENTED \
  __GLIBC_ZOS_RUNTIME_UNIMPLEMENTED

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
    int __pathname_len = strnlen (__pathname, PATH_MAX - 1);	    \
    if (__glibc_unlikely (__pathname_len == PATH_MAX - 1) &&	    \
	__glibc_likely (__pathname[PATH_MAX - 1] != '\0'))	    \
      {								    \
	*errcode = ENAMETOOLONG;				    \
	return (return_value_on_error);				    \
      }								    \
    __pathname_len;						    \
  })

/* Check if the given value can fit into an unsigned int of the given
   size in bits.   */
#define IS_UINT(bitsize, val)					    \
  ({								    \
    _Static_assert (__builtin_constant_p (bitsize), "bitsize "	    \
		    "should be a constant");			    \
    _Static_assert (bitsize > 0, "uints <= 0 make no sense.");	    \
    _Static_assert (bitsize <= 64, "this macro can't check "	    \
		    "integers larger than a uint64_t");		    \
    __typeof (val) _val = (val);				    \
    (_val) >= 0 && (_val) < (1ULL << (bitsize));		    \
  })

#define IS_UINT32(val) IS_UINT (32, val)

#endif /* __ASSEMBLER__ */
#endif /* _ZOS_LINUX_SYSCALL_SHIM_H */
