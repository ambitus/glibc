/* Copyright (C) 2019 Free Software Foundation, Inc.
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

#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

/* The TOD value corresponding to the start of the unix epoch.  */
#define TOD_UNIX_EPOCH_START	0x007D91048BCA0000UL

/* The TOD increments corresponding to one second and one microsecond.  */
#define TOD_PER_SEC		0xF42400UL
#define TOD_PER_USEC		0x10UL

/* Get the current time of day and timezone information,
   putting it into *TV and *TZ.  If TZ is NULL, *TZ is not filled.
   Returns 0 on success, -1 on errors.  */
int
__gettimeofday (struct timeval *tv, struct timezone *tz)
{
  /* z/OS TODO: The principals of operation describe a very complicated
     process for conversion from the TOD clock value to UTC time using
     the UTC Information Block that may depend on whether certain
     facilities are available, however the approach that we use here
     seems to work, somehow, but it's unclear where it is actually
     documented. We don't take TOD steering into account and we probably
     need to. Look into this again, and determine the what the proper
     and most correct conversion method is.  */

  uint64_t stcke_val[2];
  uint64_t epoch_tod, leap_second_adj, local_adj;
  uintptr_t CVTXTNT2_addr;

  if (tv != NULL)
    {
      /* Get leap second and local timezone corrections from the CVT.  */
      CVTXTNT2_addr = (0x00ffffff & *(uint32_t *) (CVT_PTR + 0x148));
      local_adj = *(uint64_t *) (CVTXTNT2_addr + 0x38);
      leap_second_adj = *(uint64_t *) (CVTXTNT2_addr + 0x50);

      /* GET the TOD clock value. We only care about the high-order 64
	 bits, the rest of the bits represent time measurements too
	 small to be relevant.  */
      __asm__ __volatile__ ("stcke %0" : "=Q" (stcke_val));

      /* Correct TOD to the unix epoch.  */
      epoch_tod = ((stcke_val[0] - leap_second_adj + local_adj)
		   - TOD_UNIX_EPOCH_START);

      tv->tv_sec = (time_t) (epoch_tod / TOD_PER_SEC);
      tv->tv_usec =
	(suseconds_t) ((epoch_tod % TOD_PER_SEC) / TOD_PER_USEC);
    }

  if (tz != NULL)
    {
      const time_t timer = tv->tv_sec;
      struct tm tm;
      const struct tm *tmp;

      const long int save_timezone = __timezone;
      const long int save_daylight = __daylight;
      char *save_tzname[2];
      save_tzname[0] = __tzname[0];
      save_tzname[1] = __tzname[1];

      tmp = localtime_r (&timer, &tm);

      tz->tz_minuteswest = __timezone / 60;
      tz->tz_dsttime = __daylight;

      __timezone = save_timezone;
      __daylight = save_daylight;
      __tzname[0] = save_tzname[0];
      __tzname[1] = save_tzname[1];

      if (tmp == NULL)
	return -1;
    }

  return 0;
}
libc_hidden_def (__gettimeofday)
weak_alias (__gettimeofday, gettimeofday)
libc_hidden_weak (gettimeofday)
