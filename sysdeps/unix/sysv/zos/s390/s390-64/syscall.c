#include <stdio.h>  /* for __libc_fatal */
#include <stdarg.h>
#include <sysdep.h>
#include <asm/unistd.h>
#include <errno.h>

long int
syscall (long int sysno, ...)
{
  __libc_fatal("not yet implemented");
}
