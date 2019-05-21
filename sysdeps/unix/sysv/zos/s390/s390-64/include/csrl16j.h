#include <stdint.h>


typedef (*csrl16j)(struct l16j_args_64 *args, int32_t *retval);

/* C wrapper over CSRL16J.
   Loads all 16 GPRs and 16 ARs, and optionally deallocates a storage
   area at the same time.  */

