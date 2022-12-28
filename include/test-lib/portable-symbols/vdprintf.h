#pragma once

#ifdef YALIBCT_LIBC_HAS_VDPRINTF
#include <stdio.h>
#else

#include "test-lib/portable-symbols/vasnprintf.h"
#include "test-lib/portable-symbols/full_write.h"
#include <stdarg.h>
#include <limits.h>

#undef vdprintf
#define vdprintf yalibct_internal_vdprintf

int
vdprintf (int fd, const char *format, va_list args)
{
  char buf[2000];
  char *output;
  size_t len;
  size_t lenbuf = sizeof (buf);

  output = vasnprintf (buf, &lenbuf, format, args);
  len = lenbuf;

  if (!output)
    return -1;

  if (full_write (fd, output, len) < len)
    {
      if (output != buf)
        free (output);
      return -1;
    }

  if (output != buf)
    free (output);

  if (len > INT_MAX)
    {
      errno = EOVERFLOW;
      return -1;
    }

  return len;
}

#endif
