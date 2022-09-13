#pragma once

#ifdef YALIBCT_LIBC_HAS_MCHECK_CHECK_ALL
#include <mcheck.h>
#else

static inline void mcheck_check_all() {}

#endif
