#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE___BUILTIN_PRINTF_UNLOCKED
#else

#include "test-lib/portable-symbols/printf_unlocked.h"

#undef __builtin_printf_unlocked
#define __builtin_printf_unlocked printf_unlocked

#endif
