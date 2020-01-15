/* z/OS TODO: There should be a __libc_vfork symbol in libc.
   remove define when it exists.  */
#define __libc_vfork __vfork
#include <sysdeps/unix/sysv/linux/aarch64/pt-vfork.c>
