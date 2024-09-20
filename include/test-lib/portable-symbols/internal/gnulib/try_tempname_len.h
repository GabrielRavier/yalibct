// Derived from code with this license:
/* Copyright (C) 1991-2023 Free Software Foundation, Inc.
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

#include "test-lib/portable-symbols/getrandom.h"
#include "test-lib/portable-symbols/internal/gnulib/__set_errno.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

# define __getrandom getrandom

/* Use getrandom if it works, falling back on a 64-bit linear
   congruential generator that starts with Var's value
   mixed in with a clock's low-order bits if available.  */
typedef uint_fast64_t random_value;

# define __clock_gettime64 clock_gettime
# define __timespec64 timespec

/* Return the result of mixing the entropy from R and S.
   Assume that R and S are not particularly random,
   and that the result should look randomish to an untrained eye.  */

static random_value
mix_random_values (random_value r, random_value s)
{
  /* As this code is used only when high-quality randomness is neither
     available nor necessary, there is no need for fancier polynomials
     such as those in the Linux kernel's 'random' driver.  */
  return (2862933555777941757 * r + 3037000493) ^ s;
}

/* Set *R to a random value.
   Return true if *R is set to high-quality value taken from getrandom.
   Otherwise return false, falling back to a low-quality *R that might
   depend on S.

   This function returns false only when getrandom fails.
   On GNU systems this should happen only early in the boot process,
   when the fallback should be good enough for programs using tempname
   because any attacker likely has root privileges already.  */

static bool
random_bits (random_value *r, random_value s)
{
  /* Without GRND_NONBLOCK it can be blocked for minutes on some systems.  */
  if (__getrandom (r, sizeof *r, GRND_NONBLOCK) == sizeof *r)
    return true;

  /* If getrandom did not work, use ersatz entropy based on low-order
     clock bits.  On GNU systems getrandom should fail only
     early in booting, when ersatz should be good enough.
     Do not use ASLR-based entropy, as that would leak ASLR info into
     the resulting file name which is typically public.

     Of course we are in a state of sin here.  */

  random_value v = s;

#if /*_LIBC ||*/ (!defined(YALIBCT_LIBC_DOESNT_HAVE_CLOCK_REALTIME) && !defined(YALIBCT_LIBC_DOESNT_HAVE_CLOCK_GETTIME))
  struct __timespec64 tv;
  __clock_gettime64 (CLOCK_REALTIME, &tv);
  v = mix_random_values (v, tv.tv_sec);
  v = mix_random_values (v, tv.tv_nsec);
#endif

  *r = mix_random_values (v, clock ());
  return false;
}


#define RANDOM_VALUE_MAX UINT_FAST64_MAX
#define BASE_62_POWER (62LL * 62 * 62 * 62 * 62 * 62 * 62 * 62 * 62 * 62)
enum {
    BASE_62_DIGITS = 10 /* 62**10 < UINT_FAST64_MAX */
};

/* These are the characters used in temporary file names.  */
static const char letters[] =
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int
try_tempname_len (char *tmpl, int suffixlen, void *args,
                  int (*tryfunc) (char *, void *), size_t x_suffix_len)
{
  size_t len;
  char *XXXXXX;
  unsigned int count;
  int fd;
  int save_errno = errno;

  /* A lower bound on the number of temporary files to attempt to
     generate.  The maximum total number of temporary file names that
     can exist for a given template is 62**6.  It should never be
     necessary to try all of these combinations.  Instead if a reasonable
     number of names is tried (we define reasonable as 62**3) fail to
     give the system administrator the chance to remove the problems.
     This value requires that X_SUFFIX_LEN be at least 3.  */
#define ATTEMPTS_MIN (62 * 62 * 62)

  /* The number of times to attempt to generate a temporary file.  To
     conform to POSIX, this must be no smaller than TMP_MAX.  */
#if ATTEMPTS_MIN < TMP_MAX
  unsigned int attempts = TMP_MAX;
#else
  unsigned int attempts = ATTEMPTS_MIN;
#endif

  /* A random variable.  */
  random_value v = 0;

  /* A value derived from the random variable, and how many random
     base-62 digits can currently be extracted from VDIGBUF.  */
  random_value vdigbuf;
  int vdigits = 0;

  /* Least biased value for V.  If V is less than this, V can generate
     BASE_62_DIGITS unbiased digits.  Otherwise the digits are biased.  */
  random_value const biased_min
    = RANDOM_VALUE_MAX - RANDOM_VALUE_MAX % BASE_62_POWER;

  len = strlen (tmpl);
  if (len < x_suffix_len + suffixlen
      || strspn (&tmpl[len - x_suffix_len - suffixlen], "X") < x_suffix_len)
    {
      yalibct_internal_gnulib___set_errno (EINVAL);
      return -1;
    }

  /* This is where the Xs start.  */
  XXXXXX = &tmpl[len - x_suffix_len - suffixlen];

  for (count = 0; count < attempts; ++count)
    {
      for (size_t i = 0; i < x_suffix_len; i++)
        {
          if (vdigits == 0)
            {
              /* Worry about bias only if the bits are high quality.  */
              while (random_bits (&v, v) && biased_min <= v)
                ;

              vdigbuf = v;
              vdigits = BASE_62_DIGITS;
            }

          XXXXXX[i] = letters[vdigbuf % 62];
          vdigbuf /= 62;
          vdigits--;
        }

      fd = tryfunc (tmpl, args);
      if (fd >= 0)
        {
          yalibct_internal_gnulib___set_errno (save_errno);
          return fd;
        }
      else if (errno != EEXIST)
        return -1;
    }

  /* We got out of the loop because we ran out of combinations to try.  */
  yalibct_internal_gnulib___set_errno (EEXIST);
  return -1;
}
