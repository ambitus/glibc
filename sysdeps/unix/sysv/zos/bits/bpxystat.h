/* Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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

   This file is designed to be included multiple times, and only to be,
   included from within a structure declaration. This is used to allow
   struct stat to have the same first elements as struct kernel_stat
   so struct stat can be initialized via a memcopy from a struct
   kernel_stat, without making a struct kernel_stat a member of struct
   stat.  */

#if !defined _BITS_STAT_H
# error "Never include <bits/bpxystat.h> directly; use <sys/stat.h> instead."
#endif


/* TODO: figure out padding, if any is necessary.
   Later TODO: make sure everything in this file is valid in all
   C standards.	 */


/* TODO: determine what types to use for special fields, like st_auditid,
   st_seclabel, etc.  */

/* This is the structure used by the stat() family of syscalls.
   It is close to, but is not always exactly compatible with, the stat()
   structure returned by the stat() syscall on linux. The main
   problem is the lack of nanosecond time fields. Of course, we can't
   actually get that information anyway on this platform right now...
   TODO: we will use this structure as struct stat for the moment.  */
  char st_eye[4];	      /* 'STAT' in EBCDIC */
  __uint16_t st_length;	      /* sizeof(struct bpxystat) */
  __uint16_t st_version;      /* bpxystat version */
  __mode_t st_mode;	      /* the file mode */
  __ino_t st_ino;	      /* inode ID */
  __dev_t st_dev;	      /* Device ID */
  __nlink_t st_nlink;	      /* Number of links to this file */
  __uid_t st_uid;	      /* File owner's UID */
  __gid_t st_gid;	      /* File owner's GID */
  __off_t st_size;	      /* File size in bytes */
  /* obsolete fields */
  __int32_t _bpx_atime32;     /* 32-bit atime */
  __int32_t _bpx_mtime32;     /* 32-bit mtime */
  __int32_t _bpx_ctime32;     /* 32-bit ctime */
  /* end obsolete fields */
  /* first halfword is major, second is minor */
  __dev_t st_rdev;	      /* Device numbers */
  __uint32_t st_auditoraudit; /* Auditor audit info */
  __uint32_t st_useraudit;    /* User audit info */
  __blksize_t st_blksize;     /* File block size */
  /* TODO: check this type */
  __int32_t _bpx_createtime32; /* 32-bit creation time */
  char st_auditid[16];	      /* RACF File ID */
  char __bpx_reserved1[4];    /* Reserved by IBM */
  __uint16_t st_ccsid;        /* File Tag CCSID */
/* Special CCSID values */
#define __ZOS_STAT_FT_UNTAGGED 0
#define __ZOS_STAT_FT_BINARY   0xFFFF
  __uint16_t st_ftflags;      /* File Tag Flags */
/* Special file tagging masks */
#define __ZOS_STAT_FT_ISTEXT      0x8000
#define __ZOS_STAT_FT_ISDEFERRED  0x4000
  char __bpx_reserved2[8];    /* Obsolete, apparently */
  /* TODO: check if high word gets set in AMODE 31 */
  __blkcnt_t st_blocks;	      /* Number of blocks allocated */
  __uint32_t st_genvalue;     /* Some random flags */
/* TODO: decide on better names for these */
# define __ZOS_STAT_GEN_SHLIB	   0x10
# define __ZOS_STAT_GEN_NOSHAREAS  0x08
# define __ZOS_STAT_GEN_APF_AUTH   0x04
# define __ZOS_STAT_GEN_PROG_CTRL  0x02
# define __ZOS_STAT_GEN_SYMLINK	   0x01
  /* TODO: check the types for the rest of the fields */
  __int32_t _bpx_reftime32;   /* TODO: what is this? */
  __uint64_t st_fid;
  /* TODO: I don't understand the sizes for the following two,
     I think the docs indicate that they should be merged into
     one char, but common disagrees.  */
  char st_filefmt;	     /* TODO: what is this? */
  char st_ifsp_flag2;	     /* IFSP_FLAG2 ACL support (?) */
# define __ZOS_STAT_ACL_ACCESS	  0x80
# define __ZOS_STAT_ACL_FMODEL	  0x40
# define __ZOS_STAT_ACL_DMODEL	  0x20
  char __bpx_reserved3[2];   /* Reserved by IBM */
  /* TODO: check type for this */
  __int32_t st_ctimemsec;    /* Microsecond part of ctime */
  char st_seclabel[8];	     /* Security label */
  char __bpx_reserved4[4];   /* Reserved by IBM */
  /* End of version 1 fields */

  char __bpx_reserved5[4];   /* Reserved by IBM */
  __time_t _bpx_atime64;     /* 64-bit atime */
  __time_t _bpx_mtime64;     /* 64-bit mtime */
  __time_t _bpx_ctime64;     /* 64-bit ctime */
  __time_t st_createtime;    /* 64-bit creation time */
  __time_t st_reftime;	     /* TODO: what is this? */
  char __bpx_reserved6[16]  /* Reserved by IBM */
  __attribute__ ((__aligned__ (8)));  /* Make sure we always force this
					 structure to be 8-byte
					 aligned.  */
  /* End of version 2 fields */
