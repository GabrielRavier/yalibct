#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_ALIGNOF
#include <stdalign.h>
#else

#undef alignof
#define alignof _Alignof

#endif
