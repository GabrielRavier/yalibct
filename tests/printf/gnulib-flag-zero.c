#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <string.h>

YALIBCT_DIAGNOSTIC_IGNORE("-Wformat-extra-args")

static char buf[100];
static double zero = 0.0;
int main ()
{
#if !defined(YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS) && !defined(YALIBCT_DISABLE_PRINTF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS)
  if (sprintf (buf, "%010f", 1.0 / zero, 33, 44, 55) < 0
      || (strcmp (buf, "       inf") != 0
          && strcmp (buf, "  infinity") != 0))
    return 1;
#endif
  return 0;
}
