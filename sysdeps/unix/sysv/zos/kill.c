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

/* z/OS kill syscall implemention.  */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define BYPASS_PTRACE 0x8000
#define SIGNAL_CODE 0x2000
#define SUPER_KILL 0x0800
#define OVERRIDE_SIGTRACE 0x0400
#define USER_SYSCALL_TRACING 0x0200

typedef uint32_t signal_options_t;

/* Type of kill system call.  */
typedef void (*__bpx4kil_t) (pid_t *process_id, uint32_t *signal,
                             signal_options_t *signal_options,
                             int32_t *return_value, int32_t *return_code,
                             int32_t *reason_code);

/* Send signal to specified pid.  */
int
__kill (pid_t pid, int sig)
{
  int32_t retval, reason_code, process_id, signal;
  signal_options_t signal_options;
  INTERNAL_SYSCALL_DECL (retcode);

  _Static_assert(sizeof(pid) == sizeof(process_id),
                 "PID sizes don't match.");
  _Static_assert(sizeof(sig) == sizeof(signal),
                 "Signal sizes don't match");

  process_id = pid;
  signal = sig;

  /*
    Setup Signal Options

    Top 2 bytes: Deliver dummy (all 0) data with the signal.  Bypass
    ptrace: We shouldn't bypass ptrace.  Signal code: z/OS TODO:
    understand this better (unset for now).  Super kill: Super kill is
    an oddity of z/OS and must be sent after a SIGKILL.  Override
    sigtrace: Don't override by default, instead toggle.  User Syscall
    Tracing: not relavent.
  */

  signal_options = 0;

  BPX_CALL (kill, __bpx4kil_t, &process_id, &signal,
            &signal_options, &retval, &retcode, &reason_code);

  if (retval == -1)
    {
      switch (retcode)
        {
        case EMVSSAF2ERR:
          __set_errno (EPERM);
          break;
        default:
          __set_errno (retcode);
          break;
        }
    }

  return retval;
}

hidden_def (__kill)
weak_alias (__kill, kill)
