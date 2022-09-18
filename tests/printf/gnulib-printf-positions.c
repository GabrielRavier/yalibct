#include <stdio.h>
#include <string.h>
/* The string "%2$d %1$d", with dollar characters protected from the shell's
   dollar expansion (possibly an autoconf bug).  */
static char format[] = { '%', '2', '$', 'd', ' ', '%', '1', '$', 'd', '\0' };
static char buf[100];
int main ()
{
#ifndef YALIBCT_DISABLE_PRINTF_NUMBERED_ARGUMENTS_TESTS
  sprintf (buf, format, 33, 55);
  return (strcmp (buf, "55 33") != 0);
#else
  return 0;
#endif
}
