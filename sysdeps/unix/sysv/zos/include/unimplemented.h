#define __GLIBC_INTERNAL_CHOKE_COMPILER_UNIMPLEMENTED \
  (sizeof(char[-1]) && "compile-time unimplemented error: the glibc feature associated with this macro is not yet implemented for z/OS.")
#define __GLIBC_INTERNAL_STUB_UNIMPLEMENTED do { } while (0)
#define __GLIBC_ZOS_INTERNAL_UNIMPLEMENTED __GLIBC_CHOKE_COMPILER_UNIMPLEMENTED
