/* siginfo constants.  Linux version.
   Copyright (C) 1997-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_SIGINFO_CONSTS_H
#define _BITS_SIGINFO_CONSTS_H 1

/* z/OS TODO: Review this file.  */

#ifndef _SIGNAL_H
#error "Don't include <bits/siginfo-consts.h> directly; use <signal.h> instead."
#endif

#ifndef __SI_ASYNCIO_AFTER_SIGIO
# define __SI_ASYNCIO_AFTER_SIGIO 1
#endif

/* Values for `si_code'.  The only reliable way to differentiate between
   user and kernel set si_codes is by checking SIG_KERNSICODE.  */
enum
{
  /* z/OS TODO: should we actually define most of these?  */
  SI_ASYNCNL = -60,		/* Sent by asynch name lookup completion.  */
  SI_TKILL = -6,		/* Sent by tkill.  */
  SI_SIGIO,			/* Sent by queued SIGIO. */
  SI_MESGQ,			/* Sent by real time mesq state change.  */
  SI_TIMER,			/* Sent by timer expiration.  */
  SI_USER,			/* Sent by kill, sigsend.  */
  SI_KERNEL = 0x80,		/* Send by kernel.  */

  /* These are only values that the z/OS kernel knows.  */
  SI_ASYNCIO = 175,		/* Sent by AIO completion.  */
  SI_QUEUE = 176		/* Sent by sigqueue.  */

#define SI_ASYNCNL	SI_ASYNCNL
#define SI_TKILL	SI_TKILL
#define SI_SIGIO	SI_SIGIO
#define SI_ASYNCIO	SI_ASYNCIO
#define SI_MESGQ	SI_MESGQ
#define SI_TIMER	SI_TIMER
#define SI_ASYNCIO	SI_ASYNCIO
#define SI_QUEUE	SI_QUEUE
#define SI_USER		SI_USER
#define SI_KERNEL	SI_KERNEL
};


# if defined __USE_XOPEN_EXTENDED || defined __USE_XOPEN2K8
/* z/OS TODO: what should we do with these?  */
/* `si_code' values for SIGILL signal.  */
enum
{
  ILL_ILLOPC = 11,		/* Illegal opcode.  */
#  define ILL_ILLOPC	ILL_ILLOPC
  ILL_ILLOPN,			/* Illegal operand.  */
#  define ILL_ILLOPN	ILL_ILLOPN
  ILL_ILLADR,			/* Illegal addressing mode.  */
#  define ILL_ILLADR	ILL_ILLADR
  ILL_ILLTRP,			/* Illegal trap. */
#  define ILL_ILLTRP	ILL_ILLTRP
  ILL_PRVOPC,			/* Privileged opcode.  */
#  define ILL_PRVOPC	ILL_PRVOPC
  ILL_PRVREG,			/* Privileged register.  */
#  define ILL_PRVREG	ILL_PRVREG
  ILL_COPROC,			/* Coprocessor error.  */
#  define ILL_COPROC	ILL_COPROC
  ILL_BADSTK,			/* Internal stack error.  */
#  define ILL_BADSTK	ILL_BADSTK
  ILL_EXECUTE,			/* Execute exception indicated.  */
#  define ILL_EXECUTE	ILL_EXECUTE
  ILL_ILLSPEC			/* Specification exception indicated.  */
#  define ILL_ILLSPEC	ILL_ILLSPEC
};

