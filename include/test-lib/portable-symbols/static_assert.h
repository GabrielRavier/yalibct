#pragma once

#ifdef YALIBCT_LIBC_HAS_STATIC_ASSERT
#include <assert.h>
#else

#undef static_assert
#define static_assert _Static_assert

#endif
