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

#include <errno.h>
#include <sys/time.h>
#include <sys/timex.h>

/* Adjust the current time of day by the amount in DELTA.
   If OLDDELTA is not NULL, it is filled in with the amount
   of time adjustment remaining to be done from the last `__adjtime' call.
   This call is restricted to the super-user.  */
/* z/OS TODO: We can't implement this, figure out what to do with it.  */
int
__adjtime (const struct timeval *delta, struct timeval *olddelta)
{
  __set_errno (ENOSYS);
  return -1;
}

weak_alias (__adjtime, adjtime)
stub_warning (adjtime)


int
__adjtimex (struct timex *tx)
{
  __set_errno (ENOSYS);
  return -1;
}

libc_hidden_def (__adjtimex)
weak_alias (__adjtimex, adjtimex)
stub_warning (adjtimex)
