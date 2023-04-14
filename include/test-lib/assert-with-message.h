#pragma once

#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

static inline YALIBCT_ATTRIBUTE_COLD HEDLEY_NO_RETURN
void yalibct_internal_assert_with_message_print_failed_assertion(const char *expr_str, const char *file, unsigned long line, const char *func_name, const char *format, ...)
{
    va_list arguments;

    assert(fprintf(stderr, "%s:%lu:%s: Assertion '%s' failed: \n", file, line, func_name, expr_str) >= 0);
    va_start(arguments, format);
    assert(vfprintf(stderr, format, arguments) >= 0);
    va_end(arguments);
    assert(fputc('\n', stderr) == '\n');
    abort();
}

#define YALIBCT_DO_ASSERT_WITH_MESSAGE(expr, expr_str, ...) (void)(HEDLEY_LIKELY(expr) || ((yalibct_internal_assert_with_message_print_failed_assertion)(expr_str, __FILE__, __LINE__, __func__, __VA_ARGS__), 0))
#define YALIBCT_ASSERT_WITH_MESSAGE(expr, ...) YALIBCT_DO_ASSERT_WITH_MESSAGE(expr, #expr, __VA_ARGS__)
