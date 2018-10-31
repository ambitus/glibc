#include <zos-syscall-base.h>
#include <zos-core.h>

/* race condition? */
static __always_inline void
__zos_early_setup (void)
{
  /* __bpx_call_table = GET_BPX_FUNCTION_TABLE; */

  /* Get the address of the current thread's Task Control Block.  */
  __ipt_zos_tcb = __get_zos_tcb_addr ();
}

/* co-opt VDSO_SETUP to do our initialization, since we aren't
   using it for anything else. */
#define VDSO_SETUP __zos_early_setup

#include <csu/init-first.c>
