#pragma once

#include <stdio.h>

#ifdef YALIBCT_LIBC_HAS___BUILTIN_PRINTF_UNLOCKED
#else

#undef printf_unlocked
#define printf_unlocked printf

#endif
