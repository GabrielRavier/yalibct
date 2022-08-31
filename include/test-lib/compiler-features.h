#pragma once

#include "test-lib/hedley.h"

#if HEDLEY_GNUC_HAS_ATTRIBUTE(__access__, 10, 0, 0)
#define GRAVIER_TESTS_ATTRIBUTE_PARAMETER_IN(parameter_idx) __attribute__((__access__(__read_only__, parameter_idx))) HEDLEY_NON_NULL(parameter_idx)
#else
#define GRAVIER_TESTS_ATTRIBUTE_PARAMETER_IN(parameter_idx) HEDLEY_NON_NULL(parameter_idx)
#endif

#if HEDLEY_HAS_ATTRIBUTE(__cold__)
#define GRAVIER_TESTS_ATTRIBUTE_COLD __attribute__((__cold__))
#else
#define GRAVIER_TESTS_ATTRIBUTE_COLD
#endif

#define GRAVIER_TESTS_ATTRIBUTE_ERROR(message) __attribute__((__error__(message)))

#define GRAVIER_TESTS_STRINGIFY_1(s) #s
#define GRAVIER_TESTS_STRINGIFY(s) GRAVIER_TESTS_STRINGIFY_1(s)

#ifdef __clang__
#define GRAVIER_TESTS_DIAGNOSTIC_IGNORE(option) _Pragma(GRAVIER_TESTS_STRINGIFY(clang diagnostic ignored option))
#else
#define GRAVIER_TESTS_DIAGNOSTIC_IGNORE(option) _Pragma(GRAVIER_TESTS_STRINGIFY(gcc diagnostic ignored option))
#endif

#define GRAVIER_TESTS_EXPRESSION_NO_WARNING(option, block) \
    HEDLEY_DIAGNOSTIC_PUSH;                                         \
    GRAVIER_TESTS_DIAGNOSTIC_IGNORE(#option);                       \
    block                                                           \
    HEDLEY_DIAGNOSTIC_PUSH;
