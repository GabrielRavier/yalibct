/* { dg-do run } */
/* { dg-options "-O2 -fdump-tree-strlen" } */

#include "test-lib/compiler-features.h"
#include "gcc-opt.h"

__attribute__((noinline)) YALIBCT_ATTRIBUTE_NOCLONE size_t
fn1 (char *p)
{
  strcpy (p, "foobar");
  return strlen (p + 2); // This strlen should be optimized into 4.
}

int
main (void)
{
  char p[] = "barfoo";
  if (fn1 (p) != 4)
    abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-times "strlen \\(" 0 "strlen1" } } */
