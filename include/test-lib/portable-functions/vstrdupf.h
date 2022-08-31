// Adapted from code with this license:
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

#ifdef GRAVIER_TESTS_LIBC_HAS_VSTRDUPF
#include <string.h>
#else

#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <stdarg.h>

static inline HEDLEY_MALLOC HEDLEY_WARN_UNUSED_RESULT GRAVIER_TESTS_ATTRIBUTE_PARAMETER_IN(1) HEDLEY_PRINTF_FORMAT(1, 0) HEDLEY_NO_THROW
char *vstrdupf(char const *__restrict format, va_list args) {
        char *result;
        return vasprintf(&result, format, args) >= 0 ? result : 0;
}

#endif
