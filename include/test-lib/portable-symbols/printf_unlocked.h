#pragma once

#include <stdio.h>

#ifndef YALIBCT_LIBC_DOESNT_HAVE_PRINTF_UNLOCKED
#else

#undef printf_unlocked
#define printf_unlocked printf

#endif
