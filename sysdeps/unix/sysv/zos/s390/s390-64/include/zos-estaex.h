/* ESTAEX routines and SDWA mappings
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Michael Colavita <mcolavita@rocketsoftware.com>.

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

#ifndef _ZOS_ESTAEX_H
#define _ZOS_ESTAEX_H 1

#include <stdint.h>

#define SIZEOF_ESTAEX_PARM_LIST 24

/* Please read https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.ieaa600/sdwa.htm */

struct sdwaptrs {
  uint32_t sdwanrc1;
  uint32_t sdwarc1;
  uint32_t sdwarc2;
  uint32_t sdwanrc2;
  uint32_t sdwarc3;
  uint32_t sdwanrc3;
  uint32_t sdwarc4;
  uint32_t sdwarc5;
};

struct sdwarc1 {
  uint8_t id_and_name_and_version[5+23+8+8];
  uint32_t abend_reason_code; /* valid if REASON_CODE_IS_VALID is on in sdwarc1 completion_code_flags */
  uint8_t recovery_routine_name[8];
  uint8_t component_id_base[4];
  uint8_t reserved1;
  uint8_t compcode_flags;
  uint8_t retry_flags;
#define RETRY_USE_64BIT_GRS 0x10
  uint8_t recovery_type; /* 0:none, 1:frr, 2:estae, 3:arr */
  uint32_t reserved2[3];
  uint32_t recovery_routine_address;
  uint8_t reserved3[28];
  uint8_t dae_info[10];
  uint8_t purgedq_info[6];
  uint8_t instructions[12];
  uint32_t ascb;
  uint32_t asst;
  uint8_t original_completion_code_flags;
  uint8_t original_completion_code[3];
  uint32_t original_reason_code;
  uint32_t crs[16];
  uint32_t ars[16];
  uint32_t rb_ars[16];
  uint8_t duct[64];
  uint8_t translation_exception_access_register; /* in low order 4 bits */
  uint8_t extended_flags;
#define EC_RB_INFO_IS_FROM_LINKAGE_STACK 0x80
#define EC_INFO_HAS_TRANSLATION_ADDRESS 0x40
#define EC_INFO_HAS_ASID 0x20
#define EC_INFO_HAS_PC_NUMBER 0x08
  uint8_t subspace_flags;
  uint8_t in_zarch_mode;
  uint32_t arr_msta_area[2];
  uint32_t linkage_stack_address; /* cr15 */
  uint32_t locks_held_string_extension;
  uint16_t linkage_stack_entries_past_estaex;
  uint8_t retry_amode;
#define RETRY_AMODE_DEFAULT 0
#define RETRY_AMODE_24 1
#define RETRY_AMODE_31 2
#define RETRY_AMODE_64 3
  uint8_t reserved4;
  uint8_t subspace_stoken[8];
  uint8_t subspace_name[8];
  uint8_t system_name[8];
};

struct sdwarc2 {
  uint64_t failing_storage_address; /* https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.ieav200/iea3v2_Hard_failures.htm */
  uint64_t machine_check_interrupt_code;
};

struct sdwarc3 {
  uint8_t pad[32]; /* what locks should be freed */
};

struct sdwarc4 {
  uint64_t gprs[16];
  uint8_t pad1[8];
  uint32_t gprs32_high[16];
  uint64_t crs[16];
  uint64_t translation_exception_address;
  uint64_t breaking_event_address;
  uint64_t psw128[2];
};

struct sdwarc5 {
  uint64_t transaction_abort_grs[16];
  uint64_t transaction_abort_psw[2];
  /* This data is valid only when bits SDWAPCHK and SDWAPTX1 are on */
};

struct sdwanrc1 {
  /* dump storage ranges */
  uint8_t pad[240];
};

struct sdwanrc2 {
  /* storage subpools */
  uint8_t pad[16];
};

struct sdwanrc3 {
  /* dumpopx storage ranges */
  uint8_t pad[240];
};

