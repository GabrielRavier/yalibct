/* PR tree-optimization/71707 */
/* { dg-do run } */
/* { dg-options "-O2 -fdump-tree-strlen" } */

#include "test-lib/compiler-features.h"
#include "gcc-opt.h"

char a[32];
size_t b;

__attribute__((noinline)) YALIBCT_ATTRIBUTE_NOCLONE char *
foo (void)
{
  char *p = (char *)memcpy (a, "a", 2) + 1;
  memcpy (&a[1], "b", 2);
  b = strlen (a) + strlen (&a[1]) + strlen (p);
  return p;
}

int
main ()
{
  if (foo () != &a[1] || b != 4)
    abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-times "strlen \\(" 0 "strlen1" } } */
