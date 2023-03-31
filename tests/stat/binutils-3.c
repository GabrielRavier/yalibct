/* Simulator options:
#sim: --sysroot=$pwd
*/
#include "test-lib/chdir-to-tmpdir.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void exit_cleanup()
{
    assert(remove("./Makefile") == 0);
    yalibct_chdir_to_tmpdir_remove_tmpdir();
}

int main ()
{
  /* Pick a regular file we know will always be in the sim builddir.  */
  char path[1024] = "./Makefile";
  struct stat buf;

  yalibct_chdir_to_tmpdir();
  FILE *fp = fopen("./Makefile", "w");
  assert(fp != NULL);
  assert(fclose(fp) == 0);

  if (stat (".", &buf) != 0
      || !S_ISDIR (buf.st_mode))
    {
      assert(fprintf (stderr, "cwd is not a directory\n") >= 0);
      exit_cleanup();
      return 1;
    }
  if (stat (path, &buf) != 0
      || !S_ISREG (buf.st_mode))
    {
      assert(fprintf (stderr, "%s: is not a regular file\n", path) >= 0);
      exit_cleanup();
      return 1;
    }
  //printf ("pass\n");
  exit_cleanup();
  exit (0);
}
