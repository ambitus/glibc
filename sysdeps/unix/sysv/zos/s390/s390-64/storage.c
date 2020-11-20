/* C interface to z/OS storage allocation services.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Giancarlo Frix <gfrix@rocketsoftware.com>.

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

/* Our memory model for z/OS:
   z/OS TODO: Implement this.
   z/OS TODO: what about the .text, global/static var area (.data/.bss)?
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



/* This does the actual storage request. Returns zero on failure,
   1 on success for RELEASE, and the storage address for OBTAIN.  */
static uint32_t
storage_request (uint32_t length, uint32_t target_addr,
		 uint32_t flags, uint32_t flags2)
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

  uint32_t ret_addr, return_code;
  uint32_t cvt = *(uint32_t *)16;
  uint32_t syst_linkage_table = *(uint32_t *)(uint64_t)(cvt + 772);
  uint32_t lx_ex_offset = (flags & STORAGE_REQ_RELEASE) ? 204 : 160;
  uint32_t lx_ex = *(uint32_t *)(uint64_t)(syst_linkage_table + lx_ex_offset);

  __asm__ __volatile__ ("l	%%r0, %2      \n\t"
			"sar	%%a15, %%r0   \n\t"
			"llgt   %%r0, %3      \n\t"
			"llgt   %%r1, %4      \n\t"
			"llgt   %%r14, %5     \n\t"
			"llgt   %%r15, %6     \n\t"
			"pc	0(%%r14)      \n\t"
			"st	%%r1, %0      \n\t"
			"st	%%r15, %1     "
			: "=m" (ret_addr), "=m" (return_code)
			: "m" (flags2), "m" (length), "m" (target_addr), "m" (lx_ex), "m" (flags)
			: "r0", "r1", "r14", "r15", "cc");

  /* Return code is 0 for a successful request.	 */
  if (flags & STORAGE_REQ_RELEASE)
    return return_code == 0 ? 1 : 0;
  
  if (flags & STORAGE_CHECKZERO_YES)
    {
      if (return_code == 0)
	memset((void *)(uint64_t)ret_addr, 0, length);
      else if (return_code == 0x14)
	return_code = 0;
    }
  return return_code == 0 ? ret_addr : 0;
}


/* C interface to a subset of STORAGE OBTAIN's functionality.
   Allocate storage of the given length, optionally forcing it to start
   on a page boundary. Return the address of the lower bound of the
   allocated area, or NULL if the allocation request failed.

   Can only get virtual storage below the 2 GB bar.  */
void *
__storage_obtain (unsigned int length, 
		  bool noexec, bool on_page_boundary)
{
  /* The storage obtain request is as follows:
     STORAGE OBTAIN,LENGTH=length,SP=131,LOC=(31,64),COND=YES,
     CALLRKY=YES,[,maybe EXECUTABLE=NO][,maybe BNDRY=PAGE]
     [,maybe CHECKZERO=YES]
     sp 131 is non auth, private low, fetch protected, pagable, owned by the 
     job step task.
     We also zero all storage once obtained, if it was not zeroed
     already by STORAGE OBTAIN.

     If on_page_boundary is true, then we also specify BNDRY=PAGE.
     BNDRY=PAGE will always get storage starting at a page boundary.

     We use LOC=(31,64) which means we get virtual storage below 2 GB
     that is backed anywhere.
     We specify CALLRKY=YES because we use subpool 129.

     TODO: Implement read/write protection
     TODO: Should we use the BACK parameter?
  */
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

  /* Base flags that we use for every OBTAIN request. Right now we use
     subpool 131 (owned by the job step), always specify LOC=(31,64),
     CALLRKEY=YES, and COND=YES.  */
  flags = REGULAR_OBTAIN_FLAGS;
  flags |= on_page_boundary ? STORAGE_BNDRY_PAGE : 0;

  if (noexec)
    {
      flags |= STORAGE_USE_AR15;
      flags2 = STORAGE_EXECUTABLE_NO;
    }
  else
    flags2 = 0;

  request_return = storage_request (length, 0, flags, flags2);
  retptr = request_return ? (void *) (uintptr_t) request_return : NULL;

  return retptr;
}

/* C interface to a subset of STORAGE RELEASE's functionality.
   Should probably only be used with STORAGE OBTAINed storage.
   Returns 0 on success and -1 on failure.  */
