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

/* z/OS TODO: remove these when we update to recent glibc sources.  */
#ifndef HWCAP_S390_VXRS_EXT2
# define HWCAP_S390_VXRS_EXT2 0
#endif
#ifndef HWCAP_S390_VXRS_PDE
# define HWCAP_S390_VXRS_PDE 0
#endif
#ifndef HWCAP_S390_SORT
# define HWCAP_S390_SORT 0
#endif
#ifndef HWCAP_S390_DFLT
# define HWCAP_S390_DFLT 0
#endif

static inline uint64_t
check_hwcaps (void)
{
  /* We always support these.  */
  uint64_t hwcap = HWCAP_S390_HIGH_GPRS;

  /* Check the stfl facilities. Assume the stfl PSA bytes are ok.  */
  uint32_t stfl = *(uint32_t *) ((volatile uintptr_t) (200));
  if ((stfl & 0x80000000) != 0) hwcap |= HWCAP_S390_ESAN3;
  if ((stfl & 0x20000000) != 0) hwcap |= HWCAP_S390_ZARCH;
  if ((stfl & 0x01000000) != 0) hwcap |= HWCAP_S390_STFLE;
  if ((stfl & 0x00004000) != 0) hwcap |= HWCAP_S390_MSA;
  if ((stfl & 0x00001000) != 0) hwcap |= HWCAP_S390_LDISP;
  if ((stfl & 0x00000400) != 0) hwcap |= HWCAP_S390_EIMM;
  if ((stfl & 0x00000202) == 0x00000202) hwcap |= HWCAP_S390_ETF3EH;
  /* z/OS TODO: When should HWCAP_S390_HPAGE be valid? What EXACT features
     does it indicate the presence of? How do we test their availability?
     right now we use it as a proxy for EDAT-1, which probably isn't
     correct.  */
  if ((stfl & 0x00800000) != 0) hwcap |= HWCAP_S390_HPAGE;

  if (hwcap & HWCAP_S390_STFLE)
    {
      /* z/OS TODO: Can we rely on the PSA bits, do we need to use STFLE?  */
      uint64_t stfle_list[4] = {0};
      register uint64_t reg0 __asm__ ("0") = (sizeof (stfle_list) / 8) - 1;
      /* z/OS TODO: We definitely shouldn't use stfle twice per program
	 (here and in the ifunc resolver code).  */
      __asm__ (".machine push"        "\n\t"
	       ".machine \"z9-109\""  "\n\t"
	       ".machinemode \"zarch_nohighgprs\"\n\t"
	       "stfle %0"             "\n\t"
	       ".machine pop"         "\n"
	       : "=QS" (stfle_list), "+d" (reg0)
	       : : "cc");;

      if ((stfle_list[0] & 0x00280000UL) == 0x00280000UL)
	hwcap |= HWCAP_S390_DFP;

      /* HWCAP_S390_TE requires constrained and regular transactions.  */
      if ((stfle_list[1] & 0x0040000000000000UL)
	  && (stfle_list[0] & 0x00002000UL))
	hwcap |= HWCAP_S390_TE;

      if (stfle_list[2] & 0x0400000000000000UL) hwcap |= HWCAP_S390_GS;
      if (stfle_list[2] & 0x0000020000000000UL) hwcap |= HWCAP_S390_SORT;
      if (stfle_list[2] & 0x0000010000000000UL) hwcap |= HWCAP_S390_DFLT;
      if (stfle_list[2] & 0x4000000000000000UL)
	{
	  hwcap |= HWCAP_S390_VX;
	  if (stfle_list[2] & 0x0200000000000000UL) hwcap |= HWCAP_S390_VXE;
	  if (stfle_list[2] & 0x0100000000000000UL) hwcap |= HWCAP_S390_VXD;
	  if (stfle_list[2] & 0x0000080000000000UL) hwcap |= HWCAP_S390_VXRS_EXT2;
	  if (stfle_list[2] & 0x0000008000000000UL) hwcap |= HWCAP_S390_VXRS_PDE;
	}

    }
  return hwcap;
}

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

  if (ehdr->e_entry == 0)
    /* z/OS TODO: Fail out here.  */;

  GLRO(dl_platform) = NULL; /* Default to nothing known about the platform.  */
  user_entry = (ElfW(Addr)) ehdr + ehdr->e_entry;

  phdr = (const void *) ehdr + ehdr->e_phoff;
  phnum = ehdr->e_phnum;
  /* z/OS TODO: How?  */
  GLRO(dl_pagesize) = __getpagesize ();
  user_entry = (ElfW(Addr)) ehdr + ehdr->e_entry;
  GLRO(dl_platform) = (void *) 0;		/* z/OS TODO: How?  */
  GLRO(dl_platformlen) = strlen (GLRO(dl_platform));
  GLRO(dl_hwcap) = check_hwcaps;
  GLRO(dl_hwcap2) = 0;
  GLRO(dl_clktck) = 100;			/* z/OS TODO: How?  */
  GLRO(dl_fpu_control) = 0;			/* z/OS TODO: How?  */
  _dl_random = (void *) 0x7f7f7f7f;		/* z/OS TODO: How?  */

  uid ^= __getuid ();
  uid ^= __geteuid ();
  gid ^= __getgid ();
  gid ^= __getegid ();
  /* If one of the two pairs of IDs does not match this is a setuid
     or setgid run.  */
  __libc_enable_secure = uid | gid;

  __tunables_init (_environ);

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
}
#endif /* SHARED  */
