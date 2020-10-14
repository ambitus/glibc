/* clock_gettime -- Get current time from a POSIX clockid_t.  z/OS version.
   Copyright (C) 2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <errno.h>
#include <time.h>

#define clks_per_second 4096000000ull
#define clks_to_nanoseconds(x) ((x * 1000ull) / 4096ull)
#define seconds_from_1900_to_1970 2208988800ull

#define SYSDEP_GETTIME							\
  case CLOCK_REALTIME:							\
  case CLOCK_MONOTONIC:							\
  {									\
    uint64_t clk;							\
    __asm__ __volatile__ ("stckf %0" : "=m"(clk));			\
    clk -= seconds_from_1900_to_1970 * clks_per_second;			\
    tp->tv_sec = (time_t) (clk / clks_per_second);			\
    clk -= tp->tv_sec * clks_per_second;				\
    tp->tv_nsec = (long) clks_to_nanoseconds(clk);			\
    retval = 0;								\
  }									\
  break;								\
  case CLOCK_PROCESS_CPUTIME_ID:					\
  {									\
    uint64_t clk;							\
    __asm__ __volatile__ ("llgt	%%r15,0x224(0,0)\n\t"	  /* ASCB */	\
			  "lg	%%r15,0x40(%%r15, 0)\n\t" /* Job Step Time */ \
			  "stg	%%r15,%0"				\
			  : "=m"(clk) : : "r15");			\
    tp->tv_sec = (time_t) (clk / clks_per_second);			\
    clk -= tp->tv_sec * clks_per_second;				\
    tp->tv_nsec = (long) clks_to_nanoseconds(clk);			\
    retval = 0;								\
  }									\
  break;								\
  case CLOCK_THREAD_CPUTIME_ID:						\
  {									\
    uint64_t clk;							\
    /* TIMEUSED STORADR=(2),ECT=YES */					\
    __asm__ __volatile__ ("llgt	%%r14,0x10(0,0)\n\t"	/* CVT */	\
			  "llgt	%%r15,0x8c(%%r14,0)\n\t" /* ECVT */	\
			  "l	%%r15,0x350(%%r15,0)\n\t" /* ECVTECT */ \
			  "basr	%%r14,%%r15\n\t" /* Extract CPU Time (z9 or later) */ \
			  "stg	%%r0,%0"				\
			 : "=m"(clk) : : "r0","r1","r14","r15");	\
    tp->tv_sec = (time_t) (clk / clks_per_second);			\
    clk -= tp->tv_sec * clks_per_second;				\
    tp->tv_nsec = (long) clks_to_nanoseconds(clk);			\
    retval = 0;								\
  }									\
  break

#define HANDLED_REALTIME 1
#define HANDLED_CPUTIME	1

#include <sysdeps/unix/clock_gettime.c>
