// Derived from code with this license:
/* An interface to read and write that retries after interrupts.

   Copyright (C) 1993-1994, 1998, 2002-2006, 2009-2022 Free Software
   Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "test-lib/portable-symbols/SYS_BUFSIZE_MAX.h"
#include <unistd.h>
#include <errno.h>

#ifdef YALIBCT_SAFE_READ_WRITE_INTERNAL_IS_WRITE
# define YALIBCT_SAFE_READ_WRITE_INTERNAL_safe_rw yalibct_internal_gnulib_safe_write
# define YALIBCT_SAFE_READ_WRITE_INTERNAL_rw write
#else
#error "TODO"
#endif

#ifdef EINTR
# define YALIBCT_SAFE_READ_WRITE_INTERNAL_IS_EINTR(x) ((x) == EINTR)
#else
# define YALIBCT_SAFE_READ_WRITE_INTERNAL_IS_EINTR(x) 0
#endif

/* Read(write) up to COUNT bytes at BUF from(to) descriptor FD, retrying if
   interrupted.  Return the actual number of bytes read(written), zero for EOF,
   or SAFE_READ_ERROR(SAFE_WRITE_ERROR) upon error.  */
size_t
YALIBCT_SAFE_READ_WRITE_INTERNAL_safe_rw (int fd, void const *buf, size_t count)
{
  for (;;)
    {
      ssize_t result = YALIBCT_SAFE_READ_WRITE_INTERNAL_rw (fd, buf, count);

      if (0 <= result)
        return result;

      if (YALIBCT_SAFE_READ_WRITE_INTERNAL_IS_EINTR (errno))
        continue;
      else if (errno == EINVAL && SYS_BUFSIZE_MAX < count)
        count = SYS_BUFSIZE_MAX;
      else
        return result;
    }
}


# undef YALIBCT_SAFE_READ_WRITE_INTERNAL_safe_rw
# undef YALIBCT_SAFE_READ_WRITE_INTERNAL_rw
