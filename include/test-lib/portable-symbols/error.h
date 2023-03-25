// Derived from code with this license:
/* Error handler for noninteractive utilities
   Copyright (C) 1990-1998, 2000-2007, 2009-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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
/* Written by David MacKenzie <djm@gnu.ai.mit.edu>.  */

#pragma once

#ifdef YALIBCT_LIBC_HAS_ERROR
#include <error.h>
#else

#include "test-lib/portable-symbols/getprogname.h"
#include "test-lib/portable-symbols/gettext.h"
#include "test-lib/hedley.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>

/* This variable is incremented each time 'error' is called.  */
static unsigned int error_message_count;

static inline void
yalibct_internal_error_print_errno_message (int errnum)
{
  char const *s;

  s = strerror (errnum);

  if (! s)
    s = gettext("Unknown system error");

  fprintf (stderr, ": %s", s);
}

static inline void HEDLEY_PRINTF_FORMAT (3, 0) HEDLEY_NON_NULL ((3))
yalibct_internal_error_error_tail (int status, int errnum, const char *message, va_list args)
{
    vfprintf (stderr, message, args);

  ++error_message_count;
  if (errnum)
    yalibct_internal_error_print_errno_message (errnum);
  putc ('\n', stderr);
  fflush (stderr);
  if (status)
    exit (status);
}


static inline int
yalibct_internal_error_is_open (int fd)
{
# if defined _WIN32 && ! defined __CYGWIN__
  /* On native Windows: The initial state of unassigned standard file
     descriptors is that they are open but point to an INVALID_HANDLE_VALUE.
     There is no fcntl, and the gnulib replacement fcntl does not support
     F_GETFL.  */
  return (HANDLE) _get_osfhandle (fd) != INVALID_HANDLE_VALUE;
# else
#  ifndef F_GETFL
#   error Please port fcntl to your platform
#  endif
  return 0 <= fcntl (fd, F_GETFL);
# endif
}

static inline void
yalibct_internal_error_flush_stdout (void)
{
  int stdout_fd;

  /* POSIX states that fileno (stdout) after fclose is unspecified.  But in
     practice it is not a problem, because stdout is statically allocated and
     the fd of a FILE stream is stored as a field in its allocated memory.  */
  stdout_fd = fileno (stdout);

  /* POSIX states that fflush (stdout) after fclose is unspecified; it
     is safe in glibc, but not on all other platforms.  fflush (NULL)
     is always defined, but too draconian.  */
  if (0 <= stdout_fd && yalibct_internal_error_is_open (stdout_fd))
    fflush (stdout);
}

/* If NULL, error will flush stdout, then print on stderr the program
   name, a colon and a space.  Otherwise, error will call this
   function without parameters instead.  */
static void (*error_print_progname) (void);

/* Print the program name and error message MESSAGE, which is a printf-style
   format string with optional args.
   If ERRNUM is nonzero, print its corresponding system error message.
   Exit with status STATUS if it is nonzero.  */
static inline void
error (int status, int errnum, const char *message, ...)
{
  va_list args;

  yalibct_internal_error_flush_stdout ();
  flockfile (stderr);
  if (error_print_progname)
    (*error_print_progname) ();
  else
      fprintf (stderr, "%s: ", getprogname());

  va_start (args, message);
  yalibct_internal_error_error_tail (status, errnum, message, args);
  va_end (args);

  funlockfile (stderr);
}

#endif
