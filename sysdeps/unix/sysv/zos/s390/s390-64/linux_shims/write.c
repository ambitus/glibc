#include <unistd.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include "linux_syscall_shim.h"

void (*bpx4wrt_t) (const int32_t *fd, const void * const *buf,
		   const int32_t *buf_alet, const int32_t *count,
		   int32_t *retval, int32_t *retcode,
		   int32_t *reason_code);

ssize_t
__linux_compat_write (int fd, const void *buf, size_t nbytes)
{
  int32_t retval, retcode, reason_code;
  const int alet = 0;
  BPX_CALL (write, bpx4wrt_t, &fd, &buf, &alet, &nbytes, &retval,
	    &retcode, &reason_code);
  if (__glibc_unlikely (retval == -1))
    {
      __set_errno(retcode);
    }
  /* TODO: check important reason codes */
  return retval;
}
