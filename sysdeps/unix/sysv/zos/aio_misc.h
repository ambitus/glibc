/* Copyright (C) 2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, see <http://www.gnu.org/licenses/>.  */

/* The only reason why this file exists is to work around a horrific use
   of extern inline in the linux implementation that breaks our whole
   static inline syscall wrapper system. Pretty much everything besides
   that is the same is it is in the linux implementation.

   z/OS TODO: Figure out a workaround that doesn't require us to manually
   copy all of the changes from the linux implementation into this file
   whenever we update our sources.  */

#ifndef _AIO_MISC_H
# define DONT_NEED_AIO_MISC_COND	1
# include <sysdeps/pthread/aio_misc.h>
# include <assert.h>
# include <nptl/pthreadP.h>
# include <futex-internal.h>
# include <limits.h>
# include <pthread.h>
# include <signal.h>
# include <sysdep.h>

#define AIO_MISC_NOTIFY(waitlist) \
  do {									      \
    if (*waitlist->counterp > 0 && --*waitlist->counterp == 0)		      \
      futex_wake ((unsigned int *) waitlist->counterp, 1, FUTEX_PRIVATE);     \
  } while (0)

#define AIO_MISC_WAIT(result, futex, timeout, cancel)			      \
  do {									      \
    volatile unsigned int *futexaddr = &futex;				      \
    unsigned int oldval = futex;					      \
									      \
    if (oldval != 0)							      \
      {									      \
	pthread_mutex_unlock (&__aio_requests_mutex);			      \
									      \
	int oldtype;							      \
	if (cancel)							      \
	  oldtype = LIBC_CANCEL_ASYNC ();				      \
									      \
	int status;							      \
	do								      \
	  {								      \
	    status = futex_reltimed_wait ((unsigned int *) futexaddr, oldval, \
					  timeout, FUTEX_PRIVATE);	      \
	    if (status != EAGAIN)					      \
	      break;							      \
									      \
	    oldval = *futexaddr;					      \
	  }								      \
	while (oldval != 0);						      \
									      \
	if (cancel)							      \
	  LIBC_CANCEL_RESET (oldtype);					      \
									      \
	if (status == EINTR)						      \
	  result = EINTR;						      \
	else if (status == ETIMEDOUT)					      \
	  result = EAGAIN;						      \
	else								      \
	  assert (status == 0 || status == EAGAIN);			      \
									      \
	pthread_mutex_lock (&__aio_requests_mutex);			      \
      }									      \
  } while (0)

# define aio_start_notify_thread __aio_start_notify_thread
# define aio_create_helper_thread __aio_create_helper_thread

static inline void
__aio_start_notify_thread (void)
{
  sigset_t ss;
  sigemptyset (&ss);
  INTERNAL_SYSCALL_DECL (err);
  INTERNAL_SYSCALL (sigprocmask, err, 3, SIG_SETMASK, &ss, NULL);
}

static inline int
__aio_create_helper_thread (pthread_t *threadp, void *(*tf) (void *),
			    void *arg)
{
  pthread_attr_t attr;

  /* Make sure the thread is created detached.  */
  pthread_attr_init (&attr);
  pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

  /* The helper thread needs only very little resources.  */
  (void) pthread_attr_setstacksize (&attr, __pthread_get_minstack (&attr));

  /* Block all signals in the helper thread.  To do this thoroughly we
     temporarily have to block all signals here.  */
  sigset_t ss;
  sigset_t oss;
  sigfillset (&ss);
  INTERNAL_SYSCALL_DECL (err);
  INTERNAL_SYSCALL (sigprocmask, err, 3, SIG_SETMASK, &ss, &oss);

  int ret = pthread_create (threadp, &attr, tf, arg);

  /* Restore the signal mask.  */
  INTERNAL_SYSCALL (sigprocmask, err, 3, SIG_SETMASK, &oss, NULL);

  (void) pthread_attr_destroy (&attr);
  return ret;
}
#endif
