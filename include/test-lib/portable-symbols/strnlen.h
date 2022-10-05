#pragma once

#ifdef YALIBCT_LIBC_HAS_STRNLEN
#include <string.h>
#else

#include "test-lib/portable-symbols/memchr.h"

#undef strnlen
#define strnlen yalibct_internal_strnlen

static inline size_t strnlen(const char *string, size_t max_length)
{
    const char *memchr_result = memchr(string, '\0', max_length);
    if (memchr_result == NULL)
        return max_length;
    return (size_t)(memchr_result - string);
}

#endif
