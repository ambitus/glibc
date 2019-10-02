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

#include <sysdeps/s390/s390-64/dl-machine.h>

#ifndef _ZOS_DL_MACHINE_H
#define _ZOS_DL_MACHINE_H 1

#include <zos-init.h>

static void __attribute_used__ __attribute__ ((noreturn))
_dl_zos_early_init (void *arg_info,
		    void (*cont) (void *) __attribute__ ((noreturn)))
{
  void *cookie = ESSENTIAL_PROC_INIT (alloca, arg_info, NULL);
  cont (cookie);
  __builtin_unreachable ();
}

/* We do some trickery with %r1, see the note about it in start.S.
   We duplicate some code from start.S for stack allocation.  */

#undef RTLD_START
#define RTLD_START __asm__ ("\n\
.text\n\
.align 4\n\
.globl _start\n\
.globl _dl_start_user\n\
_start:\n\
	lgr   %r2,%r15\n\
	# Alloc stack frame\n\
	aghi  %r15,-160\n\
	# Set the back chain to zero\n\
	xc    0(8,%r15),0(%r15)\n\
	# Call _dl_start with %r2 pointing to arg on stack\n\
	brasl %r14,_dl_start	     # call _dl_start\n\
_dl_start_user:\n\
	# Save the user entry point address in %r8.\n\
	lgr   %r8,%r2\n\
	# Point %r12 at the GOT.\n\
	larl  %r12,_GLOBAL_OFFSET_TABLE_\n\
	# See if we were run as a command with the executable file\n\
	# name as an extra leading argument.\n\
	lghi  %r1,_dl_skip_args@GOT\n\
	lg    %r1,0(%r1,%r12)\n\
	lgf   %r1,0(%r1)	  # load _dl_skip_args\n\
	# Get the original argument count.\n\
	lg    %r0,160(%r15)\n\
	# Subtract _dl_skip_args from it.\n\
	sgr   %r0,%r1\n\
	# Adjust the stack pointer to skip _dl_skip_args words.\n\
	sllg  %r1,%r1,3\n\
	agr   %r15,%r1\n\
	# Set the back chain to zero again\n\
	xc    0(8,%r15),0(%r15)\n\
	# Store back the modified argument count.\n\
	stg   %r0,160(%r15)\n\
	# The special initializer gets called with the stack just\n\
	# as the application's entry point will see it; it can\n\
	# switch stacks if it moves these contents over.\n\
" RTLD_START_SPECIAL_INIT "\n\
	# Call the function to run the initializers.\n\
	# Load the parameters:\n\
	# (%r2, %r3, %r4, %r5) = (_dl_loaded, argc, argv, envp)\n\
	lghi  %r2,_rtld_local@GOT\n\
	lg    %r2,0(%r2,%r12)\n\
	lg    %r2,0(%r2)\n\
	lg    %r3,160(%r15)\n\
	la    %r4,168(%r15)\n\
	lgr   %r5,%r3\n\
	sllg  %r5,%r5,3\n\
	la    %r5,176(%r5,%r15)\n\
	brasl %r14,_dl_init@PLT\n\
	# Pass our finalizer function to the user in %r14, as per ELF ABI.\n\
	lghi  %r14,_dl_fini@GOT\n\
	lg    %r14,0(%r14,%r12)\n\
	# Free stack frame\n\
	aghi  %r15,160\n\
	# Jump to the user's entry point (saved in %r8).\n\
	br    %r8\n\
");


#endif /* !_ZOS_DL_MACHINE_H  */
