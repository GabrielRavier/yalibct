#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_NL_ARGMAX
#include <limits.h>
#else

#undef NL_ARGMAX
#define NL_ARGMAX 9

#endif
