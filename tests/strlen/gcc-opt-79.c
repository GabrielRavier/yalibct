/* PR tree-optimization/91914 - Invalid strlen folding for offset into struct
   { dg-do run }
   { dg-options "-O2 -Wall" } */

#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/printf.h"
#include <stdio.h>
#include <stdlib.h>

#define assert(expr)						\
  ((expr)							\
   ? (void)0							\
   : (printf ("%s:%i: assertion failed: %s\n",	\
			__FILE__, __LINE__, #expr),		\
      abort ()))

extern __SIZE_TYPE__ strlen (const char*);

struct stringpool_t
{
  char s1[sizeof ("1")];
  char s2[sizeof ("12")];
  char s3[sizeof ("123")];
  char s4[sizeof ("1234")];
  char s5[sizeof ("12345")];
};

static const struct stringpool_t stringpool_contents =
  {
   "1", "12", "123", "1234", "12345"
  };

#define stringpool ((const char *) &stringpool_contents)

volatile int i0 = 0, i2 = 2, i5 = 5, i9 = 9, i14 = 14;

int main (void)
{
  /* These shouldn't trigger warnings.  */
  assert (strlen (stringpool) == 1);
  assert (strlen (stringpool + 2) == 2);
  assert (strlen (stringpool + 5) == 3);
  assert (strlen (stringpool + 9) == 4);
  assert (strlen (stringpool + 14) == 5);

  assert (strlen (stringpool + i0) == 1);
// Old versions of GCC have a bug that miscompile this when optimizing - not the libc's fault, I guess
#if !(!HEDLEY_GNUC_VERSION_CHECK(9, 4, 0) && __OPTIMIZE__)
  assert (strlen (stringpool + i2) == 2);
  assert (strlen (stringpool + i5) == 3);
  assert (strlen (stringpool + i9) == 4);
  assert (strlen (stringpool + i14) == 5);
#endif
}
