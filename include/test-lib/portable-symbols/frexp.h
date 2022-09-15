#pragma once

#ifdef YALIBCT_LIBC_HAS_FREXP
#include <math.h>
#else

#define YALIBCT_FREXP_INTERNAL_GEN_DOUBLE
#include "test-lib/portable-symbols/internal/frexp-template.h"

#endif
