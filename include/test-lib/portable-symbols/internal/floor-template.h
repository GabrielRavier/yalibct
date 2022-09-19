// Derived from code with this license:
/* Round towards negative infinity.
   Copyright (C) 2007, 2010-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <float.h>

#ifdef YALIBCT_FLOOR_INTERNAL_GEN_LONG_DOUBLE
# define YALIBCT_FLOOR_INTERNAL_FUNC floorl
# define YALIBCT_FLOOR_INTERNAL_DOUBLE long double
# define YALIBCT_FLOOR_INTERNAL_MANT_DIG LDBL_MANT_DIG
# define YALIBCT_FLOOR_INTERNAL_L_(literal) literal##L
#elif defined YALIBCT_FLOOR_INTERNAL_GEN_DOUBLE
#undef floor
#define floor yalibct_internal_floor
# define YALIBCT_FLOOR_INTERNAL_FUNC floor
# define YALIBCT_FLOOR_INTERNAL_DOUBLE double
# define YALIBCT_FLOOR_INTERNAL_MANT_DIG DBL_MANT_DIG
# define YALIBCT_FLOOR_INTERNAL_L_(literal) literal
#elif defined YALIBCT_FLOOR_INTERNAL_GEN_FLOAT
# define YALIBCT_FLOOR_INTERNAL_FUNC floorf
# define YALIBCT_FLOOR_INTERNAL_DOUBLE float
# define YALIBCT_FLOOR_INTERNAL_MANT_DIG FLT_MANT_DIG
# define YALIBCT_FLOOR_INTERNAL_L_(literal) literal##f
#else
#error "Need to define the proper macro"
#endif

/* MSVC with option -fp:strict refuses to compile constant initializers that
   contain floating-point operations.  Pacify this compiler.  */
#if defined _MSC_VER && !defined __clang__
// Note: Figure out whether this can be turned on again if we get there and this is still a problem
//# pragma fenv_access (off)
#endif

/* 2^(MANT_DIG-1).  */
static const YALIBCT_FLOOR_INTERNAL_DOUBLE YALIBCT_FLOOR_INTERNAL_TWO_MANT_DIG =
  /* Assume MANT_DIG <= 5 * 31.
     Use the identity
       n = floor(n/5) + floor((n+1)/5) + ... + floor((n+4)/5).  */
  (YALIBCT_FLOOR_INTERNAL_DOUBLE) (1U << ((YALIBCT_FLOOR_INTERNAL_MANT_DIG - 1) / 5))
  * (YALIBCT_FLOOR_INTERNAL_DOUBLE) (1U << ((YALIBCT_FLOOR_INTERNAL_MANT_DIG - 1 + 1) / 5))
  * (YALIBCT_FLOOR_INTERNAL_DOUBLE) (1U << ((YALIBCT_FLOOR_INTERNAL_MANT_DIG - 1 + 2) / 5))
  * (YALIBCT_FLOOR_INTERNAL_DOUBLE) (1U << ((YALIBCT_FLOOR_INTERNAL_MANT_DIG - 1 + 3) / 5))
  * (YALIBCT_FLOOR_INTERNAL_DOUBLE) (1U << ((YALIBCT_FLOOR_INTERNAL_MANT_DIG - 1 + 4) / 5));

static inline YALIBCT_FLOOR_INTERNAL_DOUBLE
YALIBCT_FLOOR_INTERNAL_FUNC (YALIBCT_FLOOR_INTERNAL_DOUBLE x)
{
  /* The use of 'volatile' guarantees that excess precision bits are dropped
     at each addition step and before the following comparison at the caller's
     site.  It is necessary on x86 systems where double-floats are not IEEE
     compliant by default, to avoid that the results become platform and compiler
     option dependent.  'volatile' is a portable alternative to gcc's
     -ffloat-store option.  */
  volatile YALIBCT_FLOOR_INTERNAL_DOUBLE y = x;
  volatile YALIBCT_FLOOR_INTERNAL_DOUBLE z = y;

  if (z > YALIBCT_FLOOR_INTERNAL_L_(0.0))
    {
      /* For 0 < x < 1, return +0.0 even if the current rounding mode is
         FE_DOWNWARD.  */
      if (z < YALIBCT_FLOOR_INTERNAL_L_(1.0))
        z = YALIBCT_FLOOR_INTERNAL_L_(0.0);
      /* Avoid rounding errors for values near 2^k, where k >= MANT_DIG-1.  */
      else if (z < YALIBCT_FLOOR_INTERNAL_TWO_MANT_DIG)
        {
          /* Round to the next integer (nearest or up or down, doesn't matter).  */
          z += YALIBCT_FLOOR_INTERNAL_TWO_MANT_DIG;
          z -= YALIBCT_FLOOR_INTERNAL_TWO_MANT_DIG;
          /* Enforce rounding down.  */
          if (z > y)
            z -= YALIBCT_FLOOR_INTERNAL_L_(1.0);
        }
    }
  else if (z < YALIBCT_FLOOR_INTERNAL_L_(0.0))
    {
      /* Avoid rounding errors for values near -2^k, where k >= MANT_DIG-1.  */
      if (z > - YALIBCT_FLOOR_INTERNAL_TWO_MANT_DIG)
        {
          /* Round to the next integer (nearest or up or down, doesn't matter).  */
          z -= YALIBCT_FLOOR_INTERNAL_TWO_MANT_DIG;
          z += YALIBCT_FLOOR_INTERNAL_TWO_MANT_DIG;
          /* Enforce rounding down.  */
          if (z > y)
            z -= YALIBCT_FLOOR_INTERNAL_L_(1.0);
        }
    }
  return z;
}

# undef YALIBCT_FLOOR_INTERNAL_FUNC
# undef YALIBCT_FLOOR_INTERNAL_DOUBLE
# undef YALIBCT_FLOOR_INTERNAL_MANT_DIG
# undef YALIBCT_FLOOR_INTERNAL_L_
