/* Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file with other
   programs, and to distribute those programs without any restriction
   coming from the use of this file. (The GNU Lesser General Public
   License restrictions do apply in other respects; for example, they
   cover modification of the file, and distribution when not linked
   into another program.)

   Note that people who make modified versions of this file are not
   obligated to grant this special exception for their modified
   versions; it is their choice whether to do so. The GNU Lesser
   General Public License gives permission to release a modified
   version without this exception; this exception also makes it
   possible to release a modified version which carries forward this
   exception.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */


#include <errno.h>
#include <sys/socket.h>
#include <sysdep.h>
#include <link.h>
#include <zos-core.h>

typedef void (*__bpx4lod_t) (const int *filename_len,
			     const char *filename,
			     const int *flags,
			     const int *libpath_len,
			     const char *libpath,
			     void (**entrypoint)(void),
			     int *retval, int *retcode,
			     int *reason_code);

/* Load a program at the specified path into storage.
   z/OS TODO: Expand on this a lot. Potentially rename. Work with
   AMODE31 stuff.  */
void *
__loadhfs (char *path)
{
  int retval, reason_code;
  INTERNAL_SYSCALL_DECL (retcode);

  const char *libpath = "";
  const int flags = 0;

  void *entry;
  char translated_path[__BPXK_PATH_MAX];
  int path_len = translate_and_check_size (path, translated_path);
  int libpath_len = 0;

  if (__glibc_unlikely (path_len == __BPXK_PATH_MAX
			|| libpath_len == __BPXK_PATH_MAX))
    {
      return NULL;
    }

  BPX_CALL (loadhfs, __bpx4lod_t, &path_len, translated_path, &flags,
	    &libpath_len, libpath, &entry, &retval,
	    &retcode, &reason_code);

  /* z/OS TODO: some weird stuff with the return code for AMODE31
     progs.  */
  return entry;
}
#if !defined (ZOS_HIDDEN_SYSCALL) && (IS_IN (libc) || IS_IN (rtld))
hidden_def (__loadhfs)
#endif

void *
__load_pt_interp (void)
{
  const ElfW(Phdr) *first_phdr, *phdr;
  size_t phnum;
  size_t offset = 0;
  extern const ElfW(Ehdr) __ehdr_start
    __attribute__ ((visibility ("hidden")));

  first_phdr = ((const ElfW(Phdr) *)
		((const char *) &__ehdr_start + __ehdr_start.e_phoff));
  phnum = __ehdr_start.e_phnum;

  for (phdr = first_phdr; phdr < &first_phdr[phnum]; ++phdr)
    if (phdr->p_type == PT_INTERP)
      {
	offset = phdr->p_offset;
	break;
      }

  if (offset == 0)
    return NULL;

  return __loadhfs ((char *) &__ehdr_start + offset);
}
#if !defined (ZOS_HIDDEN_SYSCALL) && (IS_IN (libc) || IS_IN (rtld))
hidden_def (__load_pt_interp)
#endif
