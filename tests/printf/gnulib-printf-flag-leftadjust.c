#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  /* Check that a '-' flag is not annihilated by a negative width.  */
  if (sprintf (buf, "a%-*sc", -3, "b") < 0
      || strcmp (buf, "ab  c") != 0)
    return 1;
  return 0;
}
