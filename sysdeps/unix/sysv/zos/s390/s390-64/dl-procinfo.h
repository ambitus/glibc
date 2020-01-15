/* s390 version of processor capability information handling macros.
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

#ifndef _DL_ZOS_PROCINFO_H
#define _DL_ZOS_PROCINFO_H 1
#include <sysdeps/unix/sysv/linux/s390/dl-procinfo.h>
#include <sys/auxv.h>

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

/* Find the hwcaps.  */

static inline unsigned long
_dl_check_hwcaps (void)
{
  /* We always support these.  */
  unsigned long hwcap = HWCAP_S390_HIGH_GPRS;

  /* Check the stfl facilities. Assume the stfl PSA bytes are ok.  */
  unsigned int stfl = *(unsigned int *) ((volatile unsigned long) (200));
  if ((stfl & 0x80000000) != 0) hwcap |= HWCAP_S390_ESAN3;
  if ((stfl & 0x20000000) != 0) hwcap |= HWCAP_S390_ZARCH;
  if ((stfl & 0x01000000) != 0) hwcap |= HWCAP_S390_STFLE;
  if ((stfl & 0x00004000) != 0) hwcap |= HWCAP_S390_MSA;
  if ((stfl & 0x00001000) != 0) hwcap |= HWCAP_S390_LDISP;
  if ((stfl & 0x00000400) != 0) hwcap |= HWCAP_S390_EIMM;
  if ((stfl & 0x00000202) == 0x00000202) hwcap |= HWCAP_S390_ETF3EH;
  /* z/OS TODO: When should HWCAP_S390_HPAGE be valid? What EXACT features
     does it indicate the presence of? How do we test their availability?
     Right now we use it as a proxy for EDAT-1, which probably isn't
     correct.  */
  if ((stfl & 0x00800000) != 0) hwcap |= HWCAP_S390_HPAGE;

  if (hwcap & HWCAP_S390_STFLE)
    {
      unsigned long stfle_list[4] = {0};
      register unsigned long reg0 __asm__ ("0") = (sizeof (stfle_list) / 8) - 1;

      /* z/OS TODO: Right now we usually do a STFLE twice per program, once
	 here and once in the ifunc resolver. 1) Do we need to do it at all,
	 or can we rely on the PSA bits? 2) If we need to do it at all, can
	 we share the results so we only run it once?  */

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

/* Find the model of this machine.
   z/OS TODO: Implement this.  */

static inline char *
_dl_check_platform (void)
{
  return NULL;
}

#endif /* !_DL_ZOS_PROCINFO  */
