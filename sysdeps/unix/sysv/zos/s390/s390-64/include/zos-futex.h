#ifndef _ZOS_FUTEX_H
#define _ZOS_FUTEX_H 1
#include <stdint.h>

typedef enum
{
  ECB,
  cond_syscalls
} wait_token_type;

typedef struct
{
  wait_token_type type;
  union {
    uint32_t ecb;
    uint64_t thread_id;
  };
} wait_token;

#endif /* _ZOS_FUTEX_H  */
