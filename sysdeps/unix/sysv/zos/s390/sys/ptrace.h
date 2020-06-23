/* `ptrace' debugger support interface.  z/OS version.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _SYS_PTRACE_H
#define _SYS_PTRACE_H	1

#include <features.h>
#include <bits/types.h>

__BEGIN_DECLS

/* Type of the REQUEST argument to `ptrace.'  */
enum __ptrace_request
{
  /* Indicate that the process making this request should be traced.
     All signals received by this process can be intercepted by its
     parent, and its parent can use the other `ptrace' requests.  */
  PTRACE_TRACEME = 0,
#define PT_TRACE_ME PTRACE_TRACEME

  /* Return the word in the process's text space at address ADDR.  */
  PTRACE_PEEKTEXT = 1,
#define PT_READ_I PTRACE_PEEKTEXT

  /* Return the word in the process's data space at address ADDR.  */
  PTRACE_PEEKDATA = 2,
#define PT_READ_D PTRACE_PEEKDATA

  /* Return the word in the process's user area at offset ADDR.  */
  PTRACE_PEEKUSER = 3,
#define PT_READ_U PTRACE_PEEKUSER

  /* Write the word DATA into the process's text space at address ADDR.  */
  PTRACE_POKETEXT = 4,
#define PT_WRITE_I PTRACE_POKETEXT

  /* Write the word DATA into the process's data space at address ADDR.  */
  PTRACE_POKEDATA = 5,
#define PT_WRITE_D PTRACE_POKEDATA

  /* We don't have PTRACE_POKEUSER.  */

  /* Continue the process.  */
  PTRACE_CONT = 7,
#define PT_CONTINUE PTRACE_CONT

  /* Kill the process.  */
  PTRACE_KILL = 8,
#define PT_KILL PTRACE_KILL

  /* Return the contents of a GPR, CR, or the PSW.  */
  PT_READ_GPR = 11,

  /* Return the contents of an FPR.  */
  PT_READ_FPR = 12,

  /* Return the contents of a vector register (16-bytes).  */
  PT_READ_VR = 13,

  /* Set the contents of a GPR, CR, or the PSW.  */
  PT_WRITE_GPR = 14,

  /* Set the contents of an FPR.  */
  PT_WRITE_FPR = 15,

  /* Set the contents of a vector register (16-bytes).  */
  PT_WRITE_VR = 16,

  /* Read a region of memory.  */
  PT_READ_BLOCK = 17,

  /* Write into a region or memory.  */
  PT_WRITE_BLOCK = 19,

  /* Read the high half of a GPR.  */
  PT_READ_GPRH = 20,

  /* Write into the high half of a GPR.  */
  PT_WRITE_GPRH = 21,

  /* Read the high halves of all GPRs.  */
  PT_REGHSET = 22,

  /* Attach to a process that is already running. */
  PTRACE_ATTACH = 30,
#define PT_ATTACH PTRACE_ATTACH

  /* Detach from a process attached to with PTRACE_ATTACH.  */
  PTRACE_DETACH = 31,
#define PT_DETACH PTRACE_DETACH

  /* Get all general purpose registers of a processes.  */
  PT_REGSET = 32,

  PT_REATTACH = 33,  /* DEPRECATED. Use PT_REATTACH2 instead.  */

  /* Obtain information about any modules loaded into the target
     process.  */
  PT_LDINFO = 34,

  /* Turn on or off multiprocess debugging mode.  */
  PT_MULTI = 35,

  /* Execute a list of multiple ptrace requests.  */
  PT_BLOCKREQ = 40,

  /* Examine thread info.  */
  PT_THREAD_INFO = 60,

  /* Modify kernel-level thread info.  */
  PT_THREAD_MODIFY = 61,

  /* Read the thread ID of the thread currently being examined, which
     is referred to as the current focus thread.  */
  PT_THREAD_READ_FOCUS = 62,

  /* Change the thread currently being examined to the thread
     with the given ID.  */
  PT_THREAD_WRITE_FOCUS = 63,

  /* Put the given thread into a held (suspended) state, or release
     it from one.  */
  PT_THREAD_HOLD = 64,

  /* Queue a signal to be delivered to a specific thread in the traced
     process.  */
  PT_THREAD_SIGNAL = 65,

  /* Retrieve information about an exteded event.  */
  PT_EXPLAIN = 66,

  /* Either enable or disable reporting for a ptrace extended event
     type.  */
  PT_EVENTS = 67,

  /* Attach to a process currently being debugged by another process,
     stealing it from the current tracing process. Saves additional
     info into a user-provided buffer to indicate whether the target
     process is the product of a local fork (i.e. it was created by
     attach_exec, attach_execmvs, or any similar facility), and is
     running in the same address space as its parent. If it is the
     product of a local fork, some ptrace operations cannot be used.  */
  PT_REATTACH2 = 71,

  /* Capture pages of memory from the target process into a
     buffer. This effectively turns the buffer into a sort of
     shared memory region, where changes made in one process are visible
     in the other. The target address must be on a 4K boundary.  */
  PT_CAPTURE = 72,

