/* Get directory entries.  z/OS version.
   Copyright (C) 2020 Free Software Foundation, Inc.
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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sysdep.h>


typedef void (*__bpx4rdd_t) (const int32_t *dirfd, void * const *buf,
                             const int32_t *buf_alet, const uint32_t *buflen,
                             int32_t *retval, int32_t *retcode,
                             int32_t *reason_code);


/* Directory entry structure that is returned by BPX4RDD z/OS 
   callable service. */
struct zos_dirent
{
  unsigned short int d_reclen; /* Total entry length. */
  unsigned short int d_namlen; /* Length of the following d_name field. */
  char d_name[1]; /* Name of the entry. */
/* Depending on the z/OS UNIX file system such fields as file serial 
   number and some additional physical file system specific data may
   be presented after 'd_name' field. */
};


/* Converts 'count' number of directory entries in the buffer 'buf'
   from z/OS internal format to glibc format.
   The return value of the function is the pointer to the next byte
   after the last directory entry in the buffer 'buf'. Also 'count'
   input parameter is used as return value too - it is updated with
   the actual number of directory entries in the buffer as the last
   entry can be a null name entry (name length of 0) to indicate
   the end of the directory. */
unsigned char* convert_dirs(unsigned char *buf, size_t *count)
{
  struct zos_dirent *zosdir;
  unsigned char *next_dir = buf;
  unsigned short int reclen;
  unsigned short int namlen;
  size_t fsdatalen = 0;
  int cnt = *count;

  /* Iterate over all directory entries in the buffer. */
  while(cnt > 0)
    {
      /* Current directory entry. */
      zosdir = (struct zos_dirent *)next_dir;
      reclen = zosdir->d_reclen;
      namlen = zosdir->d_namlen;

      /* Check for a null name entry. */
      if ((reclen == 4) && (zosdir->d_namlen == 0))
	{
	  /* Decrement the actual number of directory entries and stop. */
	  (*count)--;
	  return buf;
	}

      /* Do we need to move current entry to the left to trim empty spaces ?
	 That situation occurs if directory entry has some additional physical
	 file system specific data after file serial number field. At the
	 moment we just ignore such additional data. So to avoid unused spaces
	 between directory entries in buffer we have to shift each next entry
	 to the end of the previous entry. */
      if (buf != next_dir)
	{
	  /* Move entry ('reclen' bytes) from the next entry pointer 'next_dir'
	     to the current entry pointer 'buf'. */
	  memmove(buf, next_dir, reclen);
	  /* Update current directory entry pointer. */
	  zosdir = (struct zos_dirent*)buf;
	}

      /* Update next entry pointer to the next directory entry. */
      next_dir += reclen;

      /* Get size of file system data. It can be 0, 4 and more than 4 bytes. */
      fsdatalen = reclen - zosdir->d_namlen - 4;

      /* Update file system data size and entry size if necessary. */
      if (fsdatalen != 4)
	{
	  /* Get size of additional file system data after file serial number. */
	  if (fsdatalen > 4)
	    fsdatalen -= 4;

	  /* Ignore as additional file system data and unknown file system
	     data that is less than 4 bytes. Update entry size and file
	     system data size. */
	  reclen -= fsdatalen;
	  zosdir->d_reclen = reclen;
	  /* File system data size now is 0 or 4 bytes only. */
	  fsdatalen = reclen - zosdir->d_namlen - 4;
	}

      /* Convert current directory entry from z/OS internal format to glibc format. */
      if (fsdatalen > 0)
	{
	  /* If z/OS directory entry has serial file number field, it will have
	     the same size after z/OS->glibc format conversion, only offsets of
	     fields have to be changed:

	     - First field in glibc directory entry is file serial number 'd_ino',
	       so we have to shift by size of 'd_reclen' glibc field's offset all
	       content of z/OS directory entry to the end of the entry, and assign
	       file serial number to 'd_ino' field in the beginning of the entry.

	     - glibc directory entry must have 1-byte file type field 'd_type' as
	       it is used in glibc library code. z/OS directory entry doesn't
	       have such field, so we have to reserve 1 byte memory for that field
	       during z/OS->glibc format conversion. Also z/OS entry name is not
	       terminated by a null character but glibc entry name must have null
	       character at the end. That is the second byte we have to reserve
	       during format conversion. z/OS entry has name length field which
	       is not necessary, glibc entry doesn't use such field. So we can
	       use 2-byte memory allocated for name length field in z/OS entry as
	       the necessary 2 additional bytes in glibc entry. Total directory
	       entry size will be the same after conversion.

	     - First byte of 2-byte name length field in z/OS entry is used as
	       1-byte file type field 'd_type' in glibc entry. Second byte have
	       to be used as entry name field start address. So we have to shift
	       z/OS entry name to that address. After that unused byte at the
	       end of the entry can be used as null character in the entry name
	       field. As we need to convert entry name from EBCDIC to ASCCI it
	       is possible to combine convert and shift operations using macros
	       tr_until_len().

	     - File type field 'd_type' in glibc entry has the only value at the
	       moment - DT_UNKNOWN. Need to investigate how we can get file type
	       info by file serial number in z/OS. */

	  /* Save serial file number. */
	  int32_t fno = *((int32_t*)(zosdir->d_name + zosdir->d_namlen));
	  /* Get the size of data in z/OS directory entry we have to shift. */
	  size_t move_bytes = zosdir->d_namlen + offsetof(struct zos_dirent, d_name);
	  /* Shift data to the end of the directory entry. */
	  memmove(buf + offsetof(struct dirent, d_reclen), buf, move_bytes);
	  /* Now work with directory entry as with glibc format entry. */
	  struct dirent *dir = (struct dirent *)buf;
	  /* Assign file serial number. */
	  dir->d_ino = fno;
	  /* Stub field at the moment. */
	  dir->d_type = DT_UNKNOWN;
	  /* Translate entry name to ASCII and shift it to the appropriate address
	     in glibc directory entry (1 byte back). */
	  tr_until_len(dir->d_name+1, dir->d_name, namlen, e_to_a);
	  /* Add null character at the end of the entry name field. */
	  dir->d_name[namlen] = '\0';
	}
      else
	{
	  /* If z/OS directory entry doesn't have serial file number field, we need
	     to have reserved memory in buffer to be able to insert missing field
	     into each entry by shifting entries to the end of the buffer.

	     That case is not implemented yet. Just return NULL address and zero
	     directory entries at the moment. */
	  *count = 0;
	  return NULL;
	}

      /* Update 'buf' pointer to the empty address after the current directory entry. */
      buf += reclen;
      /* Decrement total number of entries in buffer. */
      cnt--;
    }

  return buf;
}


