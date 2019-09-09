/* Signal number definitions.  z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
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

#ifndef _BITS_SIGNUM_H
#define _BITS_SIGNUM_H 1

#ifndef _SIGNAL_H
#error "Never include <bits/signum.h> directly; use <signal.h> instead."
#endif

#include <bits/signum-generic.h>

/* These signal numbers are slightly disingenuous. 3 and 6 should be
   SIGABRT and SIGURG respectively, and I just made up 40 for SIGURG
   out of whole cloth. We fix these up in the syscall wrappers.
   z/OS TODO: Is this a smart approach? It might be better to just
   bite the bullet.  */

#define SIGABND		18
/* z/OS TODO: DANGER: SIGIO and SIGPOLL are different signals on z/OS,
   What is the significance of this, what do we need to do to address
   it? Which one is closer to the regular SIGIO/SIGPOLL in meaning?  */
#undef SIGIO
#define SIGIO		23
#define SIGIOERR	27
#define SIGDANGER	33
#define SIGTHSTOP	34
#define SIGTHCONT	35
#define SIGTRACE	37
#define SIGDUMP		39

#undef SIGPOLL
#define	SIGPOLL		5
#undef SIGSTOP
#define SIGSTOP		7
#undef SIGUSR1
#define SIGUSR1		16
#undef SIGUSR2
#define SIGUSR2		17
#undef SIGTSTP
#define SIGTSTP		25
#undef SIGTRAP
#define SIGTRAP		26
#undef SIGXCPU
#define SIGXCPU		29
#undef SIGXFSZ
#define SIGXFSZ		30
#undef SIGVTALRM
#define SIGVTALRM	31
#undef SIGPROF
#define SIGPROF		32
#undef SIGURG
#define SIGURG		40

#undef __SIGRTMIN
#define __SIGRTMIN	41
#undef __SIGRTMAX
#define __SIGRTMAX	__SIGRTMIN

#endif /* !_BITS_SIGNUM_H  */
