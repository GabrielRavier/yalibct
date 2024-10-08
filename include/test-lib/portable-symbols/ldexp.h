// Derived from code with this license:
/* Emulation for ldexpl.
   Contributed by Paolo Bonzini

   Copyright 2002-2003, 2007-2022 Free Software Foundation, Inc.

   This file is part of gnulib.

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

#pragma once

#ifndef YALIBCT_LIBC_DOESNT_HAVE_LDEXP
#include <math.h>
#else

#include <math.h>

#undef ldexp
#define ldexp yalibct_internal_ldexp

static inline double ldexp(double x, int exp)
{
    double factor;
    int bit;
    /* Check for zero, nan and infinity. */
    if (!(isnan (x) || x + x == x))
    {
        if (exp < 0)
        {
            exp = -exp;
            factor = 0.5;
        }
        else
            factor = 2.0;

        if (exp > 0)
            for (bit = 1;;)
            {
                /* Invariant: Here bit = 2^i, factor = 2^-2^i or = 2^2^i,
                   and bit <= exp.  */
                if (exp & bit)
                    x *= factor;
                bit <<= 1;
                if (bit > exp)
                    break;
                factor = factor * factor;
            }
    }

    return x;
}

#endif
