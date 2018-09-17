#include <unistd.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include "linux_syscall_shim.h"

typedef void (*bpx4wrt_t) (const int32_t *fd, const void * const *buf,
			   const int32_t *buf_alet,
			   const uint32_t *count,
			   int32_t *retval, int32_t *retcode,
			   int32_t *reason_code);

ssize_t
__linux_compat_write (int *errcode, int fd,
		      const void *buf, size_t nbytes)
{
  int32_t retval, reason_code;
  uint32_t count;
  const int32_t alet = 0;
  if (!IS_UINT32 (nbytes))
    {
      /* z/OS can't handle writes larger than a uint32 */
      /* TODO: is there a better errno for this? */
      *errcode = EOVERFLOW;
      return -1;
    }
  count = nbytes;
  BPX_CALL (write, bpx4wrt_t, &fd, &buf, &alet, &count,
	    &retval, errcode, &reason_code);
  /* TODO: check important reason codes */
  return retval;
}
