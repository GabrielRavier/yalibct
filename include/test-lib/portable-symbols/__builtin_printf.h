#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE___BUILTIN_PRINTF
#else

#include <stdio.h>

#undef __builtin_printf
#define __builtin_printf printf

#endif
