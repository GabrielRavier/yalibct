#pragma once

#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static inline YALIBCT_ATTRIBUTE_COLD HEDLEY_NO_RETURN
void yalibct_print_failed_assertion(const char *expr_str, const char *file, unsigned long line, const char *func_name, const char *format, ...)
{
    va_list arguments;

    fprintf(stderr, "%s:%lu:%s: Assertion '%s' failed: \n", file, line, func_name, expr_str);
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fputc('\n', stderr);
    abort();
}

#define YALIBCT_DO_ASSERT_WITH_MESSAGE(expr, expr_str, ...) (void)(HEDLEY_LIKELY(expr) || ((yalibct_print_failed_assertion)(expr_str, __FILE__, __LINE__, __func__, __VA_ARGS__), 0))
#define YALIBCT_ASSERT_WITH_MESSAGE(expr, ...) YALIBCT_DO_ASSERT_WITH_MESSAGE(expr, #expr, __VA_ARGS__)
