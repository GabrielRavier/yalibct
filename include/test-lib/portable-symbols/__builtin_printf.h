#pragma once

#ifdef YALIBCT_LIBC_HAS___BUILTIN_PRINTF
#else

#include <stdio.h>

#undef __builtin_printf
#define __builtin_printf printf

#endif
