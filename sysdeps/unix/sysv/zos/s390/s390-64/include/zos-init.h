/* Extremely early init routines for z/OS.
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

/* PLT/GOT indirection must be avoided for parts of this file.  */

#ifndef _ZOS_INIT_H
#define _ZOS_INIT_H 1

#include <elf.h>
#include <stdint.h>
#include <sysdep.h>
#include <zos-utils.h>
#include <ldsodefs.h>
#include <dl-procinfo.h>

struct bpxk_arg_list
{
  uint32_t *count;
  uint32_t **lens;
  char     **vals;
};

/* The format of the structure %r1 points to when we receive control.  */

struct bpxk_args
{
  struct bpxk_arg_list argv;
  struct bpxk_arg_list envp;
};

/* Helpers to translate program arguments and environ from the OS format
   and encoding to the internally usable format and encoding.  */

/* Minimal size for the argv/envp pointers themselves, plus an extra
   word for argc, and another for the ehdr address in shared cases.  */

#define ARGS_POINTERS_SIZE(arg_info)					\
  ((*arg_info->argv.count + *arg_info->envp.count + 4) * sizeof (char *))

static inline size_t __attribute__ ((always_inline))
args_min_size (const struct bpxk_args *arg_info)
{
  size_t total = ARGS_POINTERS_SIZE (arg_info);

  for (uint32_t i = 0; i < *arg_info->argv.count; i++)
    total += *arg_info->argv.lens[i] + 1;

  for (uint32_t i = 0; i < *arg_info->envp.count; i++)
    total += *arg_info->envp.lens[i] + 1;

  return total;
}

static inline void * __attribute__ ((always_inline))
translate_and_copy_one_set (void *mem, char **dest,
			    const struct bpxk_arg_list *arglist)
{
  size_t narg;
  for (narg = 0; narg < *arglist->count; narg++)
    {
      uint32_t arg_len = *arglist->lens[narg];
      char *tr_arg = mem;

      /* Translate the argument to ASCII.  */
      tr_a_until_len (arglist->vals[narg], tr_arg, arg_len);

      tr_arg[arg_len] = '\0';
      dest[narg] = tr_arg;
      mem = (void *) ((uintptr_t) mem + arg_len + 1);
    }
  dest[narg] = NULL;
  return mem;
}

static inline void * __attribute__ ((always_inline))
translate_and_copy_args (void *mem,
			 const struct bpxk_args *arg_info,
			 void *ehdr __attribute_used__)
{
  /* Reserve a word for argc.  */
  char **args_and_envs = (void *) ((uintptr_t) mem + sizeof (long int));
  char **envp_start = &args_and_envs[*arg_info->argv.count + 1];

#ifdef SHARED
  /* Store the ehdr pointer, if we have one.  */
  *(uintptr_t *) mem = (uintptr_t) ehdr;
#endif

  /* Store argc.  */
  *((long int *) mem + 1) = *arg_info->argv.count;

  /* Reserve space for the argv/p array itself.  */
  mem = (void *) ((uintptr_t) mem + ARGS_POINTERS_SIZE (arg_info));

  /* Set up the args.  */
  mem = translate_and_copy_one_set (mem, args_and_envs, &arg_info->argv);

  /* Set up the envs.  */
  mem = translate_and_copy_one_set (mem, envp_start, &arg_info->envp);

  return mem;
}

/* Do essential process start up. We need to allocate some memory that
   will never get freed, so the user should supply an allocator. This
   is a macro instead of a function to allow that allocator to be
   alloca in a noreturn function. arg_info should be the value of %r1
   when the process received control. Return a pointer to the properly
   processed args.  */

#define ESSENTIAL_PROC_INIT(allocator, arg_info, dub_fun, ehdr)		\
  ({									\
    void *__ret;							\
    /* Convert argv and argc into a Linux-like format, and convert	\
       to ASCII. Note that argv[argc + 1] must be __environ[0].  */	\
    									\
    size_t __total_args_size = args_min_size (arg_info);		\
    /* Don't assume the allocator alligns as much as we want.  */	\
    void *__mem =							\
      (allocator) (__total_args_size + __alignof__ (char *) - 1);	\
    __mem = (void *) (((uintptr_t) __mem + __alignof__ (char *) - 1)	\
		      & ~(__alignof__ (char *) - 1));			\
    __ret = __mem;							\
    __mem = translate_and_copy_args (__mem, arg_info, ehdr);		\
    									\
    /* We need to guaruantee that this task is dubbed as a process	\
       before proceeding. The easiest way to do that seems to be	\
       to do an arbitrary syscall and ignore the result. Our consumer	\
       may have some processing they would do anyway that would		\
       dub, so let them run that instead.				\
       z/OS TODO: Find the absolute minimum-cost way to dub.	*/	\
    									\
    if ((dub_fun) != NULL)						\
      (dub_fun) ();							\
    else								\
      (void) INLINE_SYSCALL_CALL (getpid);				\
    									\
    /* We should now be dubbed. Set THLIccsid to 819 for ASCII to	\
       register that our intput and output are typically expected to	\
       be ASCII, or at least should not be translated by other ASCII	\
       consumers and producers.  */					\
    (void) set_prog_ccsid (819);					\
    __ret;								\
  })

/* Do the initialization that is common between _dl_aux_init and
   _dl_sysdep_start. This doesn't need to avoid the PLT.
   Macro because otherwise the compiler will choke on it even when it's
   unused.  */
#define _dl_min_init()							\
  do									\
    {									\
      uid_t uid = 0;							\
      gid_t gid = 0;							\
      									\
      /* z/OS TODO: What IS page size on z/OS? _SC_PAGE_SIZE is always	\
	 1 MB for AMODE 64 procs, but other things still operate on	\
	 4 KB chunks.  */						\
      GLRO(dl_pagesize) = 4096;						\
      GLRO(dl_platform) = _dl_check_platform ();			\
      GLRO(dl_hwcap) = _dl_check_hwcaps ();				\
      GLRO(dl_hwcap2) = 0;						\
      /* This is defined as the resolution of times(). On z/OS times()	\
	 counts in hundreths of a second.  */				\
      GLRO(dl_clktck) = 100;						\
      /* z/OS TODO: What should this be?  */				\
      GLRO(dl_fpu_control) = 0;						\
      /* z/OS TODO: IMPORTANT: For security, we need 16 random bits.	\
	 Find some.  */							\
      _dl_random = (void *) 0x7f7f7f7f;					\
      									\
      uid ^= __getuid ();						\
      uid ^= __geteuid ();						\
      gid ^= __getgid ();						\
      gid ^= __getegid ();						\
      /* If one of the two pairs of IDs does not match this is a setuid	\
	 or setgid run.  */						\
      __libc_enable_secure = uid | gid;					\
    } while (0)

#endif /* !_ZOS_INIT_H  */
