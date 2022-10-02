/* PR tree-optimization/93982 - Assignment incorrectly omitted by
   -foptimize-strlen
   { dg-do run }
   { dg-options "-O2 -Wall" } */

#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/__builtin_abort.h"

struct A { const char **a; };
const char *buf[5];

YALIBCT_ATTRIBUTE_NOIPA struct A
foo (const char *p)
{
  struct A r = { (const char **) p };
  r.a[0] = "12345678";
  r.a[1] = "";
  r.a[2] = "";
  r.a[3] = "";
  r.a[4] = "";
  return r;
}

int
main ()
{
  struct A r = foo ((char *) &buf[0]);
  if (!r.a[1] || r.a[1][0] != '\0')
    __builtin_abort ();
  return 0;
}
