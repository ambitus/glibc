#include <linux_syscall_shim.h>

/* race condition? */
static inline void
__setup_bpx_table_ptr (void)
{
  bpx_call_table = GET_BPX_FUNCTION_TABLE;
}
