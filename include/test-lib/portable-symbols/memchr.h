#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_MEMCHR
#include <string.h>
#else

#include <stddef.h>

#undef memchr
#define memchr yalibct_internal_memchr

static inline void *memchr(const void *ptr, int character, size_t length)
{
    const unsigned char *uchar_ptr = (const unsigned char *)ptr;

    while (length-- != 0) {
        if (*uchar_ptr == (unsigned char)character)
            return (void *)uchar_ptr;
        ++uchar_ptr;
    }
    return NULL;
}

#endif
