#include "test-deps/gnulib.h"
#include "test-lib/portable-symbols/printf.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

int main()
{
  struct rlimit limit;
  int ret;
  nocrash_init ();
  /* Some printf implementations allocate temporary space with malloc.  */
  /* On BSD systems, malloc() is limited by RLIMIT_DATA.  */
#ifdef RLIMIT_DATA
  if (getrlimit (RLIMIT_DATA, &limit) < 0)
    return 77;
  if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > 5000000)
    limit.rlim_max = 5000000;
  limit.rlim_cur = limit.rlim_max;
  if (setrlimit (RLIMIT_DATA, &limit) < 0)
    return 77;
#endif
  /* On Linux systems, malloc() is limited by RLIMIT_AS.  */
#ifdef RLIMIT_AS
  if (getrlimit (RLIMIT_AS, &limit) < 0)
    return 77;
  if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > 5000000)
    limit.rlim_max = 5000000;
  limit.rlim_cur = limit.rlim_max;
  if (setrlimit (RLIMIT_AS, &limit) < 0)
    return 77;
#endif
  /* Some printf implementations allocate temporary space on the stack.  */
#ifdef RLIMIT_STACK
  if (getrlimit (RLIMIT_STACK, &limit) < 0)
    return 77;
  if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > 5000000)
    limit.rlim_max = 5000000;
  limit.rlim_cur = limit.rlim_max;
  if (setrlimit (RLIMIT_STACK, &limit) < 0)
    return 77;
#endif
#ifndef YALIBCT_DISABLE_PRINTF_ERRNO_TESTS
  ret = printf ("%.5000000f", 1.0);
  return !(ret == 5000002 || (ret < 0 && errno == ENOMEM));
#else
  return 0;
#endif
}
