/* Test strlen with 0 in the RSI register.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.
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

#ifdef WIDE
# define TEST_NAME "wcslen"
#else
# define TEST_NAME "strlen"
#endif /* WIDE */

#define TEST_MAIN
#include "test-deps/glibc/test-string.h"
#include "test-lib/compiler-features.h"

#ifdef WIDE
# include <wchar.h>
# define STRLEN wcslen
# define CHAR wchar_t
#else
# define STRLEN strlen
# define CHAR char
#endif /* WIDE */

impl_t yalibct_glibc_string_impls[] = {
    IMPL_INITIALIZER (STRLEN, 1),
};

typedef size_t (*proto_t) (const CHAR *);

typedef struct
{
  void (*fn) (void);
} parameter_t;

size_t
__attribute__ ((weak)) HEDLEY_NEVER_INLINE YALIBCT_ATTRIBUTE_NOCLONE
do_strlen (parameter_t *a, int zero, const CHAR *str)
{
  (void)zero;
  return CALL (a, str);
}

static int
test_main (void)
{
  test_init ();

  size_t size = page_size / sizeof (CHAR) - 1;
  CHAR *buf = (CHAR *) buf2;
  buf[size] = 0;

  parameter_t a;

  int ret = 0;
  FOR_EACH_IMPL (impl, 0)
    {
      a.fn = impl->fn;
      /* NB: Pass 0 in RSI.  */
      size_t res = do_strlen (&a, 0, buf);
      if (res != size)
	{
	  error (0, 0, "Wrong result in function %s: %zu != %zu",
		 impl->name, res, size);
	  ret = 1;
	}
    }

  return ret ? EXIT_FAILURE : EXIT_SUCCESS;
}

#include "test-deps/glibc/test-driver.h"
