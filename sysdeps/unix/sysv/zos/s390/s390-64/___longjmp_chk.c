/* z/OS TODO: Implement ____longjmp_chk. Right now, it doesn't check
   anything. ___longjmp_chk is used instead of longjmp when
   -DFORTIFY_SOURCE=2 is used.  */
#define __longjmp ____longjmp_chk
#include "__longjmp.c"
