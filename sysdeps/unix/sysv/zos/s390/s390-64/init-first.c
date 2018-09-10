#include "syscall_lookup.c"
/* co-opt VDSO_SETUP to do our initialization, since we aren't
   using it for anything else. */
#define VDSO_SETUP __setup_bpx_table_ptr

#include <csu/init-first.c>
