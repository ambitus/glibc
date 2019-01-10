/* C interface to z/OS storage allocation services.
   Copyright (C) 2018 Rocket Software
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>, 2018.

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

#include <stdint.h>
#include <unistd.h>
#include <zos-core.h>
#include <stdbool.h>
#include <string.h>


unsigned int __ipt_zos_tcb __attribute__ ((nocommon));
hidden_data_def (__ipt_zos_tcb)

/* Our memory model for z/OS:
   TODO: Implement this.
   TODO: what about the .text, global/static var area (.data/.bss)?
   4 separate memory area category
       1. per-thread stack
       2. common sbrk heap
       3. tls area
       4. A not-predetermined number of mmaped memory areas
   A single-threaded process would have a stack, a heap, a TLS
   area, and however many mmaped areas as it chooses to create.


   Requirements:
   * Stack should be able to grow to ~10 MB for compatibility with
     linux. Should be released automatically when thread terminates.
   * Heap should be able to grow to very large sizes and then shrink,
     and should be able to release storage back to the OS. Should be
     released automatically when process terminates.
   * TLS area size is fixed for life of thread. Should be released
     automatically when thread terminates.
   * mmap MAP_ANONYMOUS areas must not take up physical storage before
     the individual pages are referenced, and must be zeroed on
     allocation. Should only be released when program terminates.
   * mmapped areas should be able to be munmapped.

   So we will need to keep track of all mmaps at the process level

   BPX services do that for file-backed mmaps, we will need to do it
   for anonymous mappings manually. If we assocate anonymous mapping
   storage with the process's IPT (which unfortunately already has
   a heriarchial relationship with other threads), then it should
   be cleaned up on process termination.  */


/* This is the range of subpools we will consider using.  */
_Static_assert (1 <= STORAGE_SUBPOOL && STORAGE_SUBPOOL <= 127, "");

/* We want a private, fetch-protected, task-owned subpool. There are
   many available, the choice of specific subpool number is
   arbitrary.
   The storage key is ignored for the subpools we are using.  */
#define STORAGE_SUBPOOL 72


/* This does the actual storage request. Returns zero on failure,
   1 on success for RELEASE, and the storage address for OBTAIN.  */
static uint32_t
storage_request (uint32_t length, uint32_t tcbaddr,
		 uint32_t target_addr, uint32_t flags,
		 uint32_t flags2)
{
  /* storage calling convention (for our use case):
     in:
       OBTAIN and RELEASE:
	 r0:  length
	 r1:  storage address (unused for OBTAIN if not EXPLICIT)
	 r14: pc number
	 r15: flags, subpool, and key
	 a0:  addr of TCB to associate storage with
	 a15: extra flags, only used if USE_AR15 flag is set
       just OBTAIN:
     out:
       OBTAIN and RELEASE:
	 r0, r14, a0, a1 a14, a15: clobbered
	 r15: return code
       just OBTAIN:
	 r1: obtained storage addr
       just RELEASE:
	 r1: clobbered  */

  register uint32_t r15 asm ("r15") = flags;
  register uint32_t storage_addr asm ("r1") = target_addr;
  register uint32_t len asm ("r0") = length;
  __asm__ __volatile__ ("sar   %%a0, %3\n\t"
			"sar   %%a15, %4\n\t"
			"llgt  %%r14, 16\n\t"
			"l     %%r14, 772(%%r14)\n\t"
			"l     %%r14, 160(%%r14)\n\t"
			"pc    0(%%r14)\n\t"
			: "+r" (len), "+r" (storage_addr),
			  "+r" (r15)
			: "r" (tcbaddr), "r" (flags2)
			: "r14", "a0", "a1", /* "a14", "a15", */ "cc");

  uint32_t ret_addr = storage_addr, return_code = r15;

  /* Return code is 0 for a successful request.	 */
  if (flags & STORAGE_REQ_RELEASE)
    return return_code == 0 ? 1 : 0;
  return return_code == 0 ? ret_addr : 0;
}


/* C interface to a subset of STORAGE OBTAIN's functionality.
   Allocate storage of the given length, optionally forcing it to start
   on a page boundary. Return the address of the lower bound of the
   allocated area, or NULL if the allocation request failed.

   Can only get virtual storage below the 2 GB bar.  */
