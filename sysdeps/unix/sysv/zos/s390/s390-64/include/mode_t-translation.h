/* Copyright (C) 2020 Free Software Foundation, Inc.
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

#ifndef __MODE_T_TRANSLATION
#define __MODE_T_TRANSLATION

#include <stdint.h>
#include <sys/stat.h>

#define __ZOS_S_IFMT   0xFF000000 /* These bits determine file type. */

/* z/OS file types. */
#define __ZOS_S_IFDIR  0x01000000  /* Directory. */
#define __ZOS_S_IFCHR  0x02000000  /* Character device. */
#define __ZOS_S_IFBLK  0x06000000  /* Block device. */
#define __ZOS_S_IFREG  0x03000000  /* Regular file. */
#define __ZOS_S_IFIFO  0x04000000  /* FIFO. */
#define __ZOS_S_IFLNK  0x05000000  /* Symbolic link. */
#define __ZOS_S_IFSOCK 0x07000000  /* Socket. */

/* Convert a user (GNU - de facto standard) mode_t
   to a kernel (BPXYMODE) uint32_t suitable for
   passing to BPX_CALL.

   By convention all functions and macros which
   expect a mode_t expect the user. Thus
   translation must occur in the same function
   which contains the BPX_CALL. Note that some
   structures passed to BPX_CALL may indirectly
   contains a BPXYMODE which must be translated.
   INLINE_SYSCALL invocations expect an user mode_t. */
static inline __attribute__ ((unused)) uint32_t
user_to_kern_mode_t (mode_t mode)
{
  /* All but the file format bits. */
  uint32_t kern_mode = (uint32_t) (~__S_IFMT) & mode;

  switch (mode & __S_IFMT)
    {
    case __S_IFDIR:
      kern_mode |= __ZOS_S_IFDIR;
      break;
    case __S_IFCHR:
      kern_mode |= __ZOS_S_IFCHR;
      break;
    case __S_IFBLK:
      kern_mode |= __ZOS_S_IFBLK;
      break;
    case __S_IFREG:
      kern_mode |= __ZOS_S_IFREG;
      break;
    case __S_IFIFO:
      kern_mode |= __ZOS_S_IFIFO;
      break;
    case __S_IFLNK:
      kern_mode |= __ZOS_S_IFLNK;
      break;
    case __S_IFSOCK:
      kern_mode |= __ZOS_S_IFSOCK;
      break;
    default:
      /* Unknown mode? Bits are reserved leave as zeros. */
      break;
    }

  return kern_mode;
}

/* Convert a kernel (BPXYMODE) uint32_t
   to a user (GNU - de facto standard) mode_t
   suitable for passing to any function with
   a mode_t argument.

   This translation should occur in any function
   returning a mode_t from a BPX_CALL. Note that
   some structures passed to BPX_CALL may indirectly
   contains a BPXYMODE which must be translated. */
static inline __attribute__ ((unused)) mode_t
kern_to_user_mode_t (uint32_t mode)
{
  /* All but the file format bits. */
  uint32_t user_mode = (uint32_t) (~__ZOS_S_IFMT) & mode;

  switch (mode & __ZOS_S_IFMT)
    {
    case __ZOS_S_IFDIR:
      user_mode |= __S_IFDIR;
      break;
    case __ZOS_S_IFCHR:
      user_mode |= __S_IFCHR;
      break;
    case __ZOS_S_IFBLK:
      user_mode |= __S_IFBLK;
      break;
    case __ZOS_S_IFREG:
      user_mode |= __S_IFREG;
      break;
    case __ZOS_S_IFIFO:
      user_mode |= __S_IFIFO;
      break;
    case __ZOS_S_IFLNK:
      user_mode |= __S_IFLNK;
      break;
    case __ZOS_S_IFSOCK:
      user_mode |= __S_IFSOCK;
    default:
      /* Unknown mode? Leave the bits as zeros. */
      break;
    }

  return (mode_t)user_mode;
}

#endif