struct ec_info {
  uint8_t reserved;
  uint8_t ilc; /* instruction length code is (ilc > 1) & 0x3 */
  uint16_t interrupt_code;
  union {
    uint32_t translation_address; /* sdwarc1 extended_flags has bit EC_INFO_HAS_TRANSLATION_ADDRESS */
    struct {
      uint16_t reserved;
      uint16_t asid;
    } space;
    struct {
      uint8_t reserved[3];
      uint8_t exception_code;
    } data; /* for interrupt code 7 */
  } ta;
};

struct sdwa {
  uint32_t parameter_list_address;
  uint8_t completion_code_flags;
#define REASON_CODE_IS_VALID 0x04
  uint8_t completion_code[3];
#define BC_INTERRUPT_CODE(bc_psw64) (bc_psw64[0] & 0xFFFF)
#define BC_INSTRUCTION_LENGTH_CODE(bc_psw64) (bc_psw64[1] >> 30)
#define BC_CONDITION_CODE(bc_psw64) ((bc_psw64[1] >> 28) & 0x3)
#define BC_PROGRAM_MASK(bc_psw64) ((bc_psw64[1] >> 24) & 0xF)
  uint32_t bc_psw64[2];
  uint32_t bc_rb_psw64[2];
  uint32_t gprs[16];
  uint8_t program_name[8];
  uint32_t program_entry_point;
  uint32_t reserved1;
  uint32_t ec_psw64[2];
  struct ec_info ec_info;
  uint32_t ec_rb_psw64[2];
  struct ec_info ec_rb_info;
  uint32_t rb_gprs[16];
  uint8_t sdwa_subpool;
  uint8_t sdwa_length;
  uint8_t machine_check_data[28];
  uint8_t exit_info[4];
#define EXIT_INFO0_IS_PROGRAM_CHECK 0x40
#define EXIT_INFO0_IS_TASK_SVC_13   0x10
#define EXIT_INFO0_IS_SYSTEM_SVC_13 0x08
#define EXIT_INFO0_IS_BAD_SVC       0x04 /* SVC called in a context where SVCs are not allowed */
#define EXIT_INFO0_IS_TRANSLATION   0x02
#define EXIT_INFO0_IS_PAGE_IO_ERROR 0x01
#define EXIT_INFO0_IS_RTM1_SVC      0x01
#define EXIT_INFO1_PARALLEL_DETACHED 0x80
#define EXIT_INFO1_NOT_MY_FAULT      0x40 /* read about it in IHASDWA */
  /* the other flags are also relevant, see IHASDWA */
  uint16_t asid_of_memory; /* non zero means cross memory */
  uint8_t reserved2[2];
  uint32_t retry_routine_address;
  uint8_t reserved3[8];
  /* 0xFC */
  uint8_t retry_code;
#define RETRY_CODE_NEXT_ESTAI     0
#define RETRY_CODE_RETRY          4
#define RETRY_CODE_NO_MORE_ESTAI 16
  uint8_t retry_flags[3];
#define RETRY_FLAGS0_UPDATE_REGISTERS 0x08
#define RETRY_FLAGS0_FREE_SDWA        0x04
  uint8_t lock_area[32];
  uint8_t logrec_info[28];
  uint8_t reserved4[0x168 - 0x13C];
  uint16_t cpu_id;
  uint8_t reserved5[0x170 - 0x16A];
  uint32_t sdwaptrs;
  uint8_t reserved6[0x298 - 0x174];
};

/* Modify with care. This struct is referenced in assembly. */
struct estaex_handler_data {
  void (*user_handler) (struct sdwa *, void *);
  void *user_data;
};

/* z/OS TODO: make these hidden.  */

extern int __set_estaex_handler (void (*) (struct sdwa *, void *),
				 void *);
extern void __estaex_handler_dump (struct sdwa *, void *);
extern void (*__estaex_handler_wrapper) (void);
extern void __zos_dump_stack (int fd, void *psw_addr, void *r13);


#endif  /* !_ZOS_ESTAEX_H  */
