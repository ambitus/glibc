/* z/OS TODO: IMPORTANT, we don't handle cancellable system() correctly
   yet (see the linux impl for what kind of requirements those are).
   Also, the generic implemenatation might be bugged because it doesn't
   use atomic inc/dec in ADD/SUB_REF, so ask about that.  */
#include <sysdeps/posix/system.c>
