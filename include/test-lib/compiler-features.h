#pragma once

#include "test-lib/hedley.h"

#ifdef NDEBUG
#error "AAAAAAAAAAAAA!!!! NOOOOOOOOO !! DON'T DISABLE ASSERT !!!!!!!!!!!!!!!"
#endif

#ifdef __has_c_attribute
#define YALIBCT_HAS_C_ATTRIBUTE(attribute) __has_c_attribute(attribute)
#else
#define YALIBCT_HAS_C_ATTRIBUTE(attribute) 0
#endif

#if HEDLEY_GNUC_HAS_ATTRIBUTE(__access__, 10, 0, 0)
#define YALIBCT_ATTRIBUTE_PARAMETER_IN(parameter_idx) __attribute__((__access__(__read_only__, parameter_idx))) HEDLEY_NON_NULL(parameter_idx)
#else
#define YALIBCT_ATTRIBUTE_PARAMETER_IN(parameter_idx) HEDLEY_NON_NULL(parameter_idx)
#endif

#if HEDLEY_GNUC_HAS_ATTRIBUTE(__noipa__, 0, 0, 0)
#define YALIBCT_ATTRIBUTE_NOIPA __attribute__((__noipa__))
#else
#define YALIBCT_ATTRIBUTE_NOIPA
#endif

#if HEDLEY_GNUC_HAS_ATTRIBUTE(__noclone__, 4, 8, 0)
#define YALIBCT_ATTRIBUTE_NOCLONE __attribute__((__noclone__))
#else
#define YALIBCT_ATTRIBUTE_NOCLONE
#endif

#if HEDLEY_GNUC_HAS_ATTRIBUTE(__optimize__, 4, 4, 0)
#define YALIBCT_ATTRIBUTE_OPTIMIZE_O0 __attribute__((__optimize__("O0")))
#elif HEDLEY_HAS_ATTRIBUTE(__optnone__) || defined(__clang__)
#define YALIBCT_ATTRIBUTE_OPTIMIZE_O0 __attribute__((__optnone__))
#else
#define YALIBCT_ATTRIBUTE_OPTIMIZE_O0
#endif

#if HEDLEY_HAS_ATTRIBUTE(__cold__)
#define YALIBCT_ATTRIBUTE_COLD __attribute__((__cold__))
#else
#define YALIBCT_ATTRIBUTE_COLD
#endif

#define YALIBCT_ATTRIBUTE_ERROR(message) __attribute__((__error__(message)))

#if HEDLEY_HAS_ATTRIBUTE(__unused__)
#define YALIBCT_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#define YALIBCT_ATTRIBUTE_UNUSED
#endif

#if YALIBCT_HAS_C_ATTRIBUTE(__maybe_unused__)
#define YALIBCT_ATTRIBUTE_MAYBE_UNUSED [[__maybe_unused__]]
#else
#define YALIBCT_ATTRIBUTE_MAYBE_UNUSED YALIBCT_ATTRIBUTE_UNUSED
#endif

#define YALIBCT_STRINGIFY_1(s) #s
#define YALIBCT_STRINGIFY(s) YALIBCT_STRINGIFY_1(s)

#ifdef __clang__
#define YALIBCT_DIAGNOSTIC_IGNORE(option) _Pragma(YALIBCT_STRINGIFY(clang diagnostic ignored option))
#else
#define YALIBCT_DIAGNOSTIC_IGNORE(option) _Pragma(YALIBCT_STRINGIFY(GCC diagnostic ignored option))
#endif

#define YALIBCT_EXPRESSION_NO_WARNING(option, block) \
    HEDLEY_DIAGNOSTIC_PUSH;                                         \
    YALIBCT_DIAGNOSTIC_IGNORE(#option);                             \
    block;                                                          \
    HEDLEY_DIAGNOSTIC_POP;
