#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <string.h>

YALIBCT_DIAGNOSTIC_IGNORE("-Wformat-extra-args")

static char buf[5000];
int main ()
{
  int result = 0;
#if 0//def __BEOS__ // Guess we'll see if it turns out BeOS is indeed this broken, but assume it's not
  /* On BeOS, this would crash and show a dialog box.  Avoid the crash.  */
  return 1;
#endif
#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
  if (sprintf (buf, "%.4000d %d", 1, 33, 44) < 4000 + 3)
    result |= 1;
  if (sprintf (buf, "%.4000f %d", 1.0, 33, 44) < 4000 + 5)
    result |= 2;
  if (sprintf (buf, "%.511f %d", 1.0, 33, 44) < 511 + 5
      || buf[0] != '1')
    result |= 4;
  if (sprintf (buf, "%.999f %d", 1.0, 33, 44) < 999 + 5
      || buf[0] != '1')
    result |= 4;
#endif
  return result;
}
