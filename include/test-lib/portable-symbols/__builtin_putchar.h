#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE___BUILTIN_PUTCHAR
#else

#include <stdio.h>

#undef __builtin_putchar
#define __builtin_putchar putchar

#endif
