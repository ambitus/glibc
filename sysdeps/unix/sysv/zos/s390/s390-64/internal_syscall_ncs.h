/* Definition of the INTERNAL_SYSCALL_NCS macro.
   Copyright (C) 2018 Rocket Software.
   Contributed by Giancarlo Frix (gfrix@rocketsoftware.com).
   This file is part of the GNU C Library.

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
   <http://www.gnu.org/licenses/>.

   TODO: This is only here to get glibc working, its an absolute kludge.
   This whole mechanism will need to be reworked seriously, because
   inlining anything of this size cannot be good for cache or code size.
   The only user of this macro seems to be nptl, and it probably only
   requires a very small subset of sycalls, so the size of the table
   could be reduced dramatically. Of course, we might just end up
   dummying the files/caller functions altogether.

   They seem to only be used to support the nptl SIGSETXID signal
   system, so only the _id syscalls need to be implemented.  */

# define INTERNAL_SYSCALL_NCS(no, err, nr, args...)		\
  ({								\
    long _ret;							\
    switch (no)							\
    {								\
    case 20: _ret = INTERNAL_SYSCALL (getpid, err, nr, args); break;  \
    case 57: _ret = INTERNAL_SYSCALL (setpgid, err, nr, args); break; \
    case 64: _ret = INTERNAL_SYSCALL (getppid, err, nr, args); break;	\
    case 66: _ret = INTERNAL_SYSCALL (setsid, err, nr, args); break;	\
    case 112: _ret = INTERNAL_SYSCALL (idle, err, nr, args); break;	\
    case 132: _ret = INTERNAL_SYSCALL (getpgid, err, nr, args); break;	\
    case 147: _ret = INTERNAL_SYSCALL (getsid, err, nr, args); break;	\
    case 199: _ret = INTERNAL_SYSCALL (getuid, err, nr, args); break;	\
    case 200: _ret = INTERNAL_SYSCALL (getgid, err, nr, args); break;	\
    case 201: _ret = INTERNAL_SYSCALL (geteuid, err, nr, args); break;	\
    case 202: _ret = INTERNAL_SYSCALL (getegid, err, nr, args); break;	\
    case 203: _ret = INTERNAL_SYSCALL (setreuid, err, nr, args); break; \
    case 204: _ret = INTERNAL_SYSCALL (setregid, err, nr, args); break; \
    case 208: _ret = INTERNAL_SYSCALL (setresuid, err, nr, args); break; \
    case 209: _ret = INTERNAL_SYSCALL (getresuid, err, nr, args); break; \
    case 210: _ret = INTERNAL_SYSCALL (setresgid, err, nr, args); break; \
    case 211: _ret = INTERNAL_SYSCALL (getresgid, err, nr, args); break; \
    case 213: _ret = INTERNAL_SYSCALL (setuid, err, nr, args); break;	\
    case 214: _ret = INTERNAL_SYSCALL (setgid, err, nr, args); break;	\
    case 215: _ret = INTERNAL_SYSCALL (setfsuid, err, nr, args); break; \
    case 216: _ret = INTERNAL_SYSCALL (setfsgid, err, nr, args); break; \
    case 236: _ret = INTERNAL_SYSCALL (gettid, err, nr, args); break;	\
    case 252: _ret = INTERNAL_SYSCALL (set_tid_address, err, nr, args); break; \
    case 281: _ret = INTERNAL_SYSCALL (waitid, err, nr, args); break;	\
    default: _ret = 0xdeadbeef; break; /* control should never reach here */ \
    }									\
    _ret; })
