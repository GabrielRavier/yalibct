/* Split a double into fraction and mantissa.
   Copyright (C) 2007-2022 Free Software Foundation, Inc.

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

/* Written by Paolo Bonzini <bonzini@gnu.org>, 2003, and
   Bruno Haible <bruno@clisp.org>, 2007.  */

/* Specification.  */
#include <math.h>

#include <float.h>

/* This file assumes FLT_RADIX = 2.  If FLT_RADIX is a power of 2 greater
   than 2, or not even a power of 2, some rounding errors can occur, so that
   then the returned mantissa is only guaranteed to be <= 1.0, not < 1.0.  */

#ifdef YALIBCT_FREXP_INTERNAL_GEN_LONG_DOUBLE
# define YALIBCT_FREXP_INTERNAL_FUNC frexpl
# define YALIBCT_FREXP_INTERNAL_DOUBLE long double
# define YALIBCT_FREXP_INTERNAL_ISNAN isnan
# define YALIBCT_FREXP_INTERNAL_DECL_ROUNDING DECL_LONG_DOUBLE_ROUNDING
# define YALIBCT_FREXP_INTERNAL_BEGIN_ROUNDING() BEGIN_LONG_DOUBLE_ROUNDING ()
# define YALIBCT_FREXP_INTERNAL_END_ROUNDING() END_LONG_DOUBLE_ROUNDING ()
# define YALIBCT_FREXP_INTERNAL_L_(literal) literal##L
#elif defined YALIBCT_FREXP_INTERNAL_GEN_DOUBLE
#undef frexp
#define frexp yalibct_internal_frexp
# define YALIBCT_FREXP_INTERNAL_FUNC frexp
# define YALIBCT_FREXP_INTERNAL_DOUBLE double
# define YALIBCT_FREXP_INTERNAL_ISNAN isnan
# define YALIBCT_FREXP_INTERNAL_DECL_ROUNDING
# define YALIBCT_FREXP_INTERNAL_BEGIN_ROUNDING()
# define YALIBCT_FREXP_INTERNAL_END_ROUNDING()
# define YALIBCT_FREXP_INTERNAL_L_(literal) literal
#else
#error "Need to define the proper macro"
#endif

static inline YALIBCT_FREXP_INTERNAL_DOUBLE
YALIBCT_FREXP_INTERNAL_FUNC (YALIBCT_FREXP_INTERNAL_DOUBLE x, int *expptr)
{
  int sign;
  int exponent;
  YALIBCT_FREXP_INTERNAL_DECL_ROUNDING

  /* Test for NaN, infinity, and zero.  */
  if (isnan (x) || x + x == x)
    {
      *expptr = 0;
      return x;
    }

  sign = 0;
  if (x < 0)
    {
      x = - x;
      sign = -1;
    }

  YALIBCT_FREXP_INTERNAL_BEGIN_ROUNDING ();

  {
    /* Since the exponent is an 'int', it fits in 64 bits.  Therefore the
       loops are executed no more than 64 times.  */
    YALIBCT_FREXP_INTERNAL_DOUBLE pow2[64]; /* pow2[i] = 2^2^i */
    YALIBCT_FREXP_INTERNAL_DOUBLE powh[64]; /* powh[i] = 2^-2^i */
    int i;

    exponent = 0;
    if (x >= YALIBCT_FREXP_INTERNAL_L_(1.0))
      {
        /* A positive exponent.  */
        YALIBCT_FREXP_INTERNAL_DOUBLE pow2_i; /* = pow2[i] */
        YALIBCT_FREXP_INTERNAL_DOUBLE powh_i; /* = powh[i] */

        /* Invariants: pow2_i = 2^2^i, powh_i = 2^-2^i,
           x * 2^exponent = argument, x >= 1.0.  */
        for (i = 0, pow2_i = YALIBCT_FREXP_INTERNAL_L_(2.0), powh_i = YALIBCT_FREXP_INTERNAL_L_(0.5);
             ;
             i++, pow2_i = pow2_i * pow2_i, powh_i = powh_i * powh_i)
          {
            if (x >= pow2_i)
              {
                exponent += (1 << i);
                x *= powh_i;
              }
            else
              break;

            pow2[i] = pow2_i;
            powh[i] = powh_i;
          }
        /* Avoid making x too small, as it could become a denormalized
           number and thus lose precision.  */
        while (i > 0 && x < pow2[i - 1])
          {
            i--;
            powh_i = powh[i];
          }
        exponent += (1 << i);
        x *= powh_i;
        /* Here 2^-2^i <= x < 1.0.  */
      }
    else
      {
        /* A negative or zero exponent.  */
        YALIBCT_FREXP_INTERNAL_DOUBLE pow2_i; /* = pow2[i] */
        YALIBCT_FREXP_INTERNAL_DOUBLE powh_i; /* = powh[i] */

        /* Invariants: pow2_i = 2^2^i, powh_i = 2^-2^i,
           x * 2^exponent = argument, x < 1.0.  */
        for (i = 0, pow2_i = YALIBCT_FREXP_INTERNAL_L_(2.0), powh_i = YALIBCT_FREXP_INTERNAL_L_(0.5);
             ;
             i++, pow2_i = pow2_i * pow2_i, powh_i = powh_i * powh_i)
          {
            if (x < powh_i)
              {
                exponent -= (1 << i);
                x *= pow2_i;
              }
            else
              break;

            pow2[i] = pow2_i;
            powh[i] = powh_i;
          }
        /* Here 2^-2^i <= x < 1.0.  */
      }

    /* Invariants: x * 2^exponent = argument, and 2^-2^i <= x < 1.0.  */
    while (i > 0)
      {
        i--;
        if (x < powh[i])
          {
            exponent -= (1 << i);
            x *= pow2[i];
          }
      }
    /* Here 0.5 <= x < 1.0.  */
  }

  if (sign < 0)
    x = - x;

  YALIBCT_FREXP_INTERNAL_END_ROUNDING ();

  *expptr = exponent;
  return x;
}


# undef YALIBCT_FREXP_INTERNAL_FUNC
# undef YALIBCT_FREXP_INTERNAL_DOUBLE
# undef YALIBCT_FREXP_INTERNAL_ISNAN
# undef YALIBCT_FREXP_INTERNAL_DECL_ROUNDING
# undef YALIBCT_FREXP_INTERNAL_BEGIN_ROUNDING
# undef YALIBCT_FREXP_INTERNAL_END_ROUNDING
# undef YALIBCT_FREXP_INTERNAL_L_
