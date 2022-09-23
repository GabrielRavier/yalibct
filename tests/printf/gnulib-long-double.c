#include <stdio.h>
#include <string.h>
static char buf[10000];
int main ()
{
  int result = 0;
  buf[0] = '\0';
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  if (sprintf (buf, "%Lf %d", 1.75L, 33, 44, 55) < 0
      || strcmp (buf, "1.750000 33") != 0)
    result |= 1;
  buf[0] = '\0';
  if (sprintf (buf, "%Le %d", 1.75L, 33, 44, 55) < 0
      || strcmp (buf, "1.750000e+00 33") != 0)
    result |= 2;
  buf[0] = '\0';
  if (sprintf (buf, "%Lg %d", 1.75L, 33, 44, 55) < 0
      || strcmp (buf, "1.75 33") != 0)
    result |= 4;
#endif
  return result;
}
