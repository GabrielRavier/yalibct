#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <assert.h>
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

int main()
{
  static char buf[8] = { 'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F' };
  assert(my_snprintf (buf, 1, "%d", 12345) == 5);
  return buf[1] != 'E';
}