int
__storage_release (unsigned int storage_addr, unsigned int length)
{
  uint32_t flags, flags2, res;
  if (length == 0 || storage_addr == 0)
    return -1;

  /* Base flags that we use for every OBTAIN request. Right now we use
     subpool 131, with CALLRKY=YES and COND=YES.  */
  flags = REGULAR_RELEASE_FLAGS;
  flags2 = 0;

  res = storage_request (length, storage_addr, flags, flags2);
  return res ? -1 : 0;
}

/* discards the data, replacing it with zeros.  Equivalent to
   STORAGE RELEASE followed by STORAGE OBTAIN.  Faster than
   MVCL for large areas.  The pages do not use resouces. */
void
__pgser_release(unsigned int storage_addr, unsigned int length)
{
  unsigned int first_byte_addr = storage_addr;
  unsigned int last_byte_addr = storage_addr + length - 1;
  __asm__("slr  %%r0, %%r0   \n\t"
          "l    %%r1, %0     \n\t"
          "l    %%r15, %1    \n\t"
          "la   %%r14, 0x600 \n\t"
          "svc  138          \n\t"
          : : "m" (first_byte_addr), "m" (last_byte_addr)
          : "r0", "r1", "r14", "r15");
}

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

/* IARV64 is controlled by the structure that follows.
   Some pieces of functionality can only be used by authorized
   programs. Each macro is listed after the field to which it applies,
   and they are generally named after the named macro argument key-value
   specification that they represent.  */
