# Copyright (C) 2018 Rocket Software
# This file is part of the GNU C Library.
# Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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
sysdep_routines += storage mmap-anon lock-free tls-lookup zos-futex \
		   syscall-wrapper estaex estaex-handler
# Install bits/bpxystat.h, which is required by our stat.h
sysdep_headers += bits/bpxystat.h
endif

endif	# in-Makerules
