/* Definition for thread-local data handling.  NPTL / z/OS version.
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

#ifndef _TLS_H
#define _TLS_H	1

#include <dl-sysdep.h>
#ifndef __ASSEMBLER__
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdlib.h>
# include <list.h>
# include <kernel-features.h>
# include <dl-dtv.h>

typedef struct
{
  void *tcb;		/* Pointer to the TCB.  Not necessary the
			   thread descriptor used by libpthread.  */
  dtv_t *dtv;
  void *self;		/* Pointer to the thread descriptor.  */
  int multiple_threads;
  uintptr_t sysinfo;
  uintptr_t stack_guard;
  int gscope_flag;
  int __glibc_reserved1;
  /* GCC split stack support.  */
  void *__private_ss;
} tcbhead_t;
#else /* !__ASSEMBLER__ */
# include <tcb-offsets.h>
#endif


/* Alignment requirement for the stack.  For IA-32 this is governed by
   the SSE memory functions.  */
#define STACK_ALIGN	16

#ifndef __ASSEMBLER__

/* The z/OS ABI does not give us any register suitable for storing a
   thread pointer, and there are no suitable system calls we could use
   to the same effect, so we aren't left with any good way to handle
   thread pointers other than storing them in a data structure. This
   will be punishingly slow compared to more common thread pointer access
   methods, and has some serious limitations, but it should work, at
   least until a more sane solution becomes apparent. We use a lock-free
   hash table right now.  */
#include <lock-free.h>

/* Because it uses a global data structure, __zos_get_thread_pointer
   must only be defined once. However, libc, libpthread, and rtld all
   use it. For some reason, glibc chooses to make all of its components
   linkable separately, even though in practice nothing will work without
   libc.

   This means that if __zos_get_thread_pointer and company are defined
   normally in those libs, they will fail to link. We choose to solve
   this problem by making those symbols weak in all libs except rtld for
   dynamic programs. Since it's already the case that anything using TLS
   will fail without the libc startup code, which generally depends on
   the rest of libc, it shouldn't break too many things.

   z/OS TODO: IMPORTANT: Reevaluate this, over and over. Could define
   __zos_get_thread_pointer in crt1.o (and company) for minimal
   likelyhood of breakage. Or just put it in libc and change the rtld
   build process to allow undefined symbols. Generally it's best for
   ld.so to be as small as possible.  */
#if defined (SHARED) && !IS_IN (rtld)
# define attribute_tls_maybe_weak __attribute__ ((weak))
#else
# define attribute_tls_maybe_weak
#endif

extern void *__zos_get_thread_pointer (void) attribute_tls_maybe_weak;
extern void __zos_set_thread_pointer (void *addr)
  attribute_tls_maybe_weak;
/* z/OS TODO: Do we need an explicit clear function? We could make it so
   put 0 removes the entry.  */
extern void __zos_clear_thread_pointer (void) attribute_tls_maybe_weak;
extern void __zos_initialize_thread_pointer (void *addr)
  attribute_tls_maybe_weak;
extern void __zos_cleanup_thread_pointer (void *addr)
  attribute_tls_maybe_weak;

/* Something used by the thread pointer mechanism.  */
extern lf_hash_table *__zos_tp_table;
rtld_hidden_proto (__zos_tp_table);

/* Get system call information.  */
# include <sysdep.h>

/* This is the size of the initial TCB.  Can't be just sizeof (tcbhead_t),
   because NPTL getpid, __libc_alloca_cutoff etc. need (almost) the whole
   struct pthread even when not linked with -lpthread.  */
# define TLS_INIT_TCB_SIZE sizeof (struct pthread)

/* Alignment requirements for the initial TCB.  */
# define TLS_INIT_TCB_ALIGN __alignof__ (struct pthread)

/* This is the size of the TCB.  */
# define TLS_TCB_SIZE sizeof (struct pthread)

/* Alignment requirements for the TCB.  */
# define TLS_TCB_ALIGN __alignof__ (struct pthread)

/* The TCB can have any size and the memory following the address the
   thread pointer points to is unspecified.  Allocate the TCB there.  */
# define TLS_TCB_AT_TP	1
# define TLS_DTV_AT_TP	0

/* Get the thread descriptor definition.  */
# include <nptl/descr.h>


/* Install the dtv pointer.  The pointer passed is to the element with
   index -1 which contain the length.  */
#  define INSTALL_DTV(descr, dtvp) \
  ((tcbhead_t *) (descr))->dtv = (dtvp) + 1

