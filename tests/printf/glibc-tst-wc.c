#define _GNU_SOURCE 1
#include "test-deps/glibc.h"
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <wctype.h>

int
do_test (void)
{
  wchar_t tmp[3];
  tmp[0] = '8';
  tmp[1] = '1';
  tmp[2] = 0;

  printf ("Test for wide character output with printf\n");

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_S_CONVERSION_SPECIFIER_TESTS
  printf ("with %%S: %S\n", tmp);
#else
  puts("with %S: 81");
#endif

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_C_CONVERSION_SPECIFIER_TESTS
  printf ("with %%C: %C\n", (wint_t) tmp[0]);
#else
  puts("with %C: 8");
#endif

  return 0;
}
