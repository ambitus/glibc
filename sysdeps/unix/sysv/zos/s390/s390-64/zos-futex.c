/* Futex emulation for z/OS.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <lock-free.h>
#include <lowlevellock-futex.h>
#include <zos-futex.h>
#include <unimplemented.h>

/* TODO: At the moment this is not being built. Finish this up and link
   it into the rest of the library.  */

/* TODO: PEs or ECBs? Both would work, PEs would be more reliable, and
   would handle some parts of process termination for us, if we decide to
   handle shared cases with the same mechanism. ECBs are actually less
   complicated and faster.

   ECBs are easier to reuse for our use-case. Our current approach would
   not allow us to reuse PEs. Right now, we would have to deallocate
   every PE after its first use on thread wakeup. To fix that, we would
   need to do something like maintain a separate queue of valid PETs, and
   figure out how many of those we want to keep around at any one time
   before we start deallocating PEs, which is a pain.

   cond_timed_wait() is the only mechanism I can see that allows timeouts
   with waits.  */

/* TODO: make a resource manager to implement robust futexes.  */

/* TODO: Maybe use the shared memory lock mechanism to implement
   non-private futexes.  */


/* Timed and untimed futexes are implemented differently.
   For now, we use cond_wait and cond_timed_wait for everything.  */


/* We need to indicate whether or not a given futex is using
   cond_timed_wait or a regular wait_token.  */
#define IS_TIMED(futexp) (((uintptr_t) (futexp)) & 0x1)

/* A hashed wait queue.  */
lf_hash_table *__zos_futex_table = NULL;
libc_hidden_data_def (__zos_futex_table)

object_pool __wait_token_pool = {0};
libc_hidden_data_def (__wait_token_pool)

#if 0  /* For now, this is all just a work in progress.  */
static int
do_normal_wait (wait_token *wake_data)
{
  wait_token *new_pe;

  switch (/* return code.  */)
  {
  /* If we are successful, reuse the waiter's storage.  */
  case 0:
    return 0;

    /* TODO: check all important error codes.  */
  default:
    return -1;
  }
}

static int
do_timed_wait (wait_token *wake_data, const timespec *time)
{
}

static int
do_wake (wait_token *curr_pe)
{
  /* TODO: This.  */

  /* If we are unsuccessful, deallocate the pause element.  */
  switch (/* return code.  */)
  {
  case 0:
    return 0;
  default:
    destroy_token (curr_pe);
    __obj_pool_free_block (curr_pe);
    return -1;
  }
}

/* The cond_* syscalls. They are probably a fairly thin abstraction over
   pause elements or ECBs, though in practice they are likely heavier
   weight than both because of the syscall overhead. However,
   cond_timed_wait() seems to be the only readily apparent facility that
   can implement timed waits, so we have to use it.  */



typedef void (*__bpx4cse_t) (const uint32_t *event_list,
			     int32_t *retval, int32_t *retcode,
			     int32_t *reason_code);

/* Preform any setup that is necessary to start reciving wakeup requests
   on the given wait_token. This might consist of zeroing an ECB, calling
   cond_setup(), or allocating a PE token. The important thing is, once
   this function returns, if a wakeup request is sent to the wait_token
   before that waiter has had a chance to wait, the waiter will return
   from its subsequent wait immediately.

   Note: For some bizzare unknown reason, the cond_* services do not
   permit us to do any syscalls between now and when we try to wait.
   TODO: It's unclear if syscalls in other threads count. Talk to IBM. */
static void
setup_for_wakeups (wait_token *token, wait_token_type type)
{
  token->type = type;
  if (type == ECB)
    token->ecb = 0;
  else
    {
      int32_t retval, err, rc;
      const uint32_t ev = ZOS_CW_CONDVAR;

      BPX_CALL (cond_setup, __bpx4cse_t, &ev, &retval, &err, &rc);

      if (retval != 0)
	abort ();
    }
}

static wait_token *
create_token (wait_token_type type)
{
  wait_token *token = __obj_pool_alloc_block (__wait_token_pool);

  /* Initialize it with our data and become eligible to recive
     wakeups.  */
  setup_for_wakeups (token, type);
  return token;
}

static void
destroy_token (wait_token *token)
{
  if (token->type == cond_syscalls)
    {
      int32_t retval, err, rc;
      BPX_CALL (cond_cancel, __bpx4cse_t, &retval, &err, &rc);
    }
  __obj_pool_alloc_block (token, __wait_token_pool);
}

/* The core emulation of the futex syscall.  */

/* One of futex()'s arguments has variable type.  */
union time_or_v2
{
  const struct timespec *timeout;
  uint32_t val2;
};

/* The main syscall-equivalent.  */
int
__zos_sys_futex (int *errcode, int *uaddr, int futex_op, int val,
		 union time_or_v2 timeout_or_val2, ...)
{
  /* TODO: not yet implemented, we are only going to implement
     monotonic timeouts and private futexes right now.  */
  if (futex_op & FUTEX_CLOCK_REALTIME
      || !(futex_op & FUTEX_PRIVATE_FLAG))
    {
      *errcode = ENOSYS;
      return -1;
    }

