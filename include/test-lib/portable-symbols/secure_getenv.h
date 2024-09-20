#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_SECURE_GETENV
#include <stdlib.h>
#else

#include <stdlib.h>
#include <unistd.h>

char *
secure_getenv (char const *name)
{
#ifndef YALIBCT_LIBC_DOESNT_HAVE___SECURE_GETENV /* glibc */
  return __secure_getenv (name);
#elif !defined(YALIBCT_LIBC_DOESNT_HAVE_ISSETUGID) /* musl, OS X, FreeBSD, NetBSD, OpenBSD, Solaris, Minix */
  if (issetugid ())
    return NULL;
  return getenv (name);
#elif !defined(YALIBCT_LIBC_DOESNT_HAVE_GETUID) && !defined(YALIBCT_LIBC_DOESNT_HAVE_GETEUID) && !defined(YALIBCT_LIBC_DOESNT_HAVE_GETGID) && !defined(YALIBCT_LIBC_DOESNT_HAVE_GETEGID) /* other Unix */
  if (geteuid () != getuid () || getegid () != getgid ())
    return NULL;
  return getenv (name);
#elif defined _WIN32 && ! defined __CYGWIN__ /* native Windows */
  /* On native Windows, there is no such concept as setuid or setgid binaries.
     - Programs launched as system services have high privileges, but they don't
       inherit environment variables from a user.
     - Programs launched by a user with "Run as Administrator" have high
       privileges and use the environment variables, but the user has been asked
       whether he agrees.
     - Programs launched by a user without "Run as Administrator" cannot gain
       high privileges, therefore there is no risk. */
  return getenv (name);
#else
  return NULL;
#endif
}


#endif
