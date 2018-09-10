#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <sys/cdefs.h>
#include "linux_syscall_shim.h"

typedef void (*bpx4opn_t)(const uint32_t *pathname_len,
			  const char * const *pathname,
			  const struct bpxyopnf * const *options,
			  const struct bpxymode * const *mode,
			  int32_t *retval, int32_t *retcode,
			  int32_t *reason_code);

/* TODO: PATH_MAX isn't sufficient. It's historically unreliable.
   Check to see what the maximum possible path that can be made on z/OS
   through any method is.

   pathconf -a shows PATH_MAX as 1023 (PATH_MAX should include the
   terminating null character). It's possible they just read the
   standard wrong, but use it anyway for now.
 */

int
__linux_compat_open (const char *pathname, int flags, mode_t mode)
{
  int32_t retval, retcode, reason_code;
  int32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH(pathname, -1);
  int32_t mode_field = mode_t_to_bpxymode(mode);
  BPX_CALL (open, bpx4opn_t, &pathname_len, &pathname, &flags,
	    &mode_field, &retval, &retcode, &reason_code);
  if (__glibc_unlikely(retval == -1))
    {
      __set_errno(retcode);
    }
  /* TODO: check important reason codes. */
  return retval;
}

int
__linux_compat_openat (int dirfd, const char *pathname, int flags,
		       mode_t mode)
{
  if (dirfd == AT_FDCWD || *pathname == '/')
    return __linux_compat_open(pathname, flags, mode);
  _SHIM_NOT_YET_IMPLEMENTED
}
