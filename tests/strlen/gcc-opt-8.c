/* { dg-do run } */
/* { dg-options "-O2 -fdump-tree-strlen" } */

#include "test-lib/compiler-features.h"
#include "gcc-opt.h"

/* Yes, there are people who write code like this.  */

__attribute__((noinline)) YALIBCT_ATTRIBUTE_NOCLONE char *
foo (int r)
{
  char buf[10] = "";
  strcat (buf, r ? "r" : "w");
  strcat (buf, "b");
  return strdup (buf);
}

__attribute__((noinline)) YALIBCT_ATTRIBUTE_NOCLONE char *
bar (int r)
{
  char buf[10] = {};
  strcat (buf, r ? "r" : "w");
  strcat (buf, "b");
  return strdup (buf);
}

int
main ()
{
  char *q = foo (1);
  if (q != NULL)
    {
      if (strcmp (q, "rb") != 0)
	abort ();
      free (q);
    }
  q = bar (0);
  if (q != NULL)
    {
      if (strcmp (q, "wb") != 0)
	abort ();
      free (q);
    }
  return 0;
}

/* { dg-final { scan-tree-dump-times "strlen \\(" 0 "strlen1" } } */
/* { dg-final { scan-tree-dump-times "strcpy \\(" 0 "strlen1" } } */
/* { dg-final { scan-tree-dump-times "strcat \\(" 0 "strlen1" } } */
/* { dg-final { scan-tree-dump-times "strchr \\(" 0 "strlen1" } } */
/* { dg-final { scan-tree-dump-times "stpcpy \\(" 0 "strlen1" } } */
