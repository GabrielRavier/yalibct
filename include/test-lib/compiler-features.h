#pragma once

#include "lib-hedley.h"

#if HEDLEY_GNUC_HAS_ATTRIBUTE(__access__, 10, 0, 0)
#define GRAVIER_TESTS_ATTRIBUTE_PARAMETER_IN(parameter_idx) __attribute__((__access__(__read_only__, parameter_idx))) HEDLEY_NON_NULL(parameter_idx)
#else
#define GRAVIER_TESTS_ATTRIBUTE_PARAMETER_IN(parameter_idx) HEDLEY_NONNULL(parameter_idx)
#endif

#if HEDLEY_HAS_ATTRIBUTE(__cold__)
#define GRAVIER_TESTS_ATTRIBUTE_COLD __attribute__((__cold__))
#else
#define GRAVIER_TESTS_ATTRIBUTE_COLD
#endif
