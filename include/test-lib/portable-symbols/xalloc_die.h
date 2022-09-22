// Derived from code with this license:
/* Report a memory allocation failure and exit.

   Copyright (C) 1997-2000, 2002-2004, 2006, 2009-2022 Free Software
   Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#pragma once

#ifdef YALIBCT_LIBC_HAS_XALLOC_DIE
#include <xalloc.h>
#else

#include "test-lib/portable-symbols/error.h"
#include <stdlib.h>
#include <libintl.h>

static inline void
xalloc_die (void)
{
  error (EXIT_FAILURE, 0, "%s", gettext("memory exhausted"));

  /* _Noreturn cannot be given to error, since it may return if
     its first argument is 0.  To help compilers understand the
     xalloc_die does not return, call abort.  Also, the abort is a
     safety feature if exit_failure is 0 (which shouldn't happen).  */
  abort ();
}

#endif
