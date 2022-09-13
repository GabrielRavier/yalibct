#pragma once

#ifdef YALIBCT_LIBC_HAS___BUILTIN_PUTCHAR
#else

#include <stdio.h>

#undef __builtin_putchar
#define __builtin_putchar putchar

#endif
