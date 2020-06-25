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
  __uint8_t reserved[12];
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
  __uint8_t reserved[12];
  __uint32_t gprs[16];		/* GPR high half values.  */
};

struct __ptrace_fpr_req {
  __uint32_t write_regs;	/* Reg bitmask, only used by PT_WRITE_FPR.
				   Uses same flags as __ptrace_gpr_req,
				   except for the PSW bit.  */
  #define PT_FPR_WFPC	0x00008000	/* Write FPC.  */
  __uint8_t reserved[12];
  double fprs[16];		/* FPR.  */
  __uint32_t PTBR_FPR_FPC; /*Floating Point Control Register */
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

/* Structure for PT_Read_Block and PT_Write_Block. */
struct __ptrace_rw_block {
  __uint32_t address;
  __uint32_t length;
  __uint8_t reserved1[8];
#if __glibc_c99_flexarr_available
  __extension__ __uint8_t data __flexarr; /* data area.  */
#endif
};

/* rw_block request list manipulation macros.  */
#if __glibc_c99_flexarr_available
# define __PTRACE_RW_BLOCK_DATA(rw_block_rq) ((rw_block_rq)->data)
#else
# define __PTRACE_RW_BLOCK_DATA(rw_block_rq)		\
  ((__uint8_t *)					\
   ((struct __ptrace_rw_block *) (rw_block_rq) + 1))
#endif

struct __ptrace_rw_block64 {
  __uint64_t address;
  __uint32_t length;
  __uint8_t reserved1[4];
#if __glibc_c99_flexarr_available
  __extension__ __uint8_t data __flexarr; /* data area.  */
#endif
};

/* rw_block request list manipulation macros.  */
#if __glibc_c99_flexarr_available
# define __PTRACE_RW_BLOCK64_DATA(rw_block64_rq) ((rw_block64_rq)->data)
#else
# define __PTRACE_RW_BLOCK64_DATA(rw_block64_rq)	\
  ((__uint8_t *)					\
   ((struct __ptrace_rw_block64 *) (rw_block64_rq) + 1))
#endif

struct __ptrace_rw32_data {
  __uint32_t address;
  __uint32_t data;
};

struct __ptrace_rw32_data64 {
  __uint64_t address;
  __uint32_t data;
};

struct __ptrace_rw32_instructions {
  __uint32_t address;
  __uint32_t data;
};

struct __ptrace_rw32_instructions64 {
  __uint64_t address;
  __uint32_t data;
};

#define PTUAREA_MINSIG  1
#define PTUAREA_MAXSIG  1024
#define PTUAREA_INTCODE  1025 /* Request for program interrupt code */
#define PTUAREA_ABENDCC  1026 /* Request for abend completion code */
#define PTUAREA_ABENDRC  1027 /* Request for abend reason code */
#define PTUAREA_SIGCODE  1028 /* Request for signal code */
#define PTUAREA_ILC  1029 /* Request for instruction length code */
#define PTUAREA_PRFLAGS  1030 /* Request for process flags */
struct __ptrace_read_user_field {
  __uint32_t offset;
  __uint32_t data;
};

struct __ptrace_read_user_fields {
  __uint32_t count;
  __uint32_t reserved1;
  #if __glibc_c99_flexarr_available
  __extension__ struct __ptrace_read_user_field fields __flexarr;
#endif
};

/* read_user_fields list manipulation macros.  */
#if __glibc_c99_flexarr_available
# define __PTRACE_READ_USER_DATA(read_user_fields_rq) ((read_user_fields_rq)->fields)
#else
# define __PTRACE_READ_USER_DATA(read_user_fields_rq)	\
  ((struct __ptrace_read_user_field *)					\
   ((struct __ptrace_read_user_fields *) (read_user_fields_rq) + 1))
#endif

struct __ptrace_loaded_module_info {
  __uint32_t offset_to_next;
  __uint32_t unused1;
  __uint32_t text_origin;
  __uint32_t text_size;
  __uint8_t text_subpool;
  __uint8_t text_flags;
#define TEXT_IS_WRITABLE 0x80
#define TEXT_IS_FROM_MVS 0x40
#define TEXT_HAS_MORE_THAN_ONE_EXTENT 0x20
  __uint16_t offset_to_extension; 
  __uint32_t unused2;
  __uint32_t unused3;
  __uint8_t unused4;
  __uint8_t unused5;
  __uint16_t reserved;
#if __glibc_c99_flexarr_available
  __extension__ char pathname __flexarr;
#endif
};

#if __glibc_c99_flexarr_available
# define __PTRACE_LOADED_MODULE_PATHNAME(lminfo_rq) ((lminfo_rq)->pathname)
#else
# define __PTRACE_RW_BLOCK_DATA(lminfo_rq)	\
  ((char *)					\
   ((struct __ptrace_read_user_fields *) (lminfo_rq) + 1))
#endif


struct __ptrace_loaded_module_info_text_extents {
  __uint16_t count;
  __uint16_t reserved1; /* reserved */
  __uint32_t text_origin[15];
  __uint32_t text_size[15];
};

struct __ptrace_thread_info { /* PTPT */
  __uint32_t offset_to_next;
  __uint8_t thread_id[8];
  __uint8_t reserved[16];
  uint8_t blocked_signal_mask[8];
  __uint32_t state; /* Thread state flag byte */
#define THREAD_IS_ACTIVE 0x80000000
  /* The only valid information for a dead thread is: PTPTNEXT, PTPTTHID, PTPTSTATEACTIVE=0, PTPTKERNELPTHREAD, PTPTEXITSTATUS */
#define THREAD_IS_ASYNCHRONOUS 0x40000000 
#define THREAD_HAS_CANCEL_PENDING 0x20000000
  __uint32_t kernel; /* Thread kernel attribute byte */
#define THREAD_KERNEL_IS_DETACHED 0x80000000
#define THREAD_KERNEL_IS_MEDIUMWEIGHT 0x40000000
#define THREAD_KERNEL_IS_ASYNCHRONOUS 0x20000000
#define THREAD_KERNEL_WAS_CREATED_BY_PTHREAD_CREATE 0x10000000
#define THREAD_KERNEL_IS_HELD 0x00800000 
  __uint32_t exit_status;
  __uint8_t pending_signal_mask[8];
  __uint32_t exit_status_high;
  __uint32_t reserved2;
};

struct __ptrace_thread_info_extended {
  __uint32_t offset_to_next; /* Offset to next element */
  __uint8_t thread_id[8];
  __uint32_t tcb;
  __uint32_t otcb;
  __uint8_t blocked_signal_mask[8];
  __uint32_t state;
  __uint32_t kernel;
  __uint32_t exit_status;
  __uint8_t pending_signal_mask[8];
  __uint32_t pid;
  __uint16_t asid;
  __uint16_t flags;
#define THREAD_IS_IPT 0x8000
#define THREAD_INFO_IS_INCOMPLETE0 x4000
  __uint32_t oapb;
  __uint32_t exit_status_high;
};

struct __ptrace_process_and_thread_info {
  char id[4];
  __uint32_t offset_to_next;
  __uint32_t offset_to_thread_info;
  __uint32_t pid;
  __uint8_t pending_signal_mask[8]; /* Signals pending at the process */
  __uint8_t blocked_signal_mask[8]; /* blocked signals at process */
  __uint32_t total_thread_count;
  __uint32_t current_thread_count;
  __uint32_t size_of_thread_info_extended;
  __uint32_t reserved;
#if __glibc_c99_flexarr_available
  __extension__ struct __ptrace_thread_info_extended thread_info_array __flexarr;
#endif
};

/* not sure this should be used, because maybe you are supposed to use the offsets */
#if __glibc_c99_flexarr_available
# define __PTRACE_THREAD_INFO_EXTENDED(process_and_thread_info) ((lminfo_rq)->thread_info_array)
#else
# define __PTRACE_RW_BLOCK_DATA(process_and_thread_info)	\
  ((struct __ptrace_thread_info_extended *)					\
   ((struct __ptrace_process_and_thread_info *) (process_and_thread_info) + 1))
#endif

struct __ptrace_explain_info { /* registers at entry to CEEEVDBG */
  __uint32_t r1;
  __uint32_t r12;
  __uint32_t r13;
  __uint32_t reserved1;
  __uint64_t r1_64;
  __uint64_t r12_64;
  __uint64_t r13_64;
};

struct __ptrace_program_recovery_parameters {
  __uint32_t address_of_registers;
  __uint32_t address_of_psw;
  __uint16_t program_interrupt_code;
  __uint16_t signal_number_to_raise;
  __uint32_t flags;
#define PTPR_INSTRUCTION_COUNTER_HAS_BEEN_MODIFIED 0x80000000
#define PTPR_REGISTERS_HAVE_BEEN_MODIFIED 0x40000000
#define PTPR_RAISE_SIGNAL 0x20000000
#define PTPR_BYPASS_SIGNAL 0x10000000
#define PTPR_INSTRUCTION_LENGTH_CODE_EXISTS 0x08000000
#define PTPR_HIGH_REGISTERS_EXIST 0x04000000
#define PTPR_HIGH_REGISTERS_HAVE_BEEN_MODIFIED 0x02000000
#define PTPI_USE_64BITS_FOR_PSW_AND_REGISTERS 0x01000000
  __uint8_t abend_code;
  __uint8_t abend_completion_code[3];
  __uint32_t abend_reason_code;
  __uint8_t instruction_length_code;
  __uint8_t reserved[3];
  __uint32_t address_of_high_registers;
  __uint64_t address_of_registers64;
  __uint64_t address_of_psw64;
  __uint64_t address_of_high_registers64;
  __uint8_t reserved2[8];
};

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
