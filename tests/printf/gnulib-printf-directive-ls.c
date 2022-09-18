#include <stdio.h>
#include <wchar.h>
#include <string.h>
int main ()
{
  int result = 0;
  char buf[100];
#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS
  /* Test whether %ls works at all.
     This test fails on OpenBSD 4.0, IRIX 6.5, Solaris 2.6, Haiku, but not on
     Cygwin 1.5.  */
  {
    static const wchar_t wstring[] = { 'a', 'b', 'c', 0 };
    buf[0] = '\0';
    if (sprintf (buf, "%ls", wstring) < 0
        || strcmp (buf, "abc") != 0)
      result |= 1;
  }
  /* This test fails on IRIX 6.5, Solaris 2.6, Cygwin 1.5, Haiku (with an
     assertion failure inside libc), but not on OpenBSD 4.0.  */
  {
    static const wchar_t wstring[] = { 'a', 0 };
    buf[0] = '\0';
    if (sprintf (buf, "%ls", wstring) < 0
        || strcmp (buf, "a") != 0)
      result |= 2;
  }
  /* Test whether precisions in %ls are supported as specified in ISO C 99
     section 7.19.6.1:
       "If a precision is specified, no more than that many bytes are written
        (including shift sequences, if any), and the array shall contain a
        null wide character if, to equal the multibyte character sequence
        length given by the precision, the function would need to access a
        wide character one past the end of the array."
     This test fails on Solaris 10.  */
  {
    static const wchar_t wstring[] = { 'a', 'b', (wchar_t) 0xfdfdfdfd, 0 };
    buf[0] = '\0';
    if (sprintf (buf, "%.2ls", wstring) < 0
        || strcmp (buf, "ab") != 0)
      result |= 8;
  }
#endif
  return result;
}
