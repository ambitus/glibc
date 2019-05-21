/* Signal Interface Routine for z/OS.
   Copyright (C) 2018 Rocket Software.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>  /* for __libc_message() only.  */
#include <string.h>
#include <signal.h>
#include <sigsetops.h>
#include <ucontext.h>
#include <atomic.h>
#include <abort-instr.h>
#include <sir.h>
#include <zos-core.h>
#include <zos-syscall-base.h>

/* TODO: we have many questions about what we need to do here:
     * Is the SIR responsible for blocking/ignoring signals?
       * Is it run for literally every single non-lost signal the
         program gets, and is charged with discriminating between which
	 ones should be made pending?
     * When can the SIR get invoked recursively?
     * Isn't there always a race between the time the SIR is given
       control entry and the time it finishes copying the PPSD?
     * Can signals be delivered when inside of code that is in a
       different addressing mode? What PPSD format would I get then?
       What addressing mode would the SIR be in in that case?
     * How to force dump creation on signal-based termination (like
       linux)? Maybe just raise a SIGABRT?
     * What is a superkill?
     * SA_NOCLDSTOP, SA_ONSTACK, and SA_RESTART
   linux research TODO:
     * What is return code for process terminated by signal?
     * does linux support the extra SIGFPE argument?
     * how is the signal mask restored after return or exit from a
       signal handler on arches that don't use sigreturn?
       * what about on arches that use sigreturn?
     * how does longjmp work with the above problem?
       * I think longjmp restores sigmask.
   My TODO:
     * if signal handler exits via longjmp, longjmp restores sigmask
   My decisions to make:
   notes:
       STORAGE is effectively atomic. Cannot be interrupted by signals.
*/


/* TODO: There are a lot of flags here we don't know much about.
   Either have to figure it out ourselves or ask IBM.  */
/* TODO: remove the asserts in this file once this has been tested.  */
/* TODO: Allocating a stack for each signal probably isn't viable
   performance-wise, write a bench test to determine how bad it can get
   in the worst case.  */
/* TODO: Maybe change sigset_t to be real.  */
/* TODO: Probably a bad idea to try to remap signal numbers.  */
/* TODO: What are we going to do about sigaltstack?  */
/* TODO: We will always recieve control in AMODE 64, right?  */
/* TODO: port the context functions.  */
/* TODO: Do we need to do the longjmp free stack stuff for
   swapcontext?  */
/* TODO: the longjmp() part of things is currently half done.  */


/* Bits for testing the ppsd.  */

/* For flags1  */
#define QUIESCEFREEZE 0x80000000
#define SIRCOMPLETE 0x40000000
#define PROCDFT   0x20000000
#define SIGQUEUE  0x10000000
#define REDRIVE   0x08000000
#define JUMPBACK  0x04000000
#define MASKONLY  0x02000000
#define THSTOP 0x01000000

/* For flags2  */
#define QUIESCEANDGET 0x00800000
#define F2_64     0x00400000

/* For flags3  */
#define ASYNC     0x00008000
#define DUMP      0x00004000
#define PTHREADKILL 0x00002000
#define THISTHREADGEN 0x00001000
#define SIGNAL    0x00000800
#define CANCEL    0x00000400
#define QUIESCE   0x00000200
#define IPT       0x00000100

/* For flags4  */
#define NOCLDSTOP 0x00000080
#define OLDSTYLE  0x00000040
#define ONSTACK   0x00000020
#define RESETHAND 0x00000010
#define RESTART   0x00000008
#define SIGINF    0x00000004
#define NOCLDWAIT 0x00000002
#define NODEFER   0x00000001

/* Parameter list for 64-bit CSRL16J.  */
struct l16j1_params
{
  int version;			/* MUST be 1.  */
  int len;			/* Size of this structure.  */
  int subpool;			/* Subpool of storage to free.  */

  unsigned int __reserved1[16];	/* Must be zeros.  */
  unsigned int ars[16];		/* Access registers.  */
  unsigned char __reserved2[8];	/* Must be zeros.  */

