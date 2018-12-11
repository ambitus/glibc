/* Copyright (C) 2018 Rocket Software.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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

#ifndef _BITS_SIGCONTEXT_H
#define _BITS_SIGCONTEXT_H  1

#if !defined _SIGNAL_H && !defined _SYS_UCONTEXT_H
# error "Never use <bits/sigcontext.h> directly; include <signal.h> instead."
#endif

/* Some things need this, it can't hurt to have it.  */
#ifndef sigcontext_struct
# define sigcontext_struct sigcontext
#endif

/* This structure is known in the BPX documentation as the PPSD and is
   different for AMODE 31 programs.  */

/* TODO: There is a race with literally every signal.
   Every single signal overwrites the same info.  */

/* TODO: should we hide this structure from the users?  */

/* TODO: pretty this up when it has been tested/finalized.  */

struct sigcontext
{
  char eyecatcher[4];
  unsigned char subpool;
  unsigned char length[3];
  unsigned int self_ptr;
  unsigned int prtrm_status;
  unsigned int signum;
  char flags1;
  char flags2;
  char action;
  char flags3;
  unsigned int __reserved1;
  unsigned long int sigaction_sigmask;
  char flags4[4];
  unsigned long int sigmask_on_return;
  unsigned int __reserved2;
  unsigned int __reserved3;
  char regs[64];
  unsigned long int __reserved4;
  unsigned int ars[16];
  char userdata[2];
  char killdata[2];
  unsigned int __reserved5;
  unsigned int last_ptraced_sig;
  unsigned int __reserved6[2];
  unsigned long int sending_threadid;
  unsigned long int target_threadid;
  unsigned int sending_procid;
  unsigned int sending_real_uid;
  unsigned int __reserved7;
  union {
    unsigned int exit_status;
    unsigned int signal;
  };
  unsigned int __reserved8;
  unsigned int error_return_code;
  unsigned long int sigmask_for_handler;
  unsigned int __reserved9[25];
  unsigned int __reserved10;
  unsigned int signal_delay_time;
  unsigned int regs_high[16];
  unsigned long int sending_threadid_msg;
  unsigned long int sending_jobname;
  unsigned int __reserved11[4];
  void *handler_addr;
  void *sir_addr;
  void *udata_addr;
  struct
  {
    unsigned long int mask;
    unsigned long int addr;
  } psw;
  unsigned long int bpx1qut_user_data;
  unsigned long int bpx1sia_user_data;
  void *workarea_addr;
  void *fault_addr;
  unsigned long int band_event;
  unsigned long int sig_si_value;
  unsigned int __reserved12[4];
  char __reserved13[2];
  unsigned short exit_flags;
  unsigned short ppsd_aiocb_count;
  unsigned short last_arr_ind;
  unsigned long int aiocb_arr;
};

#endif /* _BITS_SIGCONTEXT_H  */
