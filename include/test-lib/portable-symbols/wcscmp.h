#pragma once

#ifdef YALIBCT_LIBC_HAS_WCSCMP
#include <wchar.h>
#else

#include <stddef.h>

int wcscmp(const wchar_t *string1, const wchar_t *string2)
{
    size_t i = 0;

    do {
        if (string1[i] != string2[i])
            return (string1[i] < string2[i]) ? -1 : (string1[i] > string2[i]);
    } while (string1[i++] != L'\0');

    return 0;
}

#endif
