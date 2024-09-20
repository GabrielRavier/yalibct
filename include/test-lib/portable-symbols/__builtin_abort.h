#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE___BUILTIN_ABORT
#else

#include <stdlib.h>

#undef __builtin_abort
#define __builtin_abort abort

#endif
