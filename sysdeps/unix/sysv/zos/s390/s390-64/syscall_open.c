#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <zos-syscall-base.h>

typedef void (*bpx4opn_t)(const uint32_t *pathname_len,
			  const char * const *pathname,
			  const int32_t *options,
			  const mode_t *mode,
			  int32_t *retval, int32_t *retcode,
			  int32_t *reason_code);

int
__zos_sys_open (int *errcode, const char *pathname,
		int flags, mode_t mode)
{
  int32_t retval, reason_code;
  uint32_t pathname_len = SAFE_PATHLEN_OR_FAIL_WITH (pathname, -1);
  BPX_CALL (open, bpx4opn_t, &pathname_len, &pathname, &flags,
	    &mode, &retval, errcode, &reason_code);
  /* TODO: check important reason codes. */
  return retval;
}
hidden_def (__zos_sys_open)


int
__zos_sys_openat (int *errcode, int dirfd, const char *pathname,
		  int flags, mode_t mode)
{
  if (dirfd == AT_FDCWD || *pathname == '/')
    return __zos_sys_open (errcode, pathname, flags, mode);
  SHIM_NOT_YET_IMPLEMENTED;
}
hidden_def (__zos_sys_openat)
