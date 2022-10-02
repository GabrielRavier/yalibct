#pragma once

#ifdef YALIBCT_LIBC_HAS___BUILTIN_ABORT
#else

#include <stdlib.h>

#undef __builtin_abort
#define __builtin_abort abort

#endif
