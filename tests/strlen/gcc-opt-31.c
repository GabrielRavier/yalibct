/* { dg-do run } */
/* { dg-options "-O2" } */

#include "test-lib/compiler-features.h"
#include "gcc-opt.h"

__attribute__((noinline)) YALIBCT_ATTRIBUTE_NOCLONE int
bar (char *p1, const char *q)
{
  strcpy (p1, "abcde");
  char *p2 = strchr (p1, '\0');
  strcpy (p2, q);
  char *p3 = strchr (p2, '\0');
  memcpy (p3, "x", 2);
  return strlen (p1);
}

int
main (void)
{
  char buffer[10];
  int res = bar (buffer, "foo");
  if (strcmp (buffer, "abcdefoox") != 0 || res != 9)
    abort ();
  return 0;
}
