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

/* Register values for use with non-blockreq
   PT_READ/WRITE_GPR/GPRH/FPR/VR.
   z/OS TODO: These are so many short definitions, they could
   interfere with some innocent programs. Should we hide them
   under __USE_MISC? Rename them?  */
#if 0
#define PT_GPR0		0
#define PT_GPR1		1
#define PT_GPR2		2
#define PT_GPR3		3
#define PT_GPR4		4
#define PT_GPR5		5
#define PT_GPR6		6
#define PT_GPR7		7
#define PT_GPR8		8
#define PT_GPR9		9
#define PT_GPR10	10
#define PT_GPR11	11
#define PT_GPR12	12
#define PT_GPR13	13
#define PT_GPR14	14
#define PT_GPR15	15
#define PT_FPR0		16
#define PT_FPR1		17
#define PT_FPR2		18
#define PT_FPR3		19
#define PT_FPR4		20
#define PT_FPR5		21
#define PT_FPR6		22
#define PT_FPR7		23
#define PT_FPR8		24
#define PT_FPR9		25
#define PT_FPR10	26
#define PT_FPR11	27
#define PT_FPR12	28
#define PT_FPR13	29
#define PT_FPR14	30
#define PT_FPR15	31
#define PT_FPC		32
#define PT_PSW		40
#define PT_PSW0		40
#define PT_PSW1		41
#define PT_CR0		42
#define PT_CR1		43
#define PT_CR2		44
#define PT_CR3		45
#define PT_CR4		46
#define PT_CR5		47
#define PT_CR6		48
#define PT_CR7		49
#define PT_CR8		50
#define PT_CR9		51
#define PT_CR10		52
#define PT_CR11		53
#define PT_CR12		54
#define PT_CR13		55
#define PT_CR14		56
#define PT_CR15		57
#define PT_GPRH0	58
#define PT_GPRH1	59
#define PT_GPRH2	60
#define PT_GPRH3	61
#define PT_GPRH4	62
#define PT_GPRH5	63
#define PT_GPRH6	64
#define PT_GPRH7	65
#define PT_GPRH8	66
#define PT_GPRH9	67
#define PT_GPRH10	68
#define PT_GPRH11	69
#define PT_GPRH12	70
#define PT_GPRH13	71
#define PT_GPRH14	72
#define PT_GPRH15	73
#define PT_VR0		74
#define PT_VR1		75
#define PT_VR2		76
#define PT_VR3		77
#define PT_VR4		78
#define PT_VR5		79
#define PT_VR6		80
#define PT_VR7		81
#define PT_VR8		82
#define PT_VR9		83
#define PT_VR10		84
#define PT_VR11		85
#define PT_VR12		86
#define PT_VR13		87
#define PT_VR14		88
#define PT_VR15		89
#define PT_VR16		90
#define PT_VR17		91
#define PT_VR18		92
#define PT_VR19		93
#define PT_VR20		94
#define PT_VR21		95
#define PT_VR22		96
#define PT_VR23		97
#define PT_VR24		98
#define PT_VR25		99
#define PT_VR26		100
#define PT_VR27		101
#define PT_VR28		102
#define PT_VR29		103
#define PT_VR30		104
#define PT_VR31		105
#endif

/* User area offsets.  */

#define PTUAREA_MINSIG	1
#define PTUAREA_MAXSIG	1024
#define PTUAREA_INTCODE	1025	/* Request for program interrupt code */
#define PTUAREA_ABENDCC	1026	/* Request for abend completion code */
#define PTUAREA_ABENDRC	1027	/* Request for abend reason code */
#define PTUAREA_SIGCODE	1028	/* Request for signal code */
#define PTUAREA_ILC	1029	/* Request for instruction length code */
#define PTUAREA_PRFLAGS	1030	/* Request for process flags */

/* Maximum length that may be required by the requests that take a buffer
   of a given length.  */
#define PTMAXIMUMLENGTH 64000

/* Structure of a single element of the list returned by PT_LDINFO.  */