/* Install new dtv for current thread.  */
#  define INSTALL_NEW_DTV(dtv) \
  (((tcbhead_t *) __zos_get_thread_pointer ())->dtv = (dtv))

/* Return dtv of given thread descriptor.  */
#  define GET_DTV(descr) \
  (((tcbhead_t *) (descr))->dtv)

#if defined NEED_DL_SYSINFO && defined SHARED
# define INIT_SYSINFO \
  _head->sysinfo = GLRO(dl_sysinfo)
#else
# define INIT_SYSINFO
#endif

/* Code to initially initialize the thread pointer.  This might need
   special attention since 'errno' is not yet available and if the
   operation can cause a failure 'errno' must not be touched.  */
# define TLS_INIT_TP(thrdescr)						\
  ({ void *_thrdescr = (thrdescr);					\
     tcbhead_t *_head = _thrdescr;					\
									\
     _head->tcb = _thrdescr;						\
     /* For now the thread descriptor is at the same address.  */	\
     _head->self = _thrdescr;						\
     /* New syscall handling support.  */				\
     INIT_SYSINFO;							\
									\
     __zos_initialize_thread_pointer (_thrdescr);			\
     NULL;								\
  })

/* Value passed to 'clone' for initialization of the thread register.  */
# define TLS_DEFINE_INIT_TP(tp, pd) void *tp = (pd)

/* Return the address of the dtv for the current thread.  */
#  define THREAD_DTV() \
  (((tcbhead_t *) __zos_get_thread_pointer ())->dtv)

/* Return the thread descriptor for the current thread.  */
# define THREAD_SELF ((struct pthread *) __zos_get_thread_pointer ())

/* Magic for libthread_db to know how to do THREAD_SELF.
   TODO: This.  */
# define DB_THREAD_SELF REGISTER (32, 32, 18 * 4, 0) \
			REGISTER (64, __WORDSIZE, 18 * 8, 0)

/* Access to data in the thread descriptor is easy.  */
#define THREAD_GETMEM(descr, member) \
  descr->member
#define THREAD_GETMEM_NC(descr, member, idx) \
  descr->member[idx]
#define THREAD_SETMEM(descr, member, value) \
  descr->member = (value)
#define THREAD_SETMEM_NC(descr, member, idx, value) \
  descr->member[idx] = (value)

/* TODO: Everything below this point.  */

/* Set the stack guard field in TCB head.  */
#define THREAD_SET_STACK_GUARD(value) \
  do									      \
   {									      \
     __asm__ __volatile__ ("" : : : "a0", "a1");			      \
     THREAD_SETMEM (THREAD_SELF, header.stack_guard, value);		      \
   }									      \
  while (0)
#define THREAD_COPY_STACK_GUARD(descr) \
  ((descr)->header.stack_guard						      \
   = THREAD_GETMEM (THREAD_SELF, header.stack_guard))

/* s390 doesn't have HP_TIMING_*, so for the time being
   use stack_guard as pointer_guard.  */
#define THREAD_GET_POINTER_GUARD() \
  THREAD_GETMEM (THREAD_SELF, header.stack_guard)
#define THREAD_SET_POINTER_GUARD(value)	((void) (value))
#define THREAD_COPY_POINTER_GUARD(descr)

/* Get and set the global scope generation counter in struct pthread.  */
#define THREAD_GSCOPE_IN_TCB      1
#define THREAD_GSCOPE_FLAG_UNUSED 0
#define THREAD_GSCOPE_FLAG_USED   1
#define THREAD_GSCOPE_FLAG_WAIT   2
#define THREAD_GSCOPE_RESET_FLAG() \
  do									     \
    { int __res								     \
	= atomic_exchange_rel (&THREAD_SELF->header.gscope_flag,	     \
			       THREAD_GSCOPE_FLAG_UNUSED);		     \
      if (__res == THREAD_GSCOPE_FLAG_WAIT)				     \
	lll_futex_wake (&THREAD_SELF->header.gscope_flag, 1, LLL_PRIVATE);   \
    }									     \
  while (0)
#define THREAD_GSCOPE_SET_FLAG() \
  do									     \
    {									     \
      THREAD_SELF->header.gscope_flag = THREAD_GSCOPE_FLAG_USED;	     \
      atomic_write_barrier ();						     \
    }									     \
  while (0)
#define THREAD_GSCOPE_WAIT() \
  GL(dl_wait_lookup_done) ()

#endif /* !__ASSEMBLER__ */

#endif	/* tls.h */
