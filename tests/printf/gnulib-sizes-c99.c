/*
dnl Copyright (C) 2003, 2007-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
*/

#include "test-lib/compiler-features.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#if 1//HAVE_STDINT_H_WITH_UINTMAX
# include <stdint.h>
#endif
#if 1//HAVE_INTTYPES_H_WITH_UINTMAX
# include <inttypes.h>
#endif

YALIBCT_DIAGNOSTIC_IGNORE("-Wformat-extra-args")

static char buf[100];
int main ()
{
  int result = 0;
#if 1//HAVE_STDINT_H_WITH_UINTMAX || HAVE_INTTYPES_H_WITH_UINTMAX
  buf[0] = '\0';
  if (sprintf (buf, "%ju %d", (uintmax_t) 12345671, 33, 44, 55) < 0
      || strcmp (buf, "12345671 33") != 0)
    result |= 1;
#else
  result |= 1;
#endif
  buf[0] = '\0';
  if (sprintf (buf, "%zu %d", (size_t) 12345672, 33, 44, 55) < 0
      || strcmp (buf, "12345672 33") != 0)
    result |= 2;
  buf[0] = '\0';
#ifndef YALIBCT_DISABLE_PRINTF_T_LENGTH_MODIFIER_TESTS
  if (sprintf (buf, "%tu %d", (ptrdiff_t) 12345673, 33, 44, 55) < 0
      || strcmp (buf, "12345673 33") != 0)
    result |= 4;
#endif
  buf[0] = '\0';
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  if (sprintf (buf, "%Lg %d", (long double) 1.5, 33, 44, 55) < 0
      || strcmp (buf, "1.5 33") != 0)
    result |= 8;
#endif
  return result;
}
