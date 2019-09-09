# Copyright (C) 2019 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

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

# filter out stuff that linux defines.

ifeq ($(subdir),stdlib)
zos-filter-out-stdlib := __makecontext_ret
sysdep_routines := $(filter-out $(zos-filter-out-stdlib),$(sysdep_routines))
endif

ifeq ($(subdir),misc)
# specifically exclude clone from the filter-out right now.
# z/OS TODO: see if the semantics of z/OS fork are close enough to
# linux's clone to meaningfully emulate clone.
# leave out epoll_wait because its needed by some things.
zos-filter-out-misc := umount2 readahead setfsuid setfsgid \
		       epoll_pwait signalfd eventfd eventfd_read \
		       eventfd_write prlimit personality \
		       tee vmsplice splice open_by_handle_at \
		       mlock2 pkey_mprotect pkey_set pkey_get sysctl \
		       epoll_create inotify_init
sysdep_routines := $(filter-out $(zos-filter-out-misc),$(sysdep_routines))
endif
