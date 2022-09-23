/* Test of u8_strcat() function.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <string.h>
#include <stdint.h>

static inline uint8_t *
u8_strcat (uint8_t *dest, const uint8_t *src)
{
  return (uint8_t *) strcat ((char *) dest, (const char *) src);
}

#define UNIT uint8_t
#define U_STRCAT u8_strcat
#define MAGIC 0xBA
#include "gnulib-test.h"
