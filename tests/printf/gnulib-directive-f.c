#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <string.h>

YALIBCT_DIAGNOSTIC_IGNORE("-Wformat-extra-args")

static char buf[100];
static double zero = 0.0;
int main ()
{
  int result = 0;
  if (sprintf (buf, "%F %d", 1234567.0, 33, 44, 55) < 0
      || strcmp (buf, "1234567.000000 33") != 0)
    result |= 1;
  if (sprintf (buf, "%F", 1.0 / zero) < 0
      || (strcmp (buf, "INF") != 0 && strcmp (buf, "INFINITY") != 0))
    result |= 2;
  /* This catches a Cygwin 1.5.x bug.  */
  if (sprintf (buf, "%.F", 1234.0) < 0
      || strcmp (buf, "1234") != 0)
    result |= 4;
  return result;
}
