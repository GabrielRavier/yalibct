// Derived from code with this license:
/* Copyright (C) 1991-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_TEMP_FAILURE_RETRY
#include <unistd.h>
#else

/* Evaluate EXPRESSION, and repeat as long as it returns -1 with `errno'
   set to EINTR.  */

#undef TEMP_FAILURE_RETRY
# define TEMP_FAILURE_RETRY(expression) \
  (__extension__                                  \
    ({ long int __result;                             \
       do __result = (long int) (expression);                     \
       while (__result == -1L && errno == EINTR);                 \
       __result; }))

#endif
