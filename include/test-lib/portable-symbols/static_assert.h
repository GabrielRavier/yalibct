#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_STATIC_ASSERT
#include <assert.h>
#else

#undef static_assert
#define static_assert _Static_assert

#endif
