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
  printf ("%i", 8);
  printf ("%.1s\n", "9x");
}


int main (void)
{
#ifdef YALIBCT_LIBC_HAS_TMPNAM
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

  if (strcmp (buf, "123456789"))
    abort ();
#endif

  return 0;
}
