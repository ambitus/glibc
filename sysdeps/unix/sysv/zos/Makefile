# Copyright (C) 2019-2020 Free Software Foundation, Inc.
# This file is part of the GNU C Library.
# Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2019.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with the GNU C Library; if not, see
# <http://www.gnu.org/licenses/>.

ifdef in-Makerules

# For now, don't try to generate anything from the installed Unix system
# and its libraries. That is only of use when building for a Unix
# system, so as to be compatible with some existing binaries for that
# system. It might allow us to maintain some compatibility with LE in
# the future, if we ever figure out exactly what this option does, but
# for now we don't bother with it.
override inhibit-glue = yes

# Do not use any assembly code from sysdeps/unix/sysv/linux (and
# subdirectories). This bypasses all the system call stubs and uses
# any existing posix or generic C files instead.
# For now, we also prevent the use of any assembly in sysdeps/s390,
# because it uses the wrong calling convention.
inhibit-sysdep-asm += unix/sysv/linux.*
override inhibit-unix-syscalls = yes

ifeq ($(subdir),misc)
sysdep_routines += storage mmap-anon zos-futex \
		   syscall-wrapper estaex estaex-handler sir sir_entry \
		   loadhfs prctl mvsprocclp fchattr chattr
# Install bits/bpxystat.h, which is required by our stat.h
sysdep_headers += bits/bpxystat.h
endif

ifeq ($(subdir),io)
# z/OS TODO: Rework this. We should determine our own set of headers
# to install, and remove the symlink from the repo.
sysdep_headers += linux/falloc.h
endif

ifeq ($(subdir),elf)
# Put the thread pointer lookup functions in the static libc and
# in ld.so for shared programs
sysdep-dl-routines += lock-free tls-lookup loadhfs
endif

ifeq ($(subdir),csu)
# Statically link the pt_interp loading code into startup code
# z/OS TODO: The dependency analysis for csu-loadhfs and
# csu-syscall-wrapper doesn't work.
# z/OS TODO: figure out a more elegant solution to linking the
# loading functions.
start-installed-name-rule = yes
$(objpfx)$(start-installed-name): $(objpfx)start.o $(objpfx)abi-note.o \
				  $(objpfx)init.o $(objpfx)static-reloc.o \
				  $(objpfx)csu-loadhfs.os \
				  $(objpfx)csu-syscall-wrapper.os
	$(link-relocatable)
$(objpfx)r$(start-installed-name): $(objpfx)start.o $(objpfx)abi-note.o \
				  $(objpfx)init.o $(objpfx)csu-loadhfs.os \
				  $(objpfx)csu-syscall-wrapper.os
	$(link-relocatable)
$(objpfx)S$(start-installed-name): $(objpfx)start.os $(objpfx)abi-note.o \
				  $(objpfx)init.o $(objpfx)csu-loadhfs.os \
				  $(objpfx)csu-syscall-wrapper.os
	$(link-relocatable)

endif

ifeq ($(subdir),malloc)
CFLAGS-malloc.c += -DMORECORE_CLEARS=1
endif

endif	# in-Makerules
