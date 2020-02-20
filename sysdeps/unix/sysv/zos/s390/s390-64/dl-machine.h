/* Machine-dependent ELF dynamic relocation inline functions.
   64 bit z/OS Version.
   Copyright (C) 2019 Free Software Foundation, Inc.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.
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

/* We can use almost everything from z/Linux except for the
   entry point.  */

#define elf_machine_load_address dummy_s390_load_address
#include <sysdeps/s390/s390-64/dl-machine.h>
#undef elf_machine_load_address

#ifndef _ZOS_DL_MACHINE_H
#define _ZOS_DL_MACHINE_H 1

#include <zos-init.h>
#include <zos-core.h>

#define _ZOS_DL_MACHINE_STRING1(x) #x
#define _ZOS_DL_MACHINE_STRING(x) _ZOS_DL_MACHINE_STRING1 (x)

/* Return the run-time load address of the shared object.  */
static inline Elf64_Addr
elf_machine_load_address (void)
{
  Elf64_Addr addr;

  __asm__ ("larl	%0, _begin\n\t"
	   : "=&d" (addr));

  return addr;
}

static void __attribute__ ((used, noreturn))
_dl_zos_early_init (void *arg_info, ElfW(Ehdr) * ehdr,
		    void (*cont) (void *) __attribute__ ((noreturn)))
{
  void *cookie = ESSENTIAL_PROC_INIT (alloca, arg_info,
				      (void (*)(void)) NULL, ehdr);
  cont (cookie);
  __builtin_unreachable ();
}

/* We do some trickery with %r1, see the note about it in start.S.
   We also duplicate some code from start.S for stack allocation.  */

#undef RTLD_START
#define RTLD_START __asm__ ("\n\
.text\n\
.align 4\n\
.globl _start\n\
.globl _dl_start_user\n\
_start:\n\
	tmll	%r1, 1\n\
	je	.Lstack_setup\n\
.Lsetup_done:\n\
	# Mark this as the top stack frame\n\
	xc	128(8,%r13), 128(%r13)\n\
	# Alloc stack frame. Just use the current one, but expand it.\n\
	lg	%r15, 136(%r13)\n\
	lgr	%r4, %r15\n\
	# 32 more bytes.\n\
	la	%r15, 32(%r15)\n\
	stg	%r15, 136(%r13)\n\
	# Call our special setup function, but pass a continuation to be\n\
	# called instead of returning, so that we can put our translated\n\
	# args on the stack and avoid another storage allocation.\n\
	# %r2 is either the executable's ehdr or NULL\n\
	larl	%r3, .Lvery_early_init_done\n\
	stmg	%r1, %r3, 0(%r4)\n\
	lgr	%r1, %r4\n\
	brasl	%r14, _dl_zos_early_init\n\
.Lvery_early_init_done:\n\
	# %r1 should now point to a 1 arg arglist pointing to argc,\n\
	# argv, and the envs in the standard format, and mabye an\n\
	# ehdr, without an aux vector.\n\
	lg	%r5, 0(%r1)\n\
	brasl	%r14, _dl_start\n\
_dl_start_user:\n\
	# User's entry point address should now be in %r15\n\
	lgr   	%r8, %r15\n\
	# Point %r12 at the GOT.\n\
	larl	%r12, _GLOBAL_OFFSET_TABLE_\n\
	# See if we were run as a command with the executable file\n\
	# name as an extra leading argument.\n\
	lghi	%r3, _dl_skip_args@GOT\n\
	lg	%r3, 0(%r3,%r12)\n\
	lgf	%r3, 0(%r3)	  # load _dl_skip_args\n\
	# Get the original argument count.\n\
	lgr	%r1, %r5\n\
	lg	%r6, 8(%r1)\n\
	# Subtract _dl_skip_args from it.\n\
	sgr	%r6, %r3\n\
	# Adjust the arg pointer to skip _dl_skip_args words.\n\
	sllg	%r7, %r3, 3\n\
	agr	%r10, %r7\n\
	# Store back the modified argument count.\n\
	stg	%r6, 0(%r10)\n\
	# The special initializer gets called with the stack just\n\
	# as the application's entry point will see it; it can\n\
	# switch stacks if it moves these contents over.\n\
" RTLD_START_SPECIAL_INIT "\n\
	# Call the function to run the initializers.\n\
	# Load the parameters:\n\
	# (%r2, %r3, %r4, %r5) = (_dl_loaded, argc, argv, envp)\n\
	lg	%r1, 136(%r13)\n\
	lghi	%r2, _rtld_local@GOT\n\
	lg	%r2, 0(%r2,%r12)\n\
	lg	%r2, 0(%r2)\n\
	lg	%r3, %r6\n\
	la	%r4, 8(%r10)\n\
	lgr	%r5, %r3\n\
	sllg	%r5, %r5, 3\n\
	la	%r5, 16(%r5,%r10)\n\
	la	%r7, 32(%r1)\n\
	stg	%r7, 136(%r13)\n\
	stmg	%r2, %r5, 0(%r1)\n\
	brasl	%r14, _dl_init@PLT\n\
	# Pass our finalizer function to the user in %r4.\n\
	lghi	%r4, _dl_fini@GOT\n\
	lg	%r4, 0(%r4,%r12)\n\
	# Load the real arguments and shrink stack frame\n\
	lg	%r2, 136(%r13)\n\
	aghi	%r2, -32\n\
	stg	%r2, 136(%r13)\n\
	lgr	%r1, %r10\n\
	# Jump to the user's entry point (saved in %r8).\n\
	br	%r15\n\
.Lstack_setup:\n\
	stmg    %r14, %r12, 8(%r13)\n\
	# Record that %r2 doesn't contain our ehdr since we jumped.\n\
	lghi	%r2, 0\n\
	# Set up a stack\n\
	lgr	%r6, %r1\n\
	larl	%r15, .Lmain_stack_length\n\
	l	%r0, 0(%r15)\n\
	l	%r15, .Lmain_stack_flags-.Lmain_stack_length(%r15)\n\
	llgt	%r14, 16\n\
	l	%r14, 772(%r14)\n\
	l	%r14, 160(%r14)\n\
	pc	0(%r14)\n\
	aghi	%r1, 15\n\
	nill	%r1, 65520\n\
	stg	%r1, 136(%r13)\n\
	stg	%r13, 128(%r1)\n\
	lgr	%r13, %r1\n\
	llilh	%r14, 50932\n\
	oill	%r14, 58049\n\
	st	%r14, 4(%r13)\n\
	la	%r15, 160(%r13)\n\
	stg	%r15, 136(%r13)\n\
	lgr	%r1, %r6\n\
	j	.Lsetup_done\n\
.Lmain_stack_length:\n\
	.long 10 * (1 << 20)\n\
.Lmain_stack_flags:\n\
	.long " _ZOS_DL_MACHINE_STRING (REGULAR_OBTAIN_FLAGS) "\n\
");

#endif /* !_ZOS_DL_MACHINE_H  */
