/* Verify that calls to printf don't get eliminated even if their
   result on success can be computed at compile time (they can fail).
   The calls can still be transformed into those of other functions.
   { dg-require-effective-target unwrapped }
   { dg-require-effective-target fileio }
   { dg-prune-output "warning: warning: \[^\n\r\]* possibly used unsafely" }
   { dg-skip-if "requires io" { avr-*-* } }
   { dg-skip-if "requires io" { freestanding } } */

#include "test-deps/gcc.h"
#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

YALIBCT_ATTRIBUTE_NOIPA void
write_file (void)
{
  printf ("1");
  printf ("%c", '2');
  printf ("%c%c", '3', '4');
  printf ("%s", "5");
  printf ("%s%s", "6", "7");
#ifndef YALIBCT_DISABLE_PRINTF_I_CONVERSION_SPECIFIER_TESTS
  printf ("%i", 8);
#else
  printf("8");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
  printf ("%.1s\n", "9x");
#else
  printf("9");
#endif
}


int main (void)
{
  char *tmpfname = gcc_tmpnam (0);
  FILE *f = freopen (tmpfname, "w", stdout);
  if (!f)
    {
      perror ("fopen for writing");
      return 1;
    }

  write_file ();
  assert(fclose (f) == 0);

  f = fopen (tmpfname, "r");
  if (!f)
    {
      perror ("fopen for reading");
      assert(remove (tmpfname) == 0);
      return 1;
    }

  char buf[12] = "";
  if (1 != fscanf (f, "%s", buf))
    {
      perror ("fscanf");
      assert(fclose (f) == 0);
      assert(remove (tmpfname) == 0);
      return 1;
    }

  assert(fclose (f) == 0);
  assert(remove (tmpfname) == 0);

  if (strcmp (buf, "123456789") != 0)
    abort ();

  return 0;
}