  char flags;			/* Set to 0x80 for us.  */
  char __reserved3[3];		/* Must be zeros.  */

  unsigned int addr_to_free;	/* pointer to area to free. Zero if
				   not freeing.  */
  int free_area_len;		/* Length of area to free or zero.
				   Signed.  */

  unsigned long gprs[16];	/* 64-bit gprs.  */
  struct {			/* Target psw.  */
    unsigned long int mask;
    unsigned long int addr;
  } target_psw;
};

/* Regardless of how we define the C structure above, we need to set
   its length field to 304.  */
#define L16J1_SIZE 304

_Static_assert (L16J1_SIZE == sizeof (struct l16j1_params), "");


/* TODO: It looks like CSRL16J takes as an argument a pointer to an
   area to put a return code, however we never actually need to use
   it. We don't need that information right now. It's unclear if
   passing in NULL is valid. We probably shouldn't make it a stack
   variable, the stack might not be valid by the time it tries to do
   the store. Remove this if it turns out that NULL works.  */
static int32_t csrl16j_dummy_retval;

volatile struct sir_data __sir_data =
  {
    .run_counter = 0,
    .bad_runs = 0,
    .cached_stack = NULL,
    .cached_stack_in_use = 0,
    .new_stack_size = 1024 * 1024
  };

__thread struct sig_tdata __sig_tdata =
  {
    .in_sighandler = false,
    .is_not_cached_stack = false,
    .stack_start = NULL
  };

libc_hidden_data_def (__sir_data)
libc_hidden_data_def (__sig_tdata)

/* The fixed PSA bit that indicates whether vector instructions are
   available.  */

#define PSA_VEC_AVAIL()					\
  ((*(char *) (volatile uintptr_t) 0x58C) & 0x2)

/* If the condition is false, kill the program without calling
   abort().  */
#define __SIRSTRSTR(parm) #parm
#define __SIRSTR(parm) __SIRSTRSTR (parm)
#define sir_assert(condition)					\
  do {								\
    if (!(condition))						\
      __libc_fatal ("A problem occured in the "			\
		    "Signal Interface Routine ("		\
		    __SIRSTR (__FILE__) ":"			\
		    __SIRSTR (__LINE__) ")\n");			\
  } while (0)
#define sir_warn(condition)					\
  do {								\
    if (!(condition))						\
      __libc_message (do_message, "The assumption \""		\
		      #condition "\" in the "			\
		      "Signal Interface Routine "		\
		      "was not true ("				\
		      __SIRSTR (__FILE__) ":"			\
		      __SIRSTR (__LINE__) ")\n");		\
  } while (0)


static void
copy_gregs_from_ppsd (unsigned long dst[16],
		      const struct sigcontext *ppsd)
{
  int i;
  /* Copy the GPRs.  */
  for (i = 0; i < 16; ++i)
    {
      /* Copy the (zero-extended) GPR low halves.  */
      dst[i] = (unsigned long) ppsd->gregs_low[i];

      /* Copy the GPR high halves.  */
      dst[i] |= (unsigned long) ppsd->gregs_high[i] << 32;
    }
}

/* Restore registers to their pre-interrupt states while simultaniously
   jumping back to the original PC, and optionally deallocating the
   signal stack.

   The operating system provides a service that we can use to do all of
   that atomically, which means we don't need a sigreturn syscall
   equivalent or a signal trampoline.  */

