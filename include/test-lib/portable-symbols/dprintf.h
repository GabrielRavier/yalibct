#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_DPRINTF
#include <stdio.h>
#else

#include "test-lib/portable-symbols/vdprintf.h"

#undef dprintf
#define dprintf yalibct_internal_dprintf

int dprintf(int fd, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);

    int result = vdprintf(fd, format, arguments);

    va_end(arguments);
    return result;
}

#endif
