#pragma once

#ifdef YALIBCT_LIBC_HAS_NL_ARGMAX
#include <limits.h>
#else

#undef NL_ARGMAX
#define NL_ARGMAX 9

#endif