struct iarv64
{
  uint8_t version;
  uint8_t request;
#define IARV64_REQUEST_GETSTOR 1
#define IARV64_REQUEST_GETSHARED 2
#define IARV64_REQUEST_DETACH 3
#define IARV64_REQUEST_PAGEFIX 4
#define IARV64_REQUEST_PAGEUNFIX 5
#define IARV64_REQUEST_PAGEOUT 6
#define IARV64_REQUEST_DISCARDDATA 7
#define IARV64_REQUEST_PAGEIN 8
#define IARV64_REQUEST_PROTECT 9
#define IARV64_REQUEST_SHAREMEMOBJ 10
#define IARV64_REQUEST_CHANGEACCESS 11
#define IARV64_REQUEST_UNPROTECT 12
#define IARV64_REQUEST_CHANGEGUARD 13
#define IARV64_REQUEST_LIST 14
#define IARV64_REQUEST_GETCOMMON 15
#define IARV64_REQUEST_COUNTPAGES 16
#define IARV64_REQUEST_PCIEFIX 17
#define IARV64_REQUEST_PCIEUNFIX 18
#define IARV64_REQUEST_CHANGEATTRIBUTE 19
  uint8_t flags0;
#define IARV64_MOTKNSOURCE_SYSTEM 0x80
#define IARV64_MOTKNCREATOR_SYSTEM 0x40
#define IARV64_MATCH_MOTOKEN 0x20
  uint8_t key;
  uint8_t flags1;
#define IARV64_FIELD_VALID_KEY 0x80
#define IARV64_FIELD_VALID_USERTKN 0x40
#define IARV64_FIELD_VALID_TTOKEN 0x20
#define IARV64_FIELD_VALID_CONVERTSTART 0x10
#define IARV64_FIELD_VALID_GUARDSIZE64 0x08
#define IARV64_FIELD_VALID_CONVERTSIZE64 0x04
#define IARV64_FIELD_VALID_MOTKN 0x02
#define IARV64_FIELD_VALID_OWNERJOBNAME 0x01
  uint8_t flags2;
#define IARV64_COND_YES 0x80
#define IARV64_FPROT_NO 0x40
#define IARV64_CONTROL_AUTH 0x20
#define IARV64_GUARDLOC_HIGH 0x10
#define IARV64_CHANGEACCESS_GLOBAL 0x08
#define IARV64_PAGEFRAMESIZE_1MEG 0x04
#define IARV64_PAGEFRAMESIZE_MAX 0x02
#define IARV64_PAGEFRAMESIZE_ALL 0x01
  uint8_t flags3;
#define IARV64_MATCH_USERTOKEN 0x80
#define IARV64_AFFINITY_SYSTEM 0x40
#define IARV64_USE2GTO32G_YES 0x20
#define IARV64_OWNER_NO 0x10
#define IARV64_V64SELECT_NO 0x08
#define IARV64_SVCDUMPRGN_NO 0x04
#define IARV64_V64SHARED_NO 0x02
#define IARV64_SVCDUMPRGN_ALL 0x01
  uint8_t flags4;
#define IARV64_LONG_NO 0x80
#define IARV64_CLEAR_NO 0x40
#define IARV64_VIEW_READONLY 0x20
#define IARV64_VIEW_SHAREDWRITE 0x10
#define IARV64_VIEW_HIDDEN 0x08
#define IARV64_CONVERT_TOGUARD 0x04
#define IARV64_CONVERT_FROMGUARD 0x02
#define IARV64_KEEPREAL_NO 0x01
  uint64_t segments;
  uint8_t ttoken[16];  /* TCB token.  */
  uint64_t user_token;  /* out_memobj_token, memobj_token */
  void *origin;
  struct iarv64_range *range_list;
  void *memobjstart;  /* in_origin */
  uint32_t guardsize;
  uint32_t convertsize32;
  uint32_t alet_value;
  uint32_t range_count;
  uint32_t v64listptr;
  uint32_t v64listlength;
  void *convertstart;
  uint64_t convertsize64;
  uint64_t guardsize64;
  uint64_t match_usertoken;
  uint8_t dump_priority;
#define IARV64_DEFAULT_DUMP_PRIORITY 99
  uint8_t flags5;
#define IARV64_DUMPPROTOCOL_YES 0x80
#define IARV64_ORDER_DUMPPRIORITY 0x40
#define IARV64_TYPE_PAGEABLE 0x20
#define IARV64_TYPE_DREF 0x10
#define IARV64_OWNERCOM_HOME 0x08
#define IARV64_OWNERCOM_PRIMARY 0x04
#define IARV64_OWNERCOM_SYSTEM 0x02
#define IARV64_OWNERCOM_BYASID 0x01
  uint8_t flags6;
#define IARV64_V64COMMON_NO 0x80
#define IARV64_MEMLIMIT_NO 0x40
#define IARV64_DETACHFIXED_YES 0x20
#define IARV64_DOAUTHCHECKS_YES 0x10
#define IARV64_LOCALSYSAREA_YES 0x08
#define IARV64_AMOUNTSIZE_4K 0x04
#define IARV64_AMOUNTSIZE_1MEG 0x02
#define IARV64_MEMLIMIT_COND 0x01
  uint8_t flags7;
#define IARV64_FIELD_VALID_DUMP 0x80
#define IARV64_FIELD_VALID_OPTIONVALUE 0x40
#define IARV64_FIELD_VALID_SVCDUMPRGN 0x20
#define IARV64_ATTRIBUTE_DEFS 0x10
#define IARV64_ATTRIBUTE_OWNERGONE 0x08
#define IARV64_ATTRIBUTE_NOTOWNERGONE 0x04
#define IARV64_TRACKINFO_YES 0x02
#define IARV64_UNLOCKED_YES 0x01
  uint8_t dump;
#define IARV64_DUMP_NONE 0
#define IARV64_DUMP_NO 1
#define IARV64_DUMP_LIKESQA 2
#define IARV64_DUMP_LIKECSA 3
#define IARV64_DUMP_LIKERGN 32
#define IARV64_DUMP_LIKELSQA 33
#define IARV64_DUMP_ALL 255
  uint8_t flags8;
#define IARV64_PAGEFRAMESIZE_PAGEABLE1MEG 0x80
#define IARV64_PAGEFRAMESIZE_DREF1MEG 0x40
#define IARV64_SADMP_YES 0x20
#define IARV64_SADMP_NO 0x10
#define IARV64_USE2GTO64G_YES 0x08
#define IARV64_DISCARDPAGES_YES 0x04
#define IARV64_EXECUTABLE_YES 0x02
#define IARV64_EXECUTABLE_NO 0x01
  uint8_t owner_asid[2];
  uint8_t option_value;
  uint8_t reserved1[8];
  uint8_t owner_jobname[8];
  uint8_t reserved2[7];
  uint64_t dma_page_table;
  uint64_t units;
  uint8_t flags9;
#define IARV64_FIELD_VALID_UNITS 0x80
#define IARV64_UNITSIZE_1M 0x40
#define IARV64_UNITSIZE_2G 0x20
#define IARV64_PAGEFRAMESIZE_1M 0x10
#define IARV64_PAGEFRAMESIZE_2G 0x08
#define IARV64_TYPE_FIXED 0x04
  uint8_t flags10;
#define IARV64_FIELD_VALID_INORIGIN 0x80
#define IARV64_SENSITIVE_YES 0x40
#define IARV64_SENSITIVE_NO 0x20
#define IARV64_FIELD_VALID_SENSITIVE 0x10
  uint8_t flags11;
#define IARV64_FIELD_VALID_OBJECTTYPE 0x80
#define IARV64_OBJECTTYPE_POOL 0x40
#define IARV64_OBJECTTYPE_RSMINTERNAL 0x20
  uint8_t reserved3[5];
};