struct __ptrace_ldinfo
{
  __uint32_t offset_to_next;	/* Offset to the start of the next
				   __ptrace_module_info structure.  */
  __uint32_t unused1;
  __uint32_t text_origin;
  __uint32_t text_size;
  __uint8_t text_subpool;
  __uint8_t text_flags;
#define PTLDTEXTWRITE	0x80	/* Text is writable.  */
#define PTLDTEXTMVS	0x40	/* Text is from an MVS load module.  */
#define PTLDTEXTEXT	0x20	/* Text has multiple extents.  */
  __uint16_t ext_offset;	/* If module has multple extents, the
				   offset from this element to the
				   extent info.  */
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
# define __PTRACE_LDINFO_PATHNAME(ldinfo) ((ldinfo)->pathname)
#else
# define __PTRACE_LDINFO_PATHNAME(ldinfo)		\
  ((char *) ((struct __ptrace_ldinfo *) (ldinfo) + 1))
#endif

/* Structure describing module extents.  */

struct __ptrace_ldinfo_extents
{
  __uint16_t count;
  __uint16_t __reserved;
  __uint32_t text_origin[15];
  __uint32_t text_size[15];
};

/* Structure of a single element of the list returned by PT_THREAD_INFO.
   Note that the only valid fields for dead threads are offset_to_next,
   thread_id, state (for PTRACE_THRD_ST_ACTIVE), kernel (for
   PTRACE_THRD_KERN_PTCREATE), exit_status_low, and exit_status_high.  */

struct __ptrace_thread_info
{
  __uint32_t offset_to_next;	/* Offset to next __ptrace_thread_info
				   structure.  */
  __uint8_t thread_id[8];	/* Thread ID.  */
  __uint8_t __reserved[16];
  __uint32_t state;		/* Thread state flags. */
#define PTRACE_TSTATE_ACTIVE		0x80000000
  /* The only valid information for a dead thread is: PTPTNEXT, PTPTTHID, PTPTSTATEACTIVE=0, PTPTKERNELPTHREAD, PTPTEXITSTATUS */
#define PTRACE_THRD_ST_ASYNC		0x40000000
#define PTRACE_THRD_ST_CANCELPEND 	0x20000000

