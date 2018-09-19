#include <zos-syscall-base.h>

/* race condition? */
static __always_inline void
__setup_bpx_table_ptr (void)
{
  __bpx_call_table = GET_BPX_FUNCTION_TABLE;
}

/* co-opt VDSO_SETUP to do our initialization, since we aren't
   using it for anything else. */
#define VDSO_SETUP __setup_bpx_table_ptr

#include <csu/init-first.c>
