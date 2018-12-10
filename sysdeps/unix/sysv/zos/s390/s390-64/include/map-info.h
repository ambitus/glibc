#ifndef _MAP_INFO_H
#define _MAP_INFO_H 1

#include <stdint.h>
#include <stdbool.h>

/* An anonymous mapping.  */
struct map_info
{
  /* Start of the mapping.  */
  void *start;

  /* Length.  */
  uint64_t length;

  /* 31-bit addr of the associated TCB.  */
  uint32_t tcbaddr;

  /* EXECUTABLE=NO specified.  */
  bool noexec;
};

#endif /* _MAP_INFO_H  */