static void
__attribute__ ((noreturn))
mvs_load_jump_and_deallocate (struct l16j1_params *params)
{
  /* The documentation suggests that csrl16j only works in amode 31,
     however it also documents a 64-bit parameter list that is
     available.  */
  uint32_t csrl16j_call_ptr;
  long r1_save_slot = 0;

  struct {
    unsigned int arg_area_addr;
    unsigned int retval_addr;
  } arglist;

  sir_assert ((uintptr_t) params < PTR31_BAR);
  sir_assert ((uintptr_t) &csrl16j_dummy_retval < PTR31_BAR);
  sir_assert ((uintptr_t) &r1_save_slot < PTR31_BAR);

  arglist.arg_area_addr = (unsigned int) (uintptr_t) params;
  arglist.retval_addr = (unsigned int) (uintptr_t) &csrl16j_dummy_retval;

  /* Call the CSRL16J service. It's not technically a bpxk call, but
     the wrapper that we use for those should work for this just as
     well.  */
  csrl16j_call_ptr =
    (uint32_t) GET_PTR31_UNSAFE (GET_PTR31_SAFE (CSRT_PTR + 32) + 0);

  /* The following is a little bit tricky since we need to go into
     AMODE 31. Furthermore, CSRL16J exepects a 72-byte 31-bit
     caller-provided save area. We need the explicit register for
     r1_save_slot_ptr to guaruantee that we do no access it through %r1,
     and routine_addr MUST be in r15.  */
  register void *r1_save_slot_ptr asm ("r3") = &r1_save_slot;
  register int routine_addr asm ("r15") = csrl16j_call_ptr;
  __asm__ __volatile__ ("stg	%%r1, 0(%1)\n\t"
			"mvc	136(8,%%r13), 144(%%r13)\n\t"
			/* TODO: 72-byte save area.  */
			"sam31\n\t"
			"lgr	%%r1, %2\n\t"
			"basr	%%r14, %3\n\t"
			"sam64\n\t"
			"lg	%%r1, 0(%1)"
			: "=m" (r1_save_slot)
			: "r" (r1_save_slot_ptr), "r" (&arglist),
			  "r" (routine_addr)
			: "r14", "memory");

  /* We only return if something has gone wrong.  */
  sir_assert (false);
}


static void
populate_l16j1_params (struct l16j1_params *params,
		       const struct sir_stack *ss, bool should_free)
{
  memset (params, 0, sizeof (*params));

  params->version = 1;
  params->len = L16J1_SIZE;

  memcpy (params->ars, ss->ppsd.ars, sizeof (params->ars));

  /* Set flag to ensure processing of ARs.  */
  params->flags = 0x80;

  if (should_free)
    {
      params->subpool = STORAGE_SUBPOOL;
      params->addr_to_free = (unsigned int) (uintptr_t) ss;
      params->free_area_len = (int) ss->length;
      sir_assert (ss->length < PTR31_BAR);
    }
  else
    {
      params->subpool = 0;
      params->addr_to_free = 0;
      params->free_area_len = 0;
    }

  copy_gregs_from_ppsd (params->gprs, &ss->ppsd);
  params->target_psw.mask = ss->ppsd.psw.mask;
  params->target_psw.addr = ss->ppsd.psw.addr;
}


static void
populate_siginfo_for_sig (siginfo_t *info, const struct sigcontext *ppsd)
{
  info->si_signo = kern_to_user_signo (ppsd->sig_si_signo);
  info->si_errno = ppsd->sig_si_errno;
  info->si_code = ppsd->sig_si_code;
  /* TODO: Check if the kernel actually populates si_code for all
     of these cases.  */

  switch (info->si_signo)
    {
    case SIGCHLD:
      info->si_pid = ppsd->sig_si_pid;
      info->si_uid = ppsd->sig_si_uid;
      info->si_status = ppsd->sig_si_status;
      info->si_utime = 0;  /* TODO: Something about this.  */
      info->si_stime = 0;  /* TODO: Something about this.  */
      break;

    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
      info->si_addr = ppsd->sig_si_addr;
      info->si_addr_lsb = (short) (uintptr_t) ppsd->sig_si_addr & 0x1;
      /* TODO: something about _bounds and _sigfault.  */
      break;

    case SIGPOLL:
      info->si_band = ppsd->sig_si_band;
      info->si_fd = 0;  /* TODO: can we populate this?  */
      break;

    case SIGTRAP:
      break;

    case SIGSYS:
      /* TODO: Everything about SIGSYS. Does kernel set si_code, do
	 we get extra information?  */
      memset (&info->_sifields._sigsys, 0,
	      sizeof (info->_sifields._sigsys));
      break;

    case SIGALRM:
      /* TODO: Can we populate the required info for SIGALRM?  */
      memset (&info->_sifields._rt, 0, sizeof (info->_sifields._rt));
      break;

    /* TODO: Check if signal was sent from kill().  */
    default:
      info->si_pid = ppsd->sig_si_pid;
      info->si_uid = ppsd->sig_si_uid;
      /* TODO: What is this field, what belongs here?  */
      memset (&info->si_value, 0, sizeof (info->si_value));
      break;
    }
  /* TODO: in the kill() wrapper, set correct si_code.  */
}


