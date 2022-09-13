#pragma once

#ifdef YALIBCT_LIBC_HAS___BUILTIN_PUTS
#else

#include <stdio.h>

#undef __builtin_puts
#define __builtin_puts puts

#endif