/* Call IARV64.  */
static inline void
call_iarv64 (struct iarv64 *args, uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  uint32_t rc, reason;
  __asm__ ("la    1, %0       \n\t"
	   "llgt  14, 16      \n\t"
	   "l     14, 772(14) \n\t"
	   "l     14, 208(14) \n\t"
	   "la    15, 14      \n\t"
	   "or    14, 15      \n\t"
	   "pc    0(14)       \n\t"
	   "st    15, %1      \n\t"
	   "st    0, %2       \n\t"
	   : "+m"(*args), "=m"(rc), "=m"(reason)
	   :: "r0", "r1", "r14", "r15", "memory");
  if (rc_ptr)
    *rc_ptr = rc;
  if (reason_ptr)
    *reason_ptr = reason;
}

/* Alocate SEGMENTS 1MB segments, optionally with a GUARDSIZE MB
   guard area either at the top or bottom of the allocation depending
   on LOC. If GUARDSIZE is 0, no guard area is allocated.  */
void *
__iarv64_getstorage (uint64_t segments,
		     uint64_t guardsize, enum guardloc loc,
		     uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  uint32_t rc, reason;

  if (!rc_ptr)
    rc_ptr = &rc;
  if (!reason_ptr)
    reason_ptr = &reason;

  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.flags2 |= IARV64_COND_YES;
  iarv64_args.request = IARV64_REQUEST_GETSTOR;
  iarv64_args.segments = segments;
  iarv64_args.guardsize64 = guardsize;
  if (guardsize > 0)
    {
      if (loc == GUARDLOC_HIGH)
	iarv64_args.flags2 |= IARV64_GUARDLOC_HIGH;
      iarv64_args.flags1 |= IARV64_FIELD_VALID_GUARDSIZE64;
      iarv64_args.guardsize64 = guardsize;
    }
  iarv64_args.dump_priority = IARV64_DEFAULT_DUMP_PRIORITY;
  iarv64_args.flags5 |= IARV64_TYPE_PAGEABLE;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);

  if (*rc_ptr > 4)
    return NULL;

  return iarv64_args.origin;
}

/* Totally deallocate the memory object starting at MEMOBJSTART.  */
void
__iarv64_detach (void *memobjstart,
		 uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.flags2 |= IARV64_COND_YES;
  iarv64_args.request = IARV64_REQUEST_DETACH;
  iarv64_args.memobjstart = memobjstart;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}

/* Grow the guard area at the edge of the specified memory object.
   If the memory object was allocated with GUARDLOC HIGH, the low
   end of the guard area ending at the highest virtual address in
   in the memory object will be expanded downwards by SEGMENTS 1MB
   segments. A guard area starting at the highest virtual address
   in the memory object will be created if one does not yet exist.
   The reverse will occur for memory objects allocated with
   GUARDLOC LOW, with the guard area starting from the lowest
   address in the memory object being expanded upward.

   The contents of any pages converted to guard areas are discarded,
   and those pages no longer contribute to the resident memory image
   of the address space.  */
void
__iarv64_grow_guard (void *origin,
		     uint64_t convertsize,
		     uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.flags2 |= IARV64_COND_YES;
  iarv64_args.request = IARV64_REQUEST_CHANGEGUARD;
  iarv64_args.flags1 |= IARV64_FIELD_VALID_CONVERTSIZE64;
  iarv64_args.flags4 |= IARV64_CONVERT_TOGUARD;
  iarv64_args.memobjstart = origin;
  iarv64_args.convertsize64 = convertsize;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}

/* Set the address range starting at CONVERTSTART (which must be on a
   MB boundary) and continuing for the next CONVERTSIZE * 1MB bytes
   to guard pages. The specified region must be inside of a memory
   object.

   The contents of any pages converted to guard areas are discarded,
   and those pages no longer contribute to the resident memory image
   of the address space.

   This operation is not permitted on high virtual common memory
   objects.  */