/* Reads multiple name entries from a directory referred to by the open file
   descriptor 'fd' into the buffer 'buf'. The argument 'nbytes' specifies
   the size of that buffer.
   The return value of the function is the number of bytes read. On end of
   directory, 0 is returned. On error, -1 is returned.

   That function emulates internal Linux system call 'getdents64()' that is
   used in 'readdir()' system call wrapper. */
ssize_t
__getdents64 (int fd, char *buf, size_t nbytes)
{
  int32_t retval, reason_code;
  INTERNAL_SYSCALL_DECL (retcode);
  int32_t dirfd = fd;
  void *pbuf = buf;
  const int32_t alet = 0;
  uint32_t buflen = nbytes;
  size_t count = 0;


  /* Use BPX4RDD z/OS callable service to read multiple name entries from
     the directory. */
  /* z/OS TODO: In order to implement seekdir we need to use readdir2
     in indexed mode here. But then we'd also need access to the index
     to read from, and the ablity to increment that index, in short
     we'd need access to dirp->offset, presuming readdir2 works as
     expected. */
  BPX_CALL (readdir, __bpx4rdd_t, &dirfd, &pbuf, &alet, &buflen,
	    &retval, &retcode, &reason_code);

  if (INTERNAL_SYSCALL_ERROR_P (retval, retcode))
    {
      __set_errno (INTERNAL_SYSCALL_ERRNO (retval, retcode));
      return -1;
    }

  /* Save the number of directory entries that have been read into the buffer. */
  count = retval;

  /* As format of glibc and z/OS directory entry is different it is necessary
     to convert each entry in the buffer from z/OS to glibc format. */
  if (count > 0)
    pbuf = (void*)convert_dirs(pbuf, &count);

  /* Check actual number of glibc name entries in buffer after conversion as
     it can be changed. */
  if (count == 0)
    pbuf = buf;

  return (ssize_t)(pbuf - (void*)buf);
}
