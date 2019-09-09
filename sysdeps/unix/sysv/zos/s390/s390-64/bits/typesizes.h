/* bits/typesizes.h -- underlying types for *_t.  z/OS version.
   Copyright (C) 2018 Rocket Software
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

#ifndef _BITS_TYPES_H
# error "Never include <bits/typesizes.h> directly; use <sys/types.h> instead."
#endif

#ifndef	_BITS_TYPESIZES_H
#define	_BITS_TYPESIZES_H	1

/* TODO: confirm that all of these are correct.
   the size of most of the corresponding members of the BPXYSTAT structure
   is fixed at 32-bits, so even if the system supports larger types for
   some of these values, we have no way of getting that info
   untruncated.  */

/* See <bits/types.h> for the meaning of these macros.  This file exists so
   that <bits/types.h> need not vary across different GNU platforms.  */

#define __DEV_T_TYPE		__U32_TYPE
#define __UID_T_TYPE		__U32_TYPE
#define __GID_T_TYPE		__U32_TYPE
#define __INO_T_TYPE		__U32_TYPE
#define __INO64_T_TYPE		__U32_TYPE
#define __MODE_T_TYPE		__U32_TYPE
#define __NLINK_T_TYPE		__U32_TYPE
/* The BPX services always use signed 64-bit values for file offsets,
   which is actually very nice.  */
#define __OFF_T_TYPE		__S64_TYPE
#define __OFF64_T_TYPE		__S64_TYPE
#define __PID_T_TYPE		__S32_TYPE
#define __RLIM_T_TYPE		__U64_TYPE
#define __RLIM64_T_TYPE		__U64_TYPE

/* TODO: make sure that the following two types should actually
   be signed. */
#define	__BLKCNT_T_TYPE		__S64_TYPE
#define	__BLKCNT64_T_TYPE	__S64_TYPE

/* TODO: FIXME: The following types haven't yet been adapted for
   z/OS.  */
#define	__FSBLKCNT_T_TYPE	__ULONGWORD_TYPE
#define	__FSBLKCNT64_T_TYPE	__UQUAD_TYPE
#define	__FSFILCNT_T_TYPE	__ULONGWORD_TYPE
#define	__FSFILCNT64_T_TYPE	__UQUAD_TYPE
#define	__FSWORD_T_TYPE		__SWORD_TYPE
#define	__ID_T_TYPE		__U32_TYPE
#define __CLOCK_T_TYPE		__SLONGWORD_TYPE
#define __TIME_T_TYPE		__SLONGWORD_TYPE
#define __USECONDS_T_TYPE	__U32_TYPE
#define __SUSECONDS_T_TYPE	__SLONGWORD_TYPE
#define __DADDR_T_TYPE		__S32_TYPE
#define __KEY_T_TYPE		__S32_TYPE
#define __CLOCKID_T_TYPE	__S32_TYPE
#define __TIMER_T_TYPE		void *
#define __BLKSIZE_T_TYPE	__SLONGWORD_TYPE
#define __FSID_T_TYPE		struct { int __val[2]; }
#if defined __GNUC__ && __GNUC__ <= 2
/* Compatibility with g++ 2.95.x.  */
#define __SSIZE_T_TYPE		__SWORD_TYPE
#else
/* size_t is unsigned long int on s390 -m31.  */
#define __SSIZE_T_TYPE		__SLONGWORD_TYPE
#endif
#define __SYSCALL_SLONG_TYPE	__SLONGWORD_TYPE
#define __SYSCALL_ULONG_TYPE	__ULONGWORD_TYPE
#define __CPU_MASK_TYPE 	__ULONGWORD_TYPE

/* Tell the libc code that off_t and off64_t are actually the same type
   for all ABI purposes, even if possibly expressed as different base types
   for C type-checking purposes.  */
#define __OFF_T_MATCHES_OFF64_T	1

/* Same for ino_t and ino64_t.  */
#define __INO_T_MATCHES_INO64_T	1

/* And for __rlim_t and __rlim64_t.  */
#define __RLIM_T_MATCHES_RLIM64_T	1

/* Number of descriptors that can fit in an `fd_set'.  */
#define	__FD_SETSIZE		1024

#endif /* bits/typesizes.h */
