#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_MCHECK_CHECK_ALL
#include <mcheck.h>
#else

static inline void mcheck_check_all() {}

#endif
