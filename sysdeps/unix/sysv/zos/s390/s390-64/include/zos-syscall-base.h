/* (C) 2018 Rocket Software
   Written by Giancarlo Frix.

   Common declarations for linux syscall wrappers.
   */
#ifndef _ZOS_SYSCALL_COMMON_H
#define _ZOS_SYSCALL_COMMON_H 1
#ifndef __ASSEMBLER__

#include <zos-syscall-table.h>

#include <stdint.h>
#include <features.h>
#include <zos-utils.h>

/* Ideally, at libc initialization time __bpx_call_table should be
   be set to the result of GET_BPX_FUNCTION_TABLE to shave off a few
   dereferences from each syscall invocation. However, I don't
   want to test out how well that works right now, so it is currently
   unused and GET_BPX_FUNCTION_TABLE is recomputed for every call.

   __bpx_call_table must be be declared exactly as follows.  */
extern uintptr_t __bpx_call_table attribute_hidden;


#define GET_BPX_FUNCTION_TABLE \
  GET_PTR31_UNSAFE (GET_PTR31_UNSAFE (GET_PTR31_UNSAFE ( \
  (volatile uintptr_t)(0x10)) + 544) + 24)

/* Return a pointer to the bpx syscall function that is at offset
   'offset' from the start of the bpx syscall table.

   z/OS TODO: FIXME: this is recomputed for every call right now.
   This function should be constant for a given offset and address
   space. */
#define BPX_FUNCTION_UNTYPED(offset)				  \
  ({								  \
    _Static_assert (offset && offset % 4 == 0,			  \
		    "incorrect offset to BPX service");		  \
    ((void *)GET_PTR31_UNSAFE (GET_BPX_FUNCTION_TABLE + offset)); \
  })

#define _SHIM_CAT(a, b) _SHIM_INDIR_CAT (a, b)
#define _SHIM_INDIR_CAT(a, b) a##b
#define _SHIM_STRINGIFY(a) _SHIM_INDIR_STRINGIFY (a)
#define _SHIM_INDIR_STRINGIFY(a) #a

#define SHIM_NAME(syscall_name) _SHIM_CAT (__zos_sys_, syscall_name)
/* Look up proto from our unholy table of protos */
#define SHIM_DECL(syscall_name) \
  _SHIM_CAT (__sys_proto_, syscall_name) (SHIM_NAME (syscall_name))
/* preform action if we have implemented syscall_name, for debugging.  */
#define SHIM_IF_ENABLED(syscall_name, action_if_true, action_if_false) \
  _SHIM_CAT (__shim_enabled_, syscall_name) (action_if_true, action_if_false)

/*************************************************************
 * macros and inline functions useful for implementing shims
 **************************************************************/
#include <sys/cdefs.h>	/* for __glibc_likely/unlikely */
#include <errno.h>
#include <string.h>  /* for strnlen.  */
#include <unimplemented.h>

extern void __bpxk_syscall (void *, ...);

/* I miss type checking...  */
#define BPX_CALL(name, ftype, args...)					\
  ((__bpxk_syscall							\
    ((BPX_FUNCTION_UNTYPED (_SHIM_CAT (__BPX_off_, name))), ## args)))

#define SHIM_RETURN_UNSUPPORTED(retval)		\
  ({						\
    *errcode = ENOSYS;				\
    return (retval);				\
  })

#define ZOS_DEBUG 0

/* TODO: This isn't perfect. We ideally want something like
   __libc_fatal, but without a dependency on all of stdio. And assuming
   that fd 2 is always stderr is not great. Users can close stderr.  */

#if ZOS_DEBUG == 1
# define SHIM_NOT_YET_IMPLEMENTED(msg)					\
  ({									\
    const char __errmsg[] =						\
      "Some functionality in the z/OS port of glibc has not yet been "	\
      "fully implemented. Unimplemented at "				\
      _SHIM_STRINGIFY (__FILE__) ":" _SHIM_STRINGIFY (__LINE__) "\n"	\
      "Additional info: " msg "\n";					\
    __zos_sys_write (2, __errmsg, sizeof (__errmsg));			\
  })
# define SHIM_NOT_YET_IMPLEMENTED_FATAL(msg, retval) \
  __GLIBC_ZOS_RUNTIME_UNIMPLEMENTED (msg)
#else
# define SHIM_NOT_YET_IMPLEMENTED(msg) do { } while (0)
# define SHIM_NOT_YET_IMPLEMENTED_FATAL(msg, retval) \
  SHIM_RETURN_UNSUPPORTED (retval)
#endif

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
    int __pathname_len = __strnlen (__pathname, PATH_MAX - 1);	    \
    if (__glibc_unlikely (__pathname_len == PATH_MAX - 1) &&	    \
	__glibc_likely (__pathname[PATH_MAX - 1] != '\0'))	    \
      {								    \
	*errcode = ENAMETOOLONG;				    \
	return (return_value_on_error);				    \
      }								    \
    __pathname_len;						    \
  })

/* Check if the given value can fit into an unsigned int of the given
   size in bits. This is imperfect, but good enough for us.  */
#define IS_UINT(bitsize, val)					    \
  ({								    \
    _Static_assert (__builtin_constant_p (bitsize), "bitsize "	    \
		    "should be a constant");			    \
    _Static_assert (bitsize > 0, "uints <= 0 make no sense.");	    \
    _Static_assert (bitsize <= 64, "this macro can't check "	    \
		    "integers larger than a uint64_t");		    \
    __typeof (val) _val = (val);				    \
    (_val) >= 0 && (bitsize == 64 || (_val) < (1ULL << (bitsize))); \
  })

#define IS_UINT32(val) IS_UINT (32, val)

/* all the syscall implementations. */
#include <zos-syscall-impl.h>

#endif /* __ASSEMBLER__ */
#endif /* _ZOS_SYSCALL_COMMON_H */
