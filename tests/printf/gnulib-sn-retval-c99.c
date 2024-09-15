#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <string.h>
#if 1//HAVE_SNPRINTF
# define my_snprintf snprintf
#else
# include <stdarg.h>
static int my_snprintf (char *buf, int size, const char *format, ...)
{
  va_list args;
  int ret;
  va_start (args, format);
  ret = vsnprintf (buf, size, format, args);
  va_end (args);
  return ret;
}
#endif

YALIBCT_DIAGNOSTIC_IGNORE_WFORMAT_TRUNCATION

static char buf[100];
int main ()
{
  strcpy (buf, "ABCDEF");
  if (my_snprintf (buf, 3, "%d %d", 4567, 89) != 7)
    return 1;
  if (my_snprintf (buf, 0, "%d %d", 4567, 89) != 7)
    return 2;
  if (my_snprintf (NULL, 0, "%d %d", 4567, 89) != 7)
    return 3;
  return 0;
}
