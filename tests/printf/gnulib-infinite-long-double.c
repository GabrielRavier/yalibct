#include "test-deps/gnulib.h"
#include <float.h>
#include <stdio.h>
#include <string.h>
static int
strisnan (const char *string, size_t start_index, size_t end_index)
{
  if (start_index < end_index)
    {
      if (string[start_index] == '-')
        start_index++;
      if (start_index + 3 <= end_index
          && memcmp (string + start_index, "nan", 3) == 0)
        {
          start_index += 3;
          if (start_index == end_index
              || (string[start_index] == '(' && string[end_index - 1] == ')'))
            return 1;
        }
    }
  return 0;
}
static char buf[10000];
static long double zeroL = 0.0L;
int main ()
{
  int result = 0;
  nocrash_init();
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
  if (sprintf (buf, "%Lf", 1.0L / zeroL) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lf", -1.0L / zeroL) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lf", zeroL / zeroL) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 1;
  if (sprintf (buf, "%Le", 1.0L / zeroL) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Le", -1.0L / zeroL) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Le", zeroL / zeroL) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 1;
  if (sprintf (buf, "%Lg", 1.0L / zeroL) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lg", -1.0L / zeroL) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lg", zeroL / zeroL) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 1;
#if ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_)) && !defined(YALIBCT_LIBC_HAS_SAME_LONG_DOUBLE_AS_DOUBLE)
/* Representation of an 80-bit 'long double' as an initializer for a sequence
   of 'unsigned int' words.  */
# ifdef WORDS_BIGENDIAN
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { ((unsigned int) (exponent) << 16) | ((unsigned int) (manthi) >> 16), \
       ((unsigned int) (manthi) << 16) | ((unsigned int) (mantlo) >> 16),   \
       (unsigned int) (mantlo) << 16                                        \
     }
# else
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { mantlo, manthi, exponent }
# endif
  { /* Quiet NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Le", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Lg", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
  }
  {
    /* Signalling NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Le", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Lg", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
  }
  { /* Pseudo-NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 4;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 4;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 4;
  }
  { /* Pseudo-Infinity.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 8;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 8;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 8;
  }
  { /* Pseudo-Zero.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 16;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 16;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 16;
  }
  { /* Unnormalized number.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 32;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 32;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 32;
  }
  { /* Pseudo-Denormal.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 64;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 64;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 64;
  }
#endif
#endif
  return result;
}