static void
populate_ucontext_for_sig (ucontext_t *ucont,
			   const struct sigcontext *ppsd)
{
  ucont->uc_mcontext.psw.mask = ppsd->psw.mask;
  ucont->uc_mcontext.psw.addr = ppsd->psw.addr;

  copy_gregs_from_ppsd (ucont->uc_mcontext.gregs, ppsd);
  memcpy (ucont->uc_mcontext.aregs, ppsd->ars, sizeof (ppsd->ars));

  /* vrregs and fpregs have already been saved.  */

  /* TODO: Finish ucontext SA_SIGINFO arg handling.  */
}

/* Since the sigmasks need to be declared as char arrays in the PPSD
   struct for alignment purposes, manually reassemble them.  */

static inline uint64_t
mask_from_array (char in[8])
{
  return ((uint64_t) in[0] << 56
	  | (uint64_t) in[1] << 48
	  | (uint64_t) in[2] << 40
	  | (uint64_t) in[3] << 32
	  | (uint64_t) in[4] << 24
	  | (uint64_t) in[5] << 16
	  | (uint64_t) in[6] << 8
	  | (uint64_t) in[7]);
}

/* Do everything we need to do in order to handle a regular old
   signal.  */
/* z/OS TODO: override pthread_sigmask to just use sigprocmask.  */

static void
handle_signals (struct sigcontext *ppsd, int flags, ucontext_t *ucont)
{
  sir_assert (flags & SIGNAL);

  /* TODO: Check that all signals are blocked here.  */

  uint64_t ret_sigmask = mask_from_array (ppsd->sigmask_on_return);
  void *sighandler = ppsd->handler;
  int signum = ppsd->sig_si_signo;
  sir_assert (signum < 64 && signum > 0);
  signum = kern_to_user_signo (signum);

  sigset_t handler_sigmask;
  uint64_t kmask =
    ret_sigmask | mask_from_array (ppsd->sigaction_sigmask);
  kern_to_user_sigset (&handler_sigmask, kmask);
  if (!(flags & NODEFER))
    __sigaddset (&handler_sigmask, signum);

  /* Check if this is a regular 1 (actually 2) argument signal handler,
     or a 3-argument SA_SIGINFO signal handler.  */
  if (flags & SIGINF)
    {
      /* 3-arg SA_SIGINFO handler.  */
      siginfo_t info;
      populate_siginfo_for_sig (&info, ppsd);
      populate_ucontext_for_sig (ucont, ppsd);

      /* Set appropriate signal mask for the handler.  */
      __sigprocmask (SIG_SETMASK, &handler_sigmask, NULL);

      /* Call the signal handler.  */
      ((void (*)(int, siginfo_t *, void *)) sighandler) (signum, &info,
							 ucont);
    }
  else
    {
      /* Regular 1-arg handler (with second secret sigcontext arg).  */
      /* Set appropriate signal mask for the handler.  */
      __sigprocmask (SIG_SETMASK, &handler_sigmask, NULL);

      /* Call the signal handler.  */
      ((void (*)(int, struct sigcontext *)) sighandler) (signum, ppsd);
    }

  /* Don't touch the ppsd we gave to the user after this point, the user
     might have modified it.  */

  /* Restore the signal mask.  */
  sigset_t ret_set;
  kern_to_user_sigset (&ret_set, ret_sigmask);
  __sigprocmask (SIG_SETMASK, &ret_set, NULL);
}

/* Handle asyncronous thread cancellation interrupts, which are similar
   to but distinct from regular signals.  */

