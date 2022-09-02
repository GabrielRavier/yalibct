// Adapted from code with this license:
// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#pragma once

#ifdef YALIBCT_LIBC_HAS_MEMCHR_INV
#include <string.h>
#else

#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

static inline YALIBCT_ATTRIBUTE_COLD HEDLEY_NO_RETURN void fortify_panic(const char *name)
{
    fprintf(stderr, "detected buffer overflow in %s\n", name);
    abort();
}

void __read_overflow(void) YALIBCT_ATTRIBUTE_ERROR("detected read beyond size of object (1st parameter)");

static void *__real_memchr_inv_check_bytes8(const uint8_t *start, uint8_t value, unsigned int bytes)
{
        while (bytes) {
                if (*start != value)
                        return (void *)start;
                start++;
                bytes--;
        }
        return NULL;
}

static inline void *__real_memchr_inv(const void *start, int c, size_t bytes)
{
    uint8_t value = c;
    uint64_t value64;
    unsigned int words, prefix;

    if (bytes <= 16)
        return __real_memchr_inv_check_bytes8(start, value, bytes);

    value64 = value;
#if BITS_PER_LONG == 64
    value64 *= 0x0101010101010101ULL;
#else
    value64 *= 0x01010101;
    value64 |= value64 << 32;
#endif

    prefix = (unsigned long)start % 8;
    if (prefix) {
        uint8_t *r;

        prefix = 8 - prefix;
        r = __real_memchr_inv_check_bytes8(start, value, prefix);
        if (r)
            return r;
        start += prefix;
        bytes -= prefix;
    }

    words = bytes / 8;

    while (words) {
        if (*(uint64_t *)start != value64)
            return __real_memchr_inv_check_bytes8(start, value, 8);
        start += 8;
        words--;
    }

    return __real_memchr_inv_check_bytes8(start, value, bytes % 8);
}

static inline void *memchr_inv(const void *const p, int c, size_t size)
{
    size_t p_size = __builtin_object_size(p, 0);

    if (__builtin_constant_p(size) && p_size < size)
        __read_overflow();
    if (p_size < size)
        fortify_panic(__func__);
    return __real_memchr_inv(p, c, size);
    
}

#endif
