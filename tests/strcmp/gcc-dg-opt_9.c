/* PR tree-optimization/92683 - strncmp incorrect result with equal substrings
   and nonconst bound
   { dg-do run }
   { dg-options "-O2 -Wall" } */

#include "../strlen/gcc-opt.h"
#include "test-lib/portable-symbols/__builtin_printf.h"
#include "test-lib/portable-symbols/__builtin_abort.h"
#include "test-lib/compiler-features.h"

#include <stddef.h>

#define ident(n) ident (n)

YALIBCT_ATTRIBUTE_NOCLONE __attribute__ ((noinline)) YALIBCT_ATTRIBUTE_NOIPA size_t
ident (size_t x)
{
  return x;
}

int nfails;

YALIBCT_ATTRIBUTE_NOCLONE __attribute__ ((noinline)) YALIBCT_ATTRIBUTE_NOIPA void
failure_on_line (int line)
{
  __builtin_printf ("failure on line %i\n", line);
  ++nfails;
}

#include "gcc-dg-opt_8.c"

int main (void)
{
  test_literal ();
  test_cst_array ();

  if (nfails)
    __builtin_abort ();
}