static void
handle_cancellation (struct sigcontext *ppsd, int flags)
{
  sir_assert (flags & CANCEL);
  /* TODO: Async cancellation.  */
}

/* Handle thread quiesce interrupts, which are similar to but distinct
   from regular signals.  */

static void
handle_quiesce (struct sigcontext *ppsd, int flags)
{
  sir_assert (flags & QUIESCE);

  /* We were either issued a freeze request or a termination request.  */
  if (flags & QUIESCEFREEZE)
    {
      /* TODO: how do we freeze a thread?  */
    }
  else  /* Terminate.  */
    {
      /* TODO: End the thread.  */
    }
}

/* Save all current floating point registers and vector registers into
   the vregset given.  */

static void
save_fprs_and_vrs (mcontext_t *mcont)
{
  /* z/OS TODO: Should I be more exact with the constraint inputs?  */
  /* Store the fp control register.  */
  __asm__ __volatile__ ("stfpc	%0"
			: "=Q" (mcont->fpregs.fpc)
			:: "memory");

  /* Check if the machine has vector registers enabled.  */
  if (PSA_VEC_AVAIL ())
    {
      /* Since the FPRs are part of some of the vrs, we don't need
	 to save them separately.  */
      __asm__ __volatile__ (".machine push\n\t"
			    ".machine \"z13\"\n\t"
			    "vstm	%%v0,  %%v15, %0\n\t"
			    "vstm	%%v16, %%v31, 256 + %0\n\t"
			    ".machine pop"
			    : "=Q" (mcont->vregs.vrs[0].vr)
			    :: "memory");
    }
  __asm__ __volatile__ ("std	%%f0, %1 * 0(%0)\n\t"
			"std	%%f1, %1 * 1(%0)\n\t"
			"std	%%f2, %1 * 2(%0)\n\t"
			"std	%%f3, %1 * 3(%0)\n\t"
			"std	%%f4, %1 * 4(%0)\n\t"
			"std	%%f5, %1 * 5(%0)\n\t"
			"std	%%f6, %1 * 6(%0)\n\t"
			"std	%%f7, %1 * 7(%0)\n\t"
			"std	%%f8, %1 * 8(%0)\n\t"
			"std	%%f9, %1 * 9(%0)\n\t"
			"std	%%f10, %1 * 10(%0)\n\t"
			"std	%%f11, %1 * 11(%0)\n\t"
			"std	%%f12, %1 * 12(%0)\n\t"
			"std	%%f13, %1 * 13(%0)\n\t"
			"std	%%f14, %1 * 14(%0)\n\t"
			"std	%%f15, %1 * 15(%0)"
			:: "r" (&mcont->fpregs.fprs[0].d),
			 "I" ((uintptr_t) &mcont->fpregs.fprs[1].d -
			      (uintptr_t) &mcont->fpregs.fprs[0].d)
			: "memory");
  /* TODO: check the PSA bit in setjmp.S and longjmp().  */
}

/* Load all floating point registers and vector registers from the
   vregset given.  */

