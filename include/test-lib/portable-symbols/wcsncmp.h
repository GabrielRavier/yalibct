#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_WCSNCMP
#include <wchar.h>
#else

#include <stddef.h>

int wcsncmp(const wchar_t *string1, const wchar_t *string2, size_t n)
{
    size_t i = 0;

    do {
        if (n == 0)
            return 0;
        if (string1[i] != string2[i])
            return (string1[i] < string2[i]) ? -1 : (string1[i] > string2[i]);
        --n;
    } while (string1[i++] != L'\0');

    return 0;
}

#endif
