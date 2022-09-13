#pragma once

#ifdef YALIBCT_LIBC_HAS_NAN
#include <math.h>
#else

#undef NAN
#define NAN (.0f / .0f)

#endif
