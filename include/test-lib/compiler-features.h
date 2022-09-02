#pragma once

#include "test-lib/hedley.h"

#if HEDLEY_GNUC_HAS_ATTRIBUTE(__access__, 10, 0, 0)
#define YALIBCT_ATTRIBUTE_PARAMETER_IN(parameter_idx) __attribute__((__access__(__read_only__, parameter_idx))) HEDLEY_NON_NULL(parameter_idx)
#else
#define YALIBCT_ATTRIBUTE_PARAMETER_IN(parameter_idx) HEDLEY_NON_NULL(parameter_idx)
#endif

#if HEDLEY_HAS_ATTRIBUTE(__cold__)
#define YALIBCT_ATTRIBUTE_COLD __attribute__((__cold__))
#else
#define YALIBCT_ATTRIBUTE_COLD
#endif

#define YALIBCT_ATTRIBUTE_ERROR(message) __attribute__((__error__(message)))

#define YALIBCT_STRINGIFY_1(s) #s
#define YALIBCT_STRINGIFY(s) YALIBCT_STRINGIFY_1(s)

#ifdef __clang__
#define YALIBCT_DIAGNOSTIC_IGNORE(option) _Pragma(YALIBCT_STRINGIFY(clang diagnostic ignored option))
#else
#define YALIBCT_DIAGNOSTIC_IGNORE(option) _Pragma(YALIBCT_STRINGIFY(gcc diagnostic ignored option))
#endif

#define YALIBCT_EXPRESSION_NO_WARNING(option, block) \
    HEDLEY_DIAGNOSTIC_PUSH;                                         \
    YALIBCT_DIAGNOSTIC_IGNORE(#option);                             \
    block                                                           \
    HEDLEY_DIAGNOSTIC_PUSH;
