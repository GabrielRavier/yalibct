#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
#ifndef YALIBCT_DISABLE_PRINTF_APOSTROPHE_FLAG_TESTS
  if (sprintf (buf, "%'d %d", 1234567, 99) < 0
      || buf[strlen (buf) - 1] != '9')
    return 1;
#endif
  return 0;
}
