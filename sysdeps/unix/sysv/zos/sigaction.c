/* z/OS TODO: go back to leaning on the generic nptl version like the
   generic linux version does, after we override internal-signals.h and
   remove any uses of SIGCANCEL and SIGSETXID.  */

#include <signal.h>
#include <sysdep.h>
#include <internal-signals.h>

/* If ACT is not NULL, change the action for SIG to *ACT.
   If OACT is not NULL, put the old action for SIG in *OACT.  */
int
__libc_sigaction (int sig, const struct sigaction *act,
		  struct sigaction *oact)
{
  return INLINE_SYSCALL_CALL (sigaction, sig, act, oact);
}

int
__sigaction (int sig, const struct sigaction *act,
	     struct sigaction *oact)
{
  if (sig <= 0 || sig >= 64)
    {
      __set_errno (EINVAL);
      return -1;
    }

  return __libc_sigaction (sig, act, oact);
}
libc_hidden_weak (__sigaction)
weak_alias (__sigaction, sigaction)
