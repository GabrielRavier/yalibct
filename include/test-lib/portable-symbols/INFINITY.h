#pragma once

#ifdef YALIBCT_LIBC_HAS_INFINITY
#include <math.h>
#else

#undef INFINITY
#define INFINITY (__builtin_inff())

#endif
