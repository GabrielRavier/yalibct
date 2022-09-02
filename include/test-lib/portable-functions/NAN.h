#pragma once

#ifdef GRAVIER_TESTS_LIBC_HAS_NAN
#include <math.h>
#else

#define NAN (.0f / .0f)

#endif
