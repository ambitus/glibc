/* Copyright (C) 2020 Free Software Foundation, Inc.
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

#include <signal.h>
#include <sysdep.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <internal-signals.h>

typedef void (*raise_bpx4pts_t) (char *ret_tid);
typedef void (*raise_bpx4ptk_t) (const char *tid, const int32_t *sig,
				 const int32_t *options,
				 int32_t *retval, int32_t *retcode,
				 int32_t *reason_code);

int
raise (int sig)
{
  char tid[8];
  int32_t options = 0, retval, retcode, reason_code;
  sigset_t set;
  __libc_signal_block_app (&set);

  BPX_CALL (pthread_self, raise_bpx4pts_t, tid);
  BPX_CALL (pthread_kill, raise_bpx4ptk_t, tid, &sig, &options, &retval,
	    &retcode, &reason_code);

  __libc_signal_restore_set (&set);

  if (retval != 0)
    __set_errno (retcode);

  return retval;
}
libc_hidden_def (raise)
weak_alias (raise, gsignal)
