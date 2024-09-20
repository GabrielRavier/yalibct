#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_NAN
#include <math.h>
#else

#undef NAN
#define NAN (.0f / .0f)

#endif
