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

/* PLT/GOT indirection must be avoided in this file.  */

#ifndef _ZOS_INIT_H
#define _ZOS_INIT_H 1

#include <stdint.h>
#include <sysdep.h>
#include <zos-utils.h>

struct bpxk_arg_list
{
  uint32_t *count;
  uint32_t **lens;
  char     **vals;
};

/* The format of the structure r1 points to when we receive control.  */
struct bpxk_args
{
  struct bpxk_arg_list argv;
  struct bpxk_arg_list envp;
};

/* Helpers to translate program arguments and environ from the OS format
   and encoding to the internally usable format and encoding.  */

#define ARGS_POINTERS_SIZE(arg_info)					\
  ((*arg_info->argv.count + *arg_info->envp.count + 2) * sizeof (char*))


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

static inline char ** __attribute__ ((always_inline))
translate_and_copy_args (void *mem,
			 const struct bpxk_args *arg_info)
{
  char **args_and_envs = mem;
  char **envp_start = &args_and_envs[*arg_info->argv.count + 1];

  /* Reserve space for the argv/p array itself.  */
  mem = (void *) ((uintptr_t) mem + ARGS_POINTERS_SIZE (arg_info));

  /* Set up the args.  */
  mem = translate_and_copy_one_set (mem, args_and_envs, &arg_info->argv);

  /* Set up the envs.  */
  mem = translate_and_copy_one_set (mem, envp_start, &arg_info->envp);

  return args_and_envs;
}

/* Do essential process start up. We need to allocate some memory that
   will never get freed, so the user should supply an allocator. This
   is a macro instead of a function to allow that allocator to be
   alloca in a noreturn function. arg_info should be the value of r1 when
   the process received control. The properly processed args in the
   standard format are stored into final_arg_ptr.  */

#define ESSENTIAL_PROC_INIT(allocator, arg_info, dub_fun,		\
			    fin_args_ptr_ptr)				\
  do									\
    {									\
      /* Convert argv and argc into a Linux-like format, and convert	\
	 to ASCII. Note that argv[argc + 1] must be __environ[0].  */	\
									\
      size_t __total_args_size = args_min_size (arg_info);		\
      /* Don't assume the allocator alligns as much as we want.  */	\
      void *__mem =							\
	allocator (__total_args_size + __alignof__ (char *) - 1);	\
      __mem = (void *) (((uintptr_t) __mem + __alignof__ (char *) - 1)	\
			& ~(__alignof__ (char *) - 1));			\
      *(fin_args_ptr_ptr) = translate_and_copy_args (__mem, arg_info);	\
									\
      /* We need to guaruantee that this task is dubbed as a process	\
	 before proceeding. The easiest way to do that seems to be	\
	 to do an arbitrary syscall and ignore the result. Our consumer	\
	 may have some processing they would do anyway that would	\
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
    } while (0)

#endif /* !_ZOS_INIT_H  */
