#pragma once

#ifdef YALIBCT_LIBC_HAS_ALIGNOF
#include <stdalign.h>
#else

#undef alignof
#define alignof _Alignof

#endif
