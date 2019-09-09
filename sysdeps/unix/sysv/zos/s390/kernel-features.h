/* Set flags signalling availability of kernel features based on given
   kernel version number.  z/OS version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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

/* TODO: This is very rough, go over this whole file again.  */

#include_next <kernel-features.h>

#undef __ASSUME_SOCKETCALL

/* The statfs64 syscalls are available in 2.5.74 (but not for alpha).  */
#undef __ASSUME_STATFS64

/* pselect/ppoll were introduced just after 2.6.16-rc1.  On x86_64 and
   SH this appeared first in 2.6.19-rc1, on ia64 in 2.6.22-rc1.  */
#undef __ASSUME_PSELECT

/* The *at syscalls were introduced just after 2.6.16-rc1.  On PPC
   they were introduced in 2.6.17-rc1, on SH in 2.6.19-rc1.  */
#undef __ASSUME_ATFCTS

/* Support for inter-process robust mutexes was added in 2.6.17 (but
   some architectures lack futex_atomic_cmpxchg_inatomic in some
   configurations).  */
#undef __ASSUME_SET_ROBUST_LIST

/* Support for various CLOEXEC and NONBLOCK flags was added in
   2.6.27.  */
#undef __ASSUME_IN_NONBLOCK

/* Support for the FUTEX_CLOCK_REALTIME flag was added in 2.6.29.  */
#undef __ASSUME_FUTEX_CLOCK_REALTIME

/* Support for preadv and pwritev was added in 2.6.30.  */
#undef __ASSUME_PREADV
#undef __ASSUME_PWRITEV

/* Support for sendmmsg functionality was added in 3.0.  */
#undef __ASSUME_SENDMMSG

/* On most architectures, most socket syscalls are supported for all
   supported kernel versions, but on some socketcall architectures
   separate syscalls were only added later.  */
#define __ASSUME_SENDMSG_SYSCALL	1
#define __ASSUME_RECVMSG_SYSCALL	1
#define __ASSUME_ACCEPT_SYSCALL		1
#define __ASSUME_CONNECT_SYSCALL	1
#define __ASSUME_RECVFROM_SYSCALL	1
#define __ASSUME_SENDTO_SYSCALL		1
/* #define __ASSUME_ACCEPT4_SYSCALL	1  */
#define __ASSUME_RECVMMSG_SYSCALL	1
#define __ASSUME_SENDMMSG_SYSCALL	1

/* Support for SysV IPC through wired syscalls.  All supported architectures
   either support ipc syscall and/or all the ipc correspondent syscalls.  */
#define __ASSUME_DIRECT_SYSVIPC_SYSCALLS	1

/* Support for p{read,write}v2 was added in 4.6.  However Linux default
   implementation does not assume the __ASSUME_* and instead use a fallback
   implementation based on p{read,write}v and returning an error for
   non supported flags.  */

/* Support for the renameat2 system call was added in kernel 3.15.  */
#undef __ASSUME_RENAMEAT2

/* Support for the execveat syscall was added in 3.19.  */
#undef __ASSUME_EXECVEAT

#undef __ASSUME_MLOCK2

#undef __ASSUME_COPY_FILE_RANGE

/* Support for statx was added in kernel 4.11.  */
#undef __ASSUME_STATX
