/* Copyright (C) 2019 Free Software Foundation, Inc.
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

/* TODO: should we hide this structure from the users?  */

/* TODO: pretty this up when it has been tested/finalized.  */

struct sigcontext
{
  char eyecatcher[4];
  unsigned char subpool;
  unsigned char length[3];
  unsigned int self_ptr;
  unsigned int prtrm_status;
  int sig_si_signo;
  char flags1;
  char flags2;
  char action;  /* TODO: What is this?  */
  char flags3;
  unsigned int __reserved1;
  char sigaction_sigmask[8];
  char flags4;
  char __reserved2[3];
  char sigmask_on_return[8];
  unsigned int __reserved3;
  unsigned int __reserved4;
  unsigned int gregs_low[16];
  char __reserved5[8];
  unsigned int ars[16];
  short sig_si_code;
  short killopts;
  unsigned int __reserved6;
  unsigned int last_ptraced_sig;
  char __reserved7[8];
  char sending_threadid[8];
  char target_threadid[8];
  unsigned int sig_si_pid;
  unsigned int sig_si_uid;
  unsigned int __reserved8;
  int sig_si_status;
  unsigned int __reserved9;
  int sig_si_errno;
  char sigmask_for_handler[8];
  unsigned int __reserved10[26];
  unsigned int signal_delay_time;
  unsigned int gregs_high[16];
  unsigned long int sending_threadid_msg;
  unsigned long int sending_jobname;
  unsigned int __reserved11[4];
  void *handler;
  void *sir;
  void *udata;
  struct
  {
    unsigned long int mask;
    unsigned long int addr;
  } psw;
  unsigned long int bpx1qut_user_data;
  unsigned long int bpx1sia_user_data;
  void *workarea_addr;
  void *sig_si_addr;
  long int sig_si_band;
  unsigned long int sig_si_value;
  unsigned short __reserved12[9];
  unsigned short exit_flags;
  unsigned short ppsd_aiocb_count;
  unsigned short last_arr_ind;
  unsigned long int aiocb_arr;
};

#endif /* !_BITS_SIGCONTEXT_H  */
