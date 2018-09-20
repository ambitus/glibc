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
		       mlock2 pkey_mprotect pkey_set pkey_get sysctl
sysdep_routines := $(filter-out $(zos-filter-out-misc),$(sysdep_routines))
endif
