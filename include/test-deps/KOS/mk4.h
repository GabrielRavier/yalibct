// Derived directly from work with this license:
/* Copyright (c) 2019-2022 Griefer@Work                                       *
 *                                                                            *
 * This software is provided 'as-is', without any express or implied          *
 * warranty. In no event will the authors be held liable for any damages      *
 * arising from the use of this software.                                     *
 *                                                                            *
 * Permission is granted to anyone to use this software for any purpose,      *
 * including commercial applications, and to alter it and redistribute it     *
 * freely, subject to the following restrictions:                             *
 *                                                                            *
 * 1. The origin of this software must not be misrepresented; you must not    *
 *    claim that you wrote the original software. If you use this software    *
 *    in a product, an acknowledgement (see the following) in the product     *
 *    documentation is required:                                              *
 *    Portions Copyright (c) 2019-2022 Griefer@Work                           *
 * 2. Altered source versions must be plainly marked as such, and must not be *
 *    misrepresented as being the original software.                          *
 * 3. This notice may not be removed or altered from any source distribution. *
 */

#pragma once

#include "test-lib/portable-functions/vstrdupf.h"
#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define DECL_BEGIN
#define PRIVATE static
#define NONNULL(...) HEDLEY_NON_NULL(__VA_ARGS__)
#define DECL_END
#define __assertion_failedf(expr_str, ...) (__afailf)(expr_str, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define __do_assertf(expr, expr_str, ...) (void)(HEDLEY_LIKELY(expr) || (__assertion_failedf(expr_str, __VA_ARGS__), 0))
#define assertf(expr, ...) __do_assertf(expr, #expr, __VA_ARGS__)
#define DEFINE_TEST(name) int main()
#define DEFINE_TEST_NON_SINGLE(name) void name()

static inline GRAVIER_TESTS_ATTRIBUTE_COLD HEDLEY_NO_RETURN
void __afailf(const char *expr_str, const char *file, unsigned long line, const char *func_name, const char *format, ...)
{
    va_list arguments;

    fprintf(stderr, "%s:%lu:%s: Assertion '%s' failed: \n", file, line, func_name, expr_str);
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fputc('\n', stderr);
    abort();
}
