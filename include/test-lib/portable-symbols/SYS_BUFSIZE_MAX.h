#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_SYS_BUFSIZE_MAX
#include "sys-limits.h"
#else

#include <limits.h>

/* Maximum number of bytes to read or write in a single system call.
   This can be useful for system calls like sendfile on GNU/Linux,
   which do not handle more than MAX_RW_COUNT bytes correctly.
   The Linux kernel MAX_RW_COUNT is at least INT_MAX >> 20 << 20,
   where the 20 comes from the Hexagon port with 1 MiB pages; use that
   as an approximation, as the exact value may not be available to us.

   Using this also works around a serious Linux bug before 2.6.16; see
   <https://bugzilla.redhat.com/show_bug.cgi?id=612839>.

   Using this also works around a Tru64 5.1 bug, where attempting
   to read INT_MAX bytes fails with errno == EINVAL.  See
   <https://lists.gnu.org/r/bug-gnu-utils/2002-04/msg00010.html>.

   Using this is likely to work around similar bugs in other operating
   systems.  */

enum { SYS_BUFSIZE_MAX = INT_MAX >> 20 << 20 };

#endif