void *
__storage_obtain (unsigned int length, unsigned int tcbaddr,
		  bool noexec,
		  bool on_page_boundary)
{
  /* The storage obtain request is as follows:
     STORAGE OBTAIN,LENGTH=length,SP=100,LOC=(31,31)
     ,TCBADDR=ipt,COND=YES
     [,maybe EXECUTABLE=NO][,maybe BNDRY=PAGE][,maybe CHECKZERO=YES]
     We also zero all storage once obtained, if it was not zeroed
     already by STORAGE OBTAIN.

     If on_page_boundary is true, then we also specify BNDRY=PAGE.
     BNDRY=PAGE will always get storage starting at a page boundary.

     We use LOC=(31,31) which means we get virtual storage below 2 GB
     that is backed by physical storage below 2 GB.
     We use a subpool for which key does not matter.

     TODO: Implement read/write protection
     TODO: Should we use the BACK parameter?
     TODO: Should we use LOC=(31,64) which means we get virtual storage
     below 2 GB that can be backed by physical storage above 2 GB?  */
  uint32_t flags, flags2, request_return;
  void *retptr;

  /* We need to preform all possible validation on inputs. If the
     parameters are incorrect, we could abend. We can't use assert,
     since we might get called very early in libc setup.  */

  /* We require length != 0. Also, storage obtain internally rounds
     all requests to the next multiple of 8. We require users to be
     aware of that behavior, so they always understand how much storage
     they are allocating.  */
  if (length == 0 || length % 8 != 0)
    return NULL;

  /* We require storage to be associated with a vaild TCB.
     TODO: check that TCB is valid here.  */
  if (tcbaddr == 0)
    return NULL;

  /* Base flags that we use for every OBTAIN request. Right now we use
     a constant fixed storage subpool, always specify LOC=(31,31),
     TCBADDR, and COND=YES.  */
  flags = REGULAR_OBTAIN_FLAGS | STORAGE_USE_TCBADDR;
  flags |= on_page_boundary ? STORAGE_BNDRY_PAGE : 0;

  if (noexec)
    {
      flags |= STORAGE_USE_AR15;
      flags2 = STORAGE_EXECUTABLE_NO;
    }
  else
    flags2 = 0;

  request_return = storage_request (length, tcbaddr, 0, flags, flags2);
  retptr = request_return ? (void *) (uintptr_t) request_return : NULL;

  /* For our subpool and LOC, when BNDRY=PAGE and LENGTH >= 4096, or
     when length >= 8192, storage obtain will zero the storage for
     us.  */
  if (retptr && (length < 4096 || (!on_page_boundary && length < 8192)))
    memset (retptr, 0, length);

  return retptr;
}
libc_hidden_def (__storage_obtain)


/* C interface to a subset of STORAGE RELEASE's functionality.
   Should probably only be used with STORAGE OBTAINed storage.
   Returns 0 on success and -1 on failure.  */
int
__storage_release (unsigned int storage_addr, unsigned int length,
		   unsigned int tcbaddr, bool noexec)
{
  uint32_t flags, flags2, res;
  if (length == 0 || tcbaddr == 0 || storage_addr == 0)
    return 1;

  /* Base flags that we use for every OBTAIN request. Right now we use
     a constant fixed storage subpool, specify TCBADDR, and
     COND=YES.  */
  flags = REGULAR_RELEASE_FLAGS | STORAGE_USE_TCBADDR;
  if (noexec)
    {
      flags |= STORAGE_USE_AR15;
      flags2 = STORAGE_EXECUTABLE_NO;
    }
  else
    flags2 = 0;

  res = storage_request (length, tcbaddr, storage_addr, flags, flags2);
  return res ? -1 : 0;
}
libc_hidden_def (__storage_release)

/* IARV64
   IARV64 is a storage management facility similar to STORAGE
   with a few differences. The important features for us are:
   * it allocates virtual storage above the 2 GB bar
   * it allocates in megabyte increments
   * it supports guard areas, which are segments of an allocation that
     cannot be used and are not backed by physical storage. The size
     of guard areas can be changed. Decreasing the size of a guard area
     is a fast operation, while increasing the size of one (that is,
     freeing physical storage) is relatively more expensive.  */