/* `si_code' values for SIGFPE signal.  */
enum
{
  FPE_INTDIV = 31,		/* Integer divide by zero.  */
#  define FPE_INTDIV	FPE_INTDIV
  FPE_INTOVF,			/* Integer overflow.  */
#  define FPE_INTOVF	FPE_INTOVF
  FPE_FLTDIV,			/* Floating point divide by zero.  */
#  define FPE_FLTDIV	FPE_FLTDIV
  FPE_FLTOVF,			/* Floating point overflow.  */
#  define FPE_FLTOVF	FPE_FLTOVF
  FPE_FLTUND,			/* Floating point underflow.  */
#  define FPE_FLTUND	FPE_FLTUND
  FPE_FLTRES,			/* Floating point inexact result.  */
#  define FPE_FLTRES	FPE_FLTRES
  FPE_FLTINV,			/* Floating point invalid operation.  */
#  define FPE_FLTINV	FPE_FLTINV
  FPE_FLTSUB,			/* Subscript out of range.  */
#  define FPE_FLTSUB	FPE_FLTSUB
  FPE_FLTSIG,			/* FPE is a floating point
				   significance exception.  */
#  define FPE_FLTSIG	FPE_FLTSIG
  FPE_DECDATA,			/* FPE is a decimal data exception.  */
#  define FPE_DECDATA	FPE_DECDATA
  FPE_DECDIV,			/* FPE is a decimal divide by zero
				   exception.  */
#  define FPE_DECDIV	FPE_DECDIV
  FPE_DECOVF,			/* FPE is a decimal overflow
				   exception.  */
#  define FPE_DECOVF	FPE_DECOVF
  FPE_UNKWN			/* Exception of undetermined nature.  */
#  define FPE_UNKWN	FPE_UNKWN
};

/* `si_code' values for SIGSEGV signal.  */
enum
{
  SEGV_MAPERR = 51,		/* Address not mapped to object.  */
#  define SEGV_MAPERR	SEGV_MAPERR
  SEGV_ACCERR,			/* Invalid permissions for mapped object.  */
#  define SEGV_ACCERR	SEGV_ACCERR
  SEGV_PROTECT,			/* Protection key checking failure.  */
#  define SEGV_PROTECT	SEGV_PROTECT
#  define SEGV_PKUERR	SEGV_PROTECT
  SEGV_ADDRESS,			/* Address is invalid.  */
#  define SEGV_ADDRESS	SEGV_ADDRESS
};

/* `si_code' values for SIGBUS signal.  */
enum
{
  BUS_ADRALN = 71,		/* Invalid address alignment.  */
#  define BUS_ADRALN	BUS_ADRALN
  BUS_ADRERR,			/* Non-existant physical address.  */
#  define BUS_ADRERR	BUS_ADRERR
  BUS_OBJERR,			/* Object specific hardware error.  */
#  define BUS_OBJERR	BUS_OBJERR
};
# endif

# ifdef __USE_XOPEN_EXTENDED
/* `si_code' values for SIGTRAP signal.  */
enum
  {
   TRAP_BRKPT = 91,		/* Process breakpoint.  */
#  define TRAP_BRKPT	TRAP_BRKPT
   TRAP_TRACE			/* Process trace trap.  */
#  define TRAP_TRACE	TRAP_TRACE
  };
# endif

# if defined __USE_XOPEN_EXTENDED || defined __USE_XOPEN2K8
/* `si_code' values for SIGCHLD signal.  */
enum
{
  CLD_EXITED = 101,		/* Child has exited.  */
#  define CLD_EXITED	CLD_EXITED
  CLD_KILLED,			/* Child was killed.  */
#  define CLD_KILLED	CLD_KILLED
  CLD_DUMPED,			/* Child terminated abnormally.  */
#  define CLD_DUMPED	CLD_DUMPED
  CLD_TRAPPED,			/* Traced child has trapped.  */
#  define CLD_TRAPPED	CLD_TRAPPED
  CLD_STOPPED,			/* Child has stopped.  */
#  define CLD_STOPPED	CLD_STOPPED
  CLD_CONTINUED			/* Stopped child has continued.  */
#  define CLD_CONTINUED	CLD_CONTINUED
};

/* `si_code' values for SIGPOLL signal.  */
enum
{
  POLL_IN = 111,		/* Data input available.  */
#  define POLL_IN	POLL_IN
  POLL_OUT,			/* Output buffers available.  */
#  define POLL_OUT	POLL_OUT
  POLL_MSG,			/* Input message available.   */
#  define POLL_MSG	POLL_MSG
  POLL_ERR,			/* I/O error.  */
#  define POLL_ERR	POLL_ERR
  POLL_PRI,			/* High priority input available.  */
#  define POLL_PRI	POLL_PRI
  POLL_HUP			/* Device disconnected.  */
#  define POLL_HUP	POLL_HUP
};
# endif
#endif
