#pragma once

#ifdef YALIBCT_LIBC_HAS_STATIC_ASSERT
#include <assert.h>
#else

#include "test-lib/hedley.h"

#undef static_assert
#define static_assert HEDLEY_STATIC_ASSERT

#endif
