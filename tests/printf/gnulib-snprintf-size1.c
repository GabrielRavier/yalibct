#include <stdio.h>
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
int main()
{
  static char buf[8] = { 'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F' };
  my_snprintf (buf, 1, "%d", 12345);
  return buf[1] != 'E';
}
