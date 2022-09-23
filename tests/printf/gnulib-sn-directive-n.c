#include <stdio.h>
#include <string.h>
#include <assert.h>
#if 1// HAVE_SNPRINTF
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
static char fmtstring[10];
static char buf[100];
int main ()
{
  int count = -1;
#ifndef YALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS
  /* Copy the format string.  Some systems (glibc with _FORTIFY_SOURCE=2)
     support %n in format strings in read-only memory but not in writable
     memory.  */
  strcpy (fmtstring, "%d %n");
  assert(my_snprintf (buf, 4, fmtstring, 12345, &count, 33, 44, 55) == 6);
  if (count != 6)
    return 1;
#endif
  return 0;
}
