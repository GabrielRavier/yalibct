#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_MTRACE
#include <mcheck.h>
#else

static inline void mtrace() {}

#endif
