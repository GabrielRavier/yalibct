#pragma once

#ifdef YALIBCT_LIBC_HAS_MTRACE
#include <mcheck.h>
#else

static inline void mtrace() {}

#endif
