#pragma once

#ifdef YALIBCT_LIBC_HAS_FLOOR
#include <math.h>
#else

#define YALIBCT_FLOOR_INTERNAL_GEN_DOUBLE
#include "test-lib/portable-symbols/internal/floor-template.h"

#endif
