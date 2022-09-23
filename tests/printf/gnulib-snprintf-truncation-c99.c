#include <stdio.h>
#include <string.h>
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
static char buf[100];
int main ()
{
  strcpy (buf, "ABCDEF");
  assert(my_snprintf (buf, 3, "%d %d", 4567, 89) == 7);
  if (memcmp (buf, "45\0DEF", 6) != 0)
    return 1;
  return 0;
}