void
__iarv64_set_guard (void *convertstart,
		    uint64_t convertsize,
		    uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.flags2 |= IARV64_COND_YES;
  iarv64_args.request = IARV64_REQUEST_CHANGEGUARD;
  iarv64_args.flags1 |= IARV64_FIELD_VALID_CONVERTSTART;
  iarv64_args.flags1 |= IARV64_FIELD_VALID_CONVERTSIZE64;
  iarv64_args.flags4 |= IARV64_CONVERT_TOGUARD;
  iarv64_args.convertstart = convertstart;
  iarv64_args.convertsize64 = convertsize;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}

/* Shrink the guard area at the edge of the specified memory object.
   If the memory object was allocated with GUARDLOC HIGH, the low
   end of the guard area ending at the highest virtual address in
   in the memory object will be shrunk upwards by CONVERTSIZE 1MB
   segments. The reverse will occur for memory objects allocated with
   GUARDLOC LOW, with the guard area starting from the lowest
   address in the memory object being shrunk downward.

   Any guard pages converted to regular pages are zeroed, and
   immediately contribute to the resident memory image of the address
   space.  */
void
__iarv64_shrink_guard (void *origin,
		       uint64_t convertsize,
		       uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.flags2 |= IARV64_COND_YES;
  iarv64_args.request = IARV64_REQUEST_CHANGEGUARD;
  iarv64_args.flags1 |= IARV64_FIELD_VALID_CONVERTSIZE64;
  iarv64_args.flags4 |= IARV64_CONVERT_FROMGUARD;
  iarv64_args.memobjstart = origin;
  iarv64_args.convertsize64 = convertsize;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}

/* Set the address range starting at CONVERTSTART (which must be on a
   MB boundary) and continuing for the next CONVERTSIZE * 1MB bytes
   to non-guard pages. The specified region must be inside of a memory
   object.

   Any guard pages converted to regular pages are zeroed, and
   immediately contribute to the resident memory image of the address
   space.

   This operation is not permitted on high virtual common memory
   objects.  */
void
__iarv64_set_nonguard (void *convertstart,
		       uint64_t convertsize,
		       uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.flags2 |= IARV64_COND_YES;
  iarv64_args.request = IARV64_REQUEST_CHANGEGUARD;
  iarv64_args.flags1 |= IARV64_FIELD_VALID_CONVERTSTART;
  iarv64_args.flags1 |= IARV64_FIELD_VALID_CONVERTSIZE64;
  iarv64_args.flags4 |= IARV64_CONVERT_FROMGUARD;
  iarv64_args.convertstart = convertstart;
  iarv64_args.convertsize64 = convertsize;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}

/* Make areas of a memory object read-only.
   The ranges to operate on are specified in the RANGE_LIST array, which
   contains RANGE_COUNT entries. RANGE_COUNT must be less than or equal
   to 16.  */
void
__iarv64_protect (uint32_t range_count,
		  struct iarv64_range *range_list,
		  uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.request = IARV64_REQUEST_PROTECT;
  iarv64_args.range_count = range_count;
  iarv64_args.range_list = range_list;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}


/* Make areas of a memory object writable.
   The ranges to operate on are specified in the RANGE_LIST array, which
   contains RANGE_COUNT entries. RANGE_COUNT must be less than or equal
   to 16.  */
void
__iarv64_unprotect (uint32_t range_count,
		    struct iarv64_range *range_list,
		    uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.request = IARV64_REQUEST_UNPROTECT;
  iarv64_args.range_count = range_count;
  iarv64_args.range_list = range_list;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}

/* Discard the data from areas of a memory object.
   The ranges to operate on are specified in the RANGE_LIST array, which
   contains RANGE_COUNT entries. RANGE_COUNT must be less than or equal
   to 16.  */
void
__iarv64_discard_data (uint32_t range_count,
		  struct iarv64_range *range_list,
		  uint32_t *rc_ptr, uint32_t *reason_ptr)
{
  struct iarv64 iarv64_args;
  memset (&iarv64_args, 0, sizeof (iarv64_args));
  iarv64_args.version = 2;
  iarv64_args.request = IARV64_REQUEST_DISCARDDATA;
  iarv64_args.flags4 |= IARV64_KEEPREAL_NO;
  iarv64_args.range_count = range_count;
  iarv64_args.range_list = range_list;
  call_iarv64 (&iarv64_args, rc_ptr, reason_ptr);
}
