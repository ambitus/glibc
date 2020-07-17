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

/* z/OS nanosleep syscall implemention.  */
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sysdep-cancel.h>

#define CW_INTRPT   1
#define CW_CONDVAR 32

#define NOT_IN_BOUNDS(x, min_inclusive, max_inclusive) \
  (x < min_inclusive || x > max_inclusive)

/* Type of cond_timed_wait system call.  */
typedef void (*__bpx4ctw_t) (const uint32_t *seconds,
                             const uint32_t *nanoseconds,
                             const uint32_t *event_list,
                             uint32_t *seconds_remaining,
                             uint32_t *nanoseconds_remaining,
                             int32_t *return_value,
                             int32_t *return_code,
                             int32_t *reason_code);


/* Pause execution for a number of nanoseconds.  */
int
__nanosleep (const struct timespec *requested_time,
             struct timespec *remaining_time)
{
  int32_t retval, reason_code;
  uint32_t event_list;
  uint32_t seconds, nanoseconds;
  uint32_t seconds_remaining, nanoseconds_remaining;
  INTERNAL_SYSCALL_DECL (retcode);

  event_list = CW_INTRPT;

  /* The bounds must be checked here... */
  if (requested_time == NULL ||
      NOT_IN_BOUNDS(requested_time->tv_nsec, 0, 1000000000) ||
      requested_time->tv_sec < 0)
    {
      __set_errno (EINVAL);
      return -1;
    }

  /* Use a saturating cast instead.  */
  seconds = requested_time->tv_sec < INT32_MAX
    ? requested_time->tv_sec : INT32_MAX;
  /* ... so the cast here is safe.  */
  nanoseconds = requested_time->tv_nsec;

  BPX_CALL_CANCEL (cond_timed_wait, __bpx4ctw_t, &seconds,
                   &nanoseconds, &event_list, &seconds_remaining,
                   &nanoseconds_remaining, &retval, &retcode,
                   &reason_code);

  /* Here retvalue is set to 0 if a CW_CONDVAR event occurs and -1
     otherwise, since we do not include CW_CONDVAR in the event_list
     retvalue should always be set to -1 here, thus the return code
     has been set.  */

  int ret;
  switch (retcode)
    {
    case EAGAIN:
      ret = 0;
      break;
    case EINTR:
      if (remaining_time != NULL)
        {
          remaining_time->tv_sec = seconds_remaining;
          remaining_time->tv_nsec = nanoseconds_remaining;
        }
      __set_errno (EINTR);
      ret = -1;
      break;
    case EINVAL:
      /* Since the second and nanosecond values are checked prior to the
         syscall we should never get this error, and this code should be
         unreachable.  */
      __set_errno (EINVAL);
      ret = -1;
      break;
    default:
      /* Something went really wrong, this should be unreachable,
         so we expose the underlying issue.  */
      __set_errno (retcode);
      ret = -1;
    }

  return ret;
}

hidden_def (__nanosleep)
weak_alias (__nanosleep, nanosleep)