static void
load_fprs_and_vrs (mcontext_t *mcont)
{
  /* We intentionally don't mark anything as clobbered since we don't
     want the compiler to restore these regs after we load them.  */

  /* Load the fp control register.  */
  __asm__ __volatile__ ("lfpc	%0" :: "Q" (mcont->fpregs.fpc));

  /* Check if the machine has vector registers enabled.  */
  if (PSA_VEC_AVAIL ())
    {
      __asm__ __volatile__ (".machine push\n\t"
			    ".machine \"z13\"\n\t"
			    "vlm	%%v0,  %%v15, %0\n\t"
			    "vlm	%%v16, %%v31, 256 + %0\n\t"
			    ".machine pop"
			    :: "Q" (mcont->vregs.vrs[0].vr));
    }
  __asm__ __volatile__ ("ld	%%f0, %1 * 0(%0)\n\t"
			"ld	%%f1, %1 * 1(%0)\n\t"
			"ld	%%f2, %1 * 2(%0)\n\t"
			"ld	%%f3, %1 * 3(%0)\n\t"
			"ld	%%f4, %1 * 4(%0)\n\t"
			"ld	%%f5, %1 * 5(%0)\n\t"
			"ld	%%f6, %1 * 6(%0)\n\t"
			"ld	%%f7, %1 * 7(%0)\n\t"
			"ld	%%f8, %1 * 8(%0)\n\t"
			"ld	%%f9, %1 * 9(%0)\n\t"
			"ld	%%f10, %1 * 10(%0)\n\t"
			"ld	%%f11, %1 * 11(%0)\n\t"
			"ld	%%f12, %1 * 12(%0)\n\t"
			"ld	%%f13, %1 * 13(%0)\n\t"
			"ld	%%f14, %1 * 14(%0)\n\t"
			"ld	%%f15, %1 * 15(%0)"
			:: "r" (&mcont->fpregs.fprs[0].d),
			 "I" ((uintptr_t) &mcont->fpregs.fprs[1].d  -
			      (uintptr_t) &mcont->fpregs.fprs[0].d));
  /* TODO: check the PSA bit in setjmp.S and longjmp().  */
  /* TODO: check if we save fpc in setjmp.  */
}

/* The main part of the SIR.  */

void
__attribute__ ((noreturn))
__sir_body (struct sir_stack *ss, int should_free)
{
  /* Make sure we got through the early init okay.  */
  sir_assert (ss && (should_free == 0 || should_free == 1));
  bool freeing = should_free ? true : false;

  /* TODO: What about quiesce exits.  */

  atomic_store_release (&__sig_tdata.in_sighandler, true);
  __sig_tdata.is_not_cached_stack = freeing;
  __sig_tdata.stack_start = ss;

  /* TODO: Right now, I'm assuming that the floating point state that
     we recieve control in is the the one that was interrupted. Test
     this assumption.  */

  /* Save the FP state, regs, and vector regs.
     This ucontext is the same one we might pass to the signal handler,
     we initialize it peicemeal as we go.
     z/OS TODO: we could save 200-odd bytes of stack space on almost all
     signal handlers by only declaring the vrregset and fpregset
     here.  */
  ucontext_t ucont = {0};
  save_fprs_and_vrs (&ucont.uc_mcontext);
  /* ---- NO FLOATING POINT OPERATIONS ABOVE HERE ---- */

  struct l16j1_params params;
  struct sigcontext *ppsd = &ss->ppsd;

  /* Construct the argument list for CSRL16J, which we will call
     later.  */
  populate_l16j1_params (&params, ss, freeing);

  /* It's easier to manage all the flag bytes if we combine them.  */
  int flags = (int) ppsd->flags1 << 24
    | (int) ppsd->flags2 << 16
    | (int) ppsd->flags3 << 8
    | (int) ppsd->flags4;

  /* TODO: Start of testing section. Remove later.  */
  sir_warn (flags & F2_64);
  if (flags & IPT)
    sir_warn (__get_zos_tcb_addr () == __ipt_zos_tcb);
  /* TODO: End of testing section. Remove later.  */

  /* Determine reason for interruption.  */
  switch (flags & (SIGNAL | CANCEL | QUIESCE))
    {
    case SIGNAL:
      handle_signals (ppsd, flags, &ucont);
      break;
    case CANCEL:
      handle_cancellation (ppsd, flags);
      break;
    case QUIESCE:
      handle_quiesce (ppsd, flags);
      break;
    default:
      sir_assert (false);
      break;
    }

  load_fprs_and_vrs (&ucont.uc_mcontext);
  /* ---- NO FLOATING POINT OPERATIONS BELOW HERE ---- */

  /* Final cleanup.  */

  atomic_store_release (&__sig_tdata.in_sighandler, false);

  if (!freeing)
    atomic_store_release (&__sir_data.cached_stack_in_use, -1);

  /* Do an MVS load 16 and return. This also deallocates the stack if
     we indicated we wanted to do so when we populated the args.  */
  mvs_load_jump_and_deallocate (&params);
  /* ---- WE MUST NOT TOUCH THE STACK AFTER THIS POINT. ----  */
}
