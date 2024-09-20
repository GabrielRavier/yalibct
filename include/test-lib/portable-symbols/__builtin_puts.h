#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE___BUILTIN_PUTS
#else

#include <stdio.h>

#undef __builtin_puts
#define __builtin_puts puts

#endif