  if ((uintptr_t) uaddr & 0x3)
    {
      *errcode = EINVAL;
      return -1;
    }

  switch (futex_op & ~(FUTEX_PRIVATE_FLAG | FUTEX_CLOCK_REALTIME))
  {
  case FUTEX_WAIT:
    return zos_futex_wait (errcode, futex_op, uaddr, val,
			   timeout_or_val2.timeout);
  case FUTEX_WAKE:
  case FUTEX_REQUEUE:
  case FUTEX_CMP_REQUEUE:
  case FUTEX_WAKE_OP:
  case FUTEX_OP_CLEAR_WAKE_IF_GT_ONE:
  case FUTEX_LOCK_PI:
  case FUTEX_UNLOCK_PI:
  case FUTEX_TRYLOCK_PI:
  case FUTEX_WAIT_BITSET:
  case FUTEX_WAKE_BITSET:
  case FUTEX_WAIT_REQUEUE_PI:
  case FUTEX_CMP_REQUEUE_PI:
  default:
    *errcode = ENOSYS;
    return -1;
  }

  /* Invalid futex_op.  */
  *errcode = EINVAL;
  return -1;
}
libc_hidden_def (__zos_sys_futex)


/* It's important to know who owns what data structures at what times.
   A thread is considered to have exclusive ownership of a wait_token
   if it is not possible for any other thread to have obtained a
   reference to the given wait_token. If a thread has exclusive ownership
   of a wait_token, it may safely deallocate the wait_token's storage.

   Once a wait_token is allocated, it may be accessed only by the
   thread that is about to suspend itself, until it is added to the wait
   queue. Once it is added to the wait queue, its storage may not be
   accessed until it has been removed from the queue, and then it may
   only be accessed by the thread that removed it from the queue.

   The removing thread owns the wait_token until it successfully wakes
   the corresponding waiting thread. If it successfully wakes the waiter,
   the removing thread is considered to have relinquished ownership of
   the wait_token structure to the newly woken thread, which will then
   deallocate it. If it fails to wake the suspended thread (or make it
   available for waking, at least) it maintains ownership of the
   wait_token and should deallocate it itself.
   TODO: Should we kill threads that we can't, for whatever reason, wake?

   It's also important to know that since a wait_token might be
   implemented by an ECB, the address of the wait token might be as
   important as its contents and as such must remain constant for the
   life of the token. For the same reason, they should be stored below
   the 31-bit bar.  */



/* All guarantees for basic private futexes are met by this
   implementation, however it will have different performance
   characteristics than can be seen on linux.  */

static int
do_futex_wait (int *errcode, int futex_op, int *uaddr,
	       int val, const struct timespec *timeout)
{
  wait_token *token;
  int retcode;

  /* Get storage for a new wait_token.  */
  token = create_token (timeout ? cond_syscalls : ECB);

  /* Add the new PE to the wait queue.  */
  __lf_hash_table_put ((uintptr_t) uaddr, (uintptr_t) token,
		       __zos_futex_table);

  /* It's possible that a wakeup occured before we could add our PE to
     the list, so we need to return.  */
  if (atomic_load_acquire (uaddr) != val)
    {
      uint64_t res = __lf_hash_table_pop ((uintptr_t) uaddr,
					  __zos_futex_table);

      if (!res)
	abort ();

      destroy_token ((wait_token *) res);

      *errcode = EAGAIN;
      return -1;
    }

  /* We don't need to worry about the time between the node getting added
     to the list and the pause, if a wake happens before we pause, the
     pause call will return immediately, so there is no race there.  */

  /* Suspend ourselves.  */
  int retcode = do_wait (token);

  switch (retcode)
  {
  case 0:
    destroy_token (token);
    break;
  /* TODO: figure out what to do here.  */
  default:
    *errcode = ENOSYS;
    return -1;
  }

  return 0;
}


static int
do_futex_wake (int *errcode, int futex_op, int *uaddr, int val)
{
  wait_token *token;
  int woken, retcode;
  bool problems = false;

  /* TODO: make sure that there can only be INT_MAX waiters
     on a single futex at most.  */

  /* Wake val waiters.  */
  for (woken = 0; woken < val; ++woken)
    {
      token = __lf_hash_table_pop ((uintptr_t) uaddr, __zos_futex_table);

      /* Presumably we've run out of waiters.  */
      if (!token)
	return woken;

      /* Wake.  */
      retcode = do_wake (token);

      switch (retcode)
      {
      case 0:
	break;
      default:
	problems = true;
	/* If we have not successfully woken anyone, destroy the token
	   ourselves.  */
	destroy_token (token);
	break;
      }
    }
  /* If we had an unsuccessful wake, report that something went
     wrong.  */
  *errcode = ENOSYS;
  return -1;
}

#endif /* 0  */
