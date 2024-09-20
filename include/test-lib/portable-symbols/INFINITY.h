#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_INFINITY
#include <math.h>
#else

#undef INFINITY
#define INFINITY (__builtin_inff())

#endif
