#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <inttypes.h>
/* See page about "Parameter Validation" on msdn.microsoft.com.
   <https://docs.microsoft.com/en-us/cpp/c-runtime-library/parameter-validation>
   <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-invalid-parameter-handler-set-thread-local-invalid-parameter-handler>  */
static void cdecl
invalid_parameter_handler (const wchar_t *expression,
                           const wchar_t *function,
                           const wchar_t *file, unsigned int line,
                           uintptr_t dummy)
{
  exit (1);
}
#endif
static char fmtstring[10];
static char buf[100];
int main ()
{
  int count = -1;
#ifdef _MSC_VER
  _set_invalid_parameter_handler (invalid_parameter_handler);
#endif
#ifndef YALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS
  /* Copy the format string.  Some systems (glibc with _FORTIFY_SOURCE=2)
     support %n in format strings in read-only memory but not in writable
     memory.  */
  strcpy (fmtstring, "%d %n");
  if (sprintf (buf, fmtstring, 123, &count, 33, 44, 55) < 0
      || strcmp (buf, "123 ") != 0
      || count != 4)
    return 1;
#endif
  return 0;
}
