/* Operating system support for run-time dynamic linker.  z/OS version.
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

/* NOTE: Keep this file in sync with the generic file.  If the generic
   file changes to initialize/define additional things,
   initialize/define those things here.  */

#ifdef SHARED

extern char **_environ attribute_hidden;
extern char _end[] attribute_hidden;

/* Protect SUID program against misuse of file descriptors.  */
extern void __libc_check_standard_fds (void);
int __libc_enable_secure attribute_relro = 0;
rtld_hidden_data_def (__libc_enable_secure)
  int __libc_multiple_libcs = 0;	/* Defining this here avoids the inclusion
					   of init-first.  */
/* This variable contains the lowest stack address ever used.  */
void *__libc_stack_end attribute_relro = NULL;
rtld_hidden_data_def(__libc_stack_end)
  void *_dl_random attribute_relro = NULL;

#define DL_FIND_ARG_COMPONENTS(cookie, ehdr, argc, argv, envp)	\
  do {								\
    (ehdr) = *(const ElfW(Ehdr) **) cookie;			\
    (argc) = *((long int *) cookie + 1);			\
    (argv) = (char **) ((long int *) cookie + 2);		\
    (envp) = (argv) + (argc) + 1;				\
  } while (0)

/* z/OS TODO: This isn't the true stack end.  */
#define DL_STACK_END(cookie) ((void *) (cookie))

ElfW(Addr)
_dl_sysdep_start (void **start_argptr,
		  void (*dl_main) (const ElfW(Phdr) *phdr, ElfW(Word) phnum,
				   ElfW(Addr) *user_entry, ElfW(auxv_t) *auxv))
{
  const ElfW(Ehdr) *ehdr;
  const ElfW(Phdr) *phdr;
  ElfW(Word) phnum = 0;
  ElfW(Addr) user_entry;
  uid_t uid = 0;
  gid_t gid = 0;

  __libc_stack_end = DL_STACK_END (start_argptr);
  DL_FIND_ARG_COMPONENTS (start_argptr, ehdr, _dl_argc, _dl_argv, _environ);

  user_entry = (ElfW(Addr)) ehdr + ehdr->e_entry;
  phdr = (const void *) ((ElfW(Addr)) ehdr + ehdr->e_phoff);
  phnum = ehdr->e_phnum;

  /* z/OS TODO: What IS page size on z/OS? _SC_PAGE_SIZE is always 1 MB
     for AMODE 64 procs, but other things still operate on 4 KB
     chunks.  */
  GLRO(dl_pagesize) = 4096;
  GLRO(dl_platform) = _dl_check_platform ();
  GLRO(dl_platformlen) = strlen (GLRO(dl_platform));
  GLRO(dl_hwcap) = _dl_check_hwcaps ();
  GLRO(dl_hwcap2) = 0;
  /* This is defined as the resolution of times(). On z/OS times() counts
     in hundreths of a second.  */
  GLRO(dl_clktck) = 100;
  /* z/OS TODO: What should this be?  */
  GLRO(dl_fpu_control) = 0;
  /* z/OS TODO: IMPORTANT: For security, we need 16 random bits. Find some.  */
  _dl_random = (void *) 0x7f7f7f7f;

  uid ^= __getuid ();
  uid ^= __geteuid ();
  gid ^= __getgid ();
  gid ^= __getegid ();
  /* If one of the two pairs of IDs does not match this is a setuid
     or setgid run.  */
  __libc_enable_secure = uid | gid;

  __tunables_init (_environ);

  DL_PLATFORM_INIT;

  /* Determine the length of the platform name.  */
  if (GLRO(dl_platform) != NULL)
    GLRO(dl_platformlen) = strlen (GLRO(dl_platform));

  /* if this is a suid program we make sure that fds 0, 1, and 2 are
     allocated.  if necessary we are doing it ourself.  if it is not
     possible we stop the program.  */
  if (__builtin_expect (__libc_enable_secure, 0))
    __libc_check_standard_fds ();

  (*dl_main) (phdr, phnum, &user_entry, NULL);
  return user_entry;
}

void
_dl_sysdep_start_cleanup (void)
{
}

void
_dl_show_auxv (void)
{
  _dl_printf ("z/OS does not provide an aux vector\n",
	      auxvars[idx].label, val);
}
#endif /* SHARED  */
