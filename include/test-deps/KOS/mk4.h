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
#include "test-lib/hedley.h"
#include "test-lib/assert-with-message.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define DECL_BEGIN
#define PRIVATE static
#define NONNULL(...) HEDLEY_NON_NULL(__VA_ARGS__)
#define DECL_END
#define assertf(expr, ...) YALIBCT_ASSERT_WITH_MESSAGE(expr, __VA_ARGS__)
#define DEFINE_TEST(name) int main()
#define DEFINE_TEST_NON_SINGLE(name) static void name()