  __uint32_t kernel;		/* Thread kernel flags. */
#define PTRACE_THRD_KERN_DETACHED	0x80000000	/* Thread in a
							   detached state.  */
#define PTRACE_THRD_KERN_MEDIUM		0x40000000	/* Thread is a
							   mediumweight thread.  */
#define PTRACE_THRD_KERN_ASYNC		0x20000000
#define PTRACE_THRD_KERN_PTCREATE	0x10000000	/* Thread created by
							   pthread_create.  */
#define PTRACE_THRD_KERN_HOLD		0x00800000	/* Thread is held by
							   kernel.  */
  __uint32_t exit_status_low;	/* If thread is dead, exit status.
				   If 64-bit, only contains the low half.  */
  __uint8_t pending_sigmask[8];	/* Currently pending signals.  */
  __uint32_t exit_status_high;	/* High half of exit status.  */
  __uint32_t __reserved2;
};

/* z/OS TODO: This and the regular thread info structures
   may be compatible? Should they be the same structure?  */

struct __ptrace_thread_info_ext
{
  __uint32_t offset_to_next;	/* Offset to next element */
  __uint8_t thread_id[8];
  __uint32_t tcb;
  __uint32_t otcb;
  __uint8_t blocked_sigmask[8];
  __uint32_t state;
  __uint32_t kernel;
  __uint32_t exit_status_low;
  __uint8_t pending_sigmask[8];
  __uint32_t pid;
  __uint16_t asid;
  __uint16_t flags;
#define PTRACE_THRD_EXT_IPT		0x8000	/* This thread is the
						   IPT.  */
#define PTRACE_THRD_EXT_INCOMPLETE	0x4000	/* Thread info is
						   incomplete.  */
  __uint32_t oapb;
  __uint32_t exit_status_high;
};

struct __ptrace_process_and_thread_info
{
  char id[4];
  __uint32_t offset_to_next;
  __uint32_t offset_to_thread_info;
  __uint32_t pid;
  __uint8_t pending_sigmask[8];	/* Signals pending at the process */
  __uint8_t blocked_sigmask[8];	/* blocked signals at process */
  __uint32_t total_thread_count;
  __uint32_t current_thread_count;
  __uint32_t thread_info_size;  /* Size of thread_info.  */
  __uint32_t __reserved;
#if __glibc_c99_flexarr_available
  __extension__ struct __ptrace_thread_info_ext thrd_infos __flexarr;
#endif
};

/* not sure this should be used, because maybe you are supposed to use the offsets */
#if __glibc_c99_flexarr_available
# define __PTRACE_THREAD_INFO_EXTENDED(process_and_thread_info) \
  ((process_and_thread_info)->thrd_infos)
#else
# define __PTRACE_THREAD_INFO_EXTENDED(process_and_thread_info)		\
  ((struct __ptrace_thread_info_ext *)					\
   ((struct __ptrace_process_and_thread_info *) (process_and_thread_info) + 1))
#endif

struct __ptrace_explain_info
{
  /* registers at entry to CEEEVDBG */
  __uint32_t r1;
  __uint32_t r12;
  __uint32_t r13;
  __uint32_t reserved1;
  __uint64_t r1_64;
  __uint64_t r12_64;
  __uint64_t r13_64;
};

struct __ptrace_program_recovery_parameters
{
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

/* PT_BLOCKREQ structures.  */

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

/* Structure used by PT_READ_GPR and PT_WRITE_GPR in a PT_BLOCKREQ
   request.  */

struct __ptrace_gpr_blockreq
{
  __uint32_t write_regs;	/* Reg bitmask, only used by PT_WRITE_GPR.  */
#define PTBR_GPR_WGPR0	0x80000000	/* Write GPR0.  */
#define PTBR_GPR_WGPR1	0x40000000	/* Write GPR1.  */
#define PTBR_GPR_WGPR2	0x20000000	/* Write GPR2.  */
#define PTBR_GPR_WGPR3	0x10000000	/* Write GPR3.  */
#define PTBR_GPR_WGPR4	0x08000000	/* Write GPR4.  */
#define PTBR_GPR_WGPR5	0x04000000	/* Write GPR5.  */
#define PTBR_GPR_WGPR6	0x02000000	/* Write GPR6.  */
#define PTBR_GPR_WGPR7	0x01000000	/* Write GPR7.  */
#define PTBR_GPR_WGPR8	0x00800000	/* Write GPR8.  */
#define PTBR_GPR_WGPR9	0x00400000	/* Write GPR9.  */
#define PTBR_GPR_WGPR10	0x00200000	/* Write GPR10.  */
#define PTBR_GPR_WGPR11	0x00100000	/* Write GPR11.  */
#define PTBR_GPR_WGPR12	0x00080000	/* Write GPR12.  */
#define PTBR_GPR_WGPR13	0x00040000	/* Write GPR13.  */
#define PTBR_GPR_WGPR14	0x00020000	/* Write GPR14.  */
#define PTBR_GPR_WGPR15	0x00010000	/* Write GPR15.  */
#define PTBR_GPR_WPSW	0x00008000	/* Write PSW right half.  */
  __uint8_t __reserved[12];
  __uint32_t gprs[16];		/* GPR low half values.  */
  __uint32_t crs[16];		/* CR values.  */
  __uint32_t psw_mask;		/* PSW first half.  */
  __uint32_t psw_addr;		/* PSW second half. Only this part can
				   be written.  */
};

/* Structure used for PT_READ_GPRH and PT_WRITE_GPRH in a PT_BLOCKREQ
   request.  */

struct __ptrace_gprh_blockreq
{
  __uint32_t write_regs;	/* Reg bitmask, only used by PT_WRITE_GPRH.
				   Uses same flags as __ptrace_gpr_req,
				   except for the PSW bit.  */
  __uint8_t __reserved[12];
  __uint32_t gprs[16];		/* GPR high half values.  */
};

/* Structure used for PT_READ_FPR and PT_WRITE_FPR in a PT_BLOCKREQ
   request.  */

struct __ptrace_fpr_blockreq
{
  __uint32_t write_regs;	/* Reg bitmask, only used by PT_WRITE_FPR.
				   Uses same flags as __ptrace_gpr_req,
				   except for the PSW bit.  */
#define PTBR_FPR_WFPR0	0x80000000	/* Write FPR0.  */
#define PTBR_FPR_WFPR1	0x40000000	/* Write FPR1.  */
#define PTBR_FPR_WFPR2	0x20000000	/* Write FPR2.  */
#define PTBR_FPR_WFPR3	0x10000000	/* Write FPR3.  */
#define PTBR_FPR_WFPR4	0x08000000	/* Write FPR4.  */
#define PTBR_FPR_WFPR5	0x04000000	/* Write FPR5.  */
#define PTBR_FPR_WFPR6	0x02000000	/* Write FPR6.  */
#define PTBR_FPR_WFPR7	0x01000000	/* Write FPR7.  */
#define PTBR_FPR_WFPR8	0x00800000	/* Write FPR8.  */
#define PTBR_FPR_WFPR9	0x00400000	/* Write FPR9.  */
#define PTBR_FPR_WFPR10	0x00200000	/* Write FPR10.  */
#define PTBR_FPR_WFPR11	0x00100000	/* Write FPR11.  */
#define PTBR_FPR_WFPR12	0x00080000	/* Write FPR12.  */
#define PTBR_FPR_WFPR13	0x00040000	/* Write FPR13.  */
#define PTBR_FPR_WFPR14	0x00020000	/* Write FPR14.  */
#define PTBR_FPR_WFPR15	0x00010000	/* Write FPR15.  */
#define PTBR_FPR_WFPC	0x00008000	/* Write FPC.  */
  __uint8_t __reserved[12];
  double fprs[16];	/* FPR.  */
  __uint32_t fpc;	/*Floating Point Control Register */
};

/* Structure used for PT_READ_BLOCK and PT_WRITE_BLOCK in a PT_BLOCKREQ
   request when debugging 31-bit processes. */

struct __ptrace_block32_blockreq
{
  __uint32_t address;
  __uint32_t length;
  __uint8_t __reserved[8];
#if __glibc_c99_flexarr_available
  __extension__ char data __flexarr;	/* data area.  */
#endif
};

/* Read/write block 32-bit PT_BLOCKREQ data manipulation macros.  */
#if __glibc_c99_flexarr_available
# define __PTRACE_BLOCK32_DATA(block32) ((block32)->data)
#else
# define __PTRACE_BLOCK32_DATA(block32)					\
  ((char *) ((struct __ptrace_block32_blockreq *) (block32) + 1))
#endif

/* Structure used for PT_READ_BLOCK and PT_WRITE_BLOCK in a PT_BLOCKREQ
   request when debugging 64-bit processes. */

struct __ptrace_block64_blockreq
{
  __uint64_t address;
  __uint32_t length;
  __uint8_t __reserved[4];
#if __glibc_c99_flexarr_available
  __extension__ char data __flexarr;	/* data area.  */
#endif
};

/* Read/write block 64-bit PT_BLOCKREQ data manipulation macros.  */
#if __glibc_c99_flexarr_available
# define __PTRACE_BLOCK64_DATA(rw_block64_rq) ((block64)->data)
#else
# define __PTRACE_BLOCK64_DATA(rw_block64_rq)		\
  ((char *) ((struct __ptrace_block64_blockreq *) (block64) + 1))
#endif

/* Structure used for PT_READ_D, PT_WRITE_D, PT_READ_I, and PT_WRITE_I
   in a PT_BLOCKREQ request when debugging 31-bit processes. */

struct __ptrace_data32_blockreq
{
  __uint32_t address;
  __uint32_t data;
};

/* Structure used for PT_READ_D, PT_WRITE_D, PT_READ_I, and PT_WRITE_I
   in a PT_BLOCKREQ request when debugging 64-bit processes. */

struct __ptrace_data64_blockreq
{
  __uint64_t address;
  __uint32_t data;
};

struct __ptrace_read_u_field
{
  __uint32_t offset;	/* Input: offset into the user area.  */
  __uint32_t data;	/* Output: word at offset.  */
};

/* Structure used for PT_READ_U in a PT_BLOCKREQ request. */

struct __ptrace_read_u_blockreq
{
  __uint32_t count;
  __uint32_t __reserved;
  #if __glibc_c99_flexarr_available
  __extension__ struct __ptrace_read_u_field data  __flexarr;
#endif
};

/* read_user_fields list manipulation macros.  */
#if __glibc_c99_flexarr_available
# define __PTRACE_READ_U_DATA(read_user_rq) ((read_user_rq)->fields)
#else
# define __PTRACE_READ_U_DATA(read_user_rq)				\
  ((struct __ptrace_read_u_field *)					\
   ((struct __ptrace_read_u_blockreq *) (read_user_rq) + 1))
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