  /* Uncapture memory previously captured with PT_CAPTURE.  */
  PT_UNCAPTURE = 73,

  /* Get the address of the thread's TCB.  */
  PT_GET_THREAD_TCB = 74,

  /* Get the target PID's ALET.  */
  PT_GET_ALET = 75,

  /* Cause the target PID's address space to be swapped in.  */
  PT_SWAPIN = 76,

  /* From within a process, notify any external tracing processes of
     an event that may be of interest. The nature of the event is
     completely defined by the application or environment.  */
  PT_EXTENDED_EVENT = 98,

  /* From within a process, notify any external tracing processes of
     a program check or abnormal end (abend). This is expected to be
     issued unconditionally from the applciation process's recovery
     routine. The return information indicates whether the process is
     being debugged, and if so, whether the debugger process modified
     the program's PSW or registers.  */
  PT_RECOVER = 99,
};

/* Structure used by PT_READ_GPR and PT_WRITE_GPR.  */

struct __ptrace_gpr_req
{
  __uint32_t write_regs;	/* Reg bitmask, only used by PT_WRITE_GPR.  */
#define PT_GPR_WGPR0	0x80000000	/* Write GPR0.  */
#define PT_GPR_WGPR1	0x40000000	/* Write GPR1.  */
#define PT_GPR_WGPR2	0x20000000	/* Write GPR2.  */
#define PT_GPR_WGPR3	0x10000000	/* Write GPR3.  */
#define PT_GPR_WGPR4	0x08000000	/* Write GPR4.  */
#define PT_GPR_WGPR5	0x04000000	/* Write GPR5.  */
#define PT_GPR_WGPR6	0x02000000	/* Write GPR6.  */
#define PT_GPR_WGPR7	0x01000000	/* Write GPR7.  */
#define PT_GPR_WGPR8	0x00800000	/* Write GPR8.  */
#define PT_GPR_WGPR9	0x00400000	/* Write GPR9.  */
#define PT_GPR_WGPR10	0x00200000	/* Write GPR10.  */
#define PT_GPR_WGPR11	0x00100000	/* Write GPR11.  */
#define PT_GPR_WGPR12	0x00080000	/* Write GPR12.  */
#define PT_GPR_WGPR13	0x00040000	/* Write GPR13.  */
#define PT_GPR_WGPR14	0x00020000	/* Write GPR14.  */
#define PT_GPR_WGPR15	0x00010000	/* Write GPR15.  */
#define PT_GPR_WPSW	0x00008000	/* Write PSW right half.  */
  __uint32_t gprs[16];		/* GPR low half values.  */
  __uint32_t crs[16];		/* CR values.  */
  __uint32_t psw_mask;		/* PSW first half.  */
  __uint32_t psw_addr;		/* PSW second half. Only this part can
				   be written.  */
};

/* Structure used for PT_READ_GPRH and PT_WRITE_GPRH.  */

struct __ptrace_gprh_req
{
  __uint32_t write_regs;	/* Reg bitmask, only used by PT_WRITE_GPRH.
				   Uses same flags as __ptrace_gpr_req,
				   except for the PSW bit.  */
  __uint32_t gprs[16];		/* GPR high half values.  */
};

/* Structure used for a single ptrace request in a PT_BLOCKREQ
   request.  */

struct __ptrace_blockreq_elem
{
  __int32_t type;	/* Input: ptrace request type.  */
  __int32_t status;	/* Output: error code for request.  */
  __uint32_t req_blk_off;	/* Offset to request-specific data area.
				   Note that this is an offset in bytes
				   from the start of the parent
				   __ptrace_blockreq structure.  */
  __uint8_t __reserved[4];
};

/* Structure used by PT_BLOCKREQ. Allows the user to perform a list
   of ptrace operations in order with a single call.  */

struct __ptrace_blockreq
{
  __int32_t num_reqs;		/* Number of requests.  */
  __uint8_t __reserved[12];
#if __glibc_c99_flexarr_available
  __extension__ struct __ptrace_blockreq_elem requests __flexarr; /* Request list.  */
#endif
};

/* Blockreq request list manipulation macros.  */
#if __glibc_c99_flexarr_available
# define __PTRACE_BLOCKREQ_REQUESTS(blkrq) ((blkrq)->requests)
#else
# define __PTRACE_BLOCKREQ_REQUESTS(blkrq)				\
  ((struct __ptrace_blockreq_elem *)					\
   ((struct __ptrace_blockreq *) (blkrq) + 1))
#endif

/* Perform process tracing functions.  REQUEST is one of the values
   above, and determines the action to be taken.
   For all requests except PTRACE_TRACEME, PID specifies the process to be
   traced.

   PID and the other arguments described above for the various requests should
   appear (those that are used for the particular request) as:
   pid_t PID, void *ADDR, int DATA, void *ADDR2
   after REQUEST.  */
extern long int ptrace (enum __ptrace_request __request, ...) __THROW;

__END_DECLS

#endif /* _SYS_PTRACE_H */
