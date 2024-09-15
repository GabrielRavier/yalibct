///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2017-2019, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief printf unit tests
//
///////////////////////////////////////////////////////////////////////////////

#include "test-deps/catch2.h"
#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/printf.h"
#include "test-lib/portable-symbols/NAN.h"
#include "test-lib/portable-symbols/INFINITY.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

YALIBCT_DIAGNOSTIC_IGNORE("-Wformat")

// dummy putchar
static char   printf_buffer[100];
static size_t printf_idx = 0U;

void _putchar(char character)
{
  printf_buffer[printf_idx++] = character;
}

void _out_fct(char character, void* arg)
{
  (void)arg;
  printf_buffer[printf_idx++] = character;
}


TEST_CASE(printf, "[]" ) {
  printf_idx = 0U;
  memset(printf_buffer, 0xCC, 100U);
  REQUIRE(printf("% d", 4232) == 5);
  REQUIRE(printf_buffer[5] == (char)0xCC);
  printf_buffer[5] = 0;
  //REQUIRE(!strcmp(printf_buffer, " 4232"));
}

#define ASSERT_STRLEN_EQ_BUFFER_snprintf(...) assert(snprintf(__VA_ARGS__) == strlen(buffer))

TEST_CASE(snprintf, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_snprintf(buffer, 100U, "%d", -1000);
  REQUIRE(!strcmp(buffer, "-1000"));

  assert(snprintf(buffer, 3U, "%d", -1000) == 5);
  REQUIRE(!strcmp(buffer, "-1"));
}

static void vprintf_builder_1(YALIBCT_ATTRIBUTE_MAYBE_UNUSED char* buffer, ...)
{
  va_list args;
  va_start(args, buffer);
  vprintf("%d", args);
  va_end(args);
}

#define ASSERT_STRLEN_EQ_BUFFER_vsnprintf(...) assert(vsnprintf(__VA_ARGS__) == strlen(buffer))

static void vsnprintf_builder_1(char* buffer, ...)
{
  va_list args;
  va_start(args, buffer);
  ASSERT_STRLEN_EQ_BUFFER_vsnprintf(buffer, 100U, "%d", args);
  va_end(args);
}

static void vsnprintf_builder_3(char* buffer, ...)
{
  va_list args;
  va_start(args, buffer);
  ASSERT_STRLEN_EQ_BUFFER_vsnprintf(buffer, 100U, "%d %d %s", args);
  va_end(args);
}


TEST_CASE(vprintf, "[]" ) {
  char buffer[100];
  printf_idx = 0U;
  memset(printf_buffer, 0xCC, 100U);
  vprintf_builder_1(buffer, 2345);
  REQUIRE(printf_buffer[4] == (char)0xCC);
  printf_buffer[4] = 0;
  //REQUIRE(!strcmp(printf_buffer, "2345"));
}


TEST_CASE(vsnprintf, "[]" ) {
  char buffer[100];

  vsnprintf_builder_1(buffer, -1);
  REQUIRE(!strcmp(buffer, "-1"));

  vsnprintf_builder_3(buffer, 3, -1000, "test");
  REQUIRE(!strcmp(buffer, "3 -1000 test"));
}

#define ASSERT_STRLEN_EQ_BUFFER_sprintf(...) assert(sprintf(__VA_ARGS__) == strlen(buffer))

TEST_CASE(space_flag, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% d", 42);
  REQUIRE(!strcmp(buffer, " 42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% d", -42);
  REQUIRE(!strcmp(buffer, "-42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 5d", 42);
  REQUIRE(!strcmp(buffer, "   42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 5d", -42);
  REQUIRE(!strcmp(buffer, "  -42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 15d", 42);
  REQUIRE(!strcmp(buffer, "             42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 15d", -42);
  REQUIRE(!strcmp(buffer, "            -42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 15d", -42);
  REQUIRE(!strcmp(buffer, "            -42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 15.3f", -42.987);
  REQUIRE(!strcmp(buffer, "        -42.987"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 15.3f", 42.987);
  REQUIRE(!strcmp(buffer, "         42.987"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% s", "Hello testing");
  REQUIRE(!strcmp(buffer, "Hello testing"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% d", 1024);
  REQUIRE(!strcmp(buffer, " 1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% d", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% i", 1024);
  REQUIRE(!strcmp(buffer, " 1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% i", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% u", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% u", 4294966272U);
  REQUIRE(!strcmp(buffer, "4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% o", 511);
  REQUIRE(!strcmp(buffer, "777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% o", 4294966785U);
  REQUIRE(!strcmp(buffer, "37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% x", 305441741);
  REQUIRE(!strcmp(buffer, "1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% x", 3989525555U);
  REQUIRE(!strcmp(buffer, "edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% X", 305441741);
  REQUIRE(!strcmp(buffer, "1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% X", 3989525555U);
  REQUIRE(!strcmp(buffer, "EDCB5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% c", 'x');
  REQUIRE(!strcmp(buffer, "x"));
}


TEST_CASE(plus_flag, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+d", 42);
  REQUIRE(!strcmp(buffer, "+42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+d", -42);
  REQUIRE(!strcmp(buffer, "-42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+5d", 42);
  REQUIRE(!strcmp(buffer, "  +42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+5d", -42);
  REQUIRE(!strcmp(buffer, "  -42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+15d", 42);
  REQUIRE(!strcmp(buffer, "            +42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+15d", -42);
  REQUIRE(!strcmp(buffer, "            -42"));

#ifndef YALIBCT_DISABLE_PRINTF_PLUS_FLAG_WITH_S_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+s", "Hello testing");
  REQUIRE(!strcmp(buffer, "Hello testing"));
#endif

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+d", 1024);
  REQUIRE(!strcmp(buffer, "+1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+d", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+i", 1024);
  REQUIRE(!strcmp(buffer, "+1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+i", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+u", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+u", 4294966272U);
  REQUIRE(!strcmp(buffer, "4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+o", 511);
  REQUIRE(!strcmp(buffer, "777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+o", 4294966785U);
  REQUIRE(!strcmp(buffer, "37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+x", 305441741);
  REQUIRE(!strcmp(buffer, "1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+x", 3989525555U);
  REQUIRE(!strcmp(buffer, "edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+X", 305441741);
  REQUIRE(!strcmp(buffer, "1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+X", 3989525555U);
  REQUIRE(!strcmp(buffer, "EDCB5433"));

#ifndef YALIBCT_DISABLE_PRINTF_PLUS_FLAG_WITH_C_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+c", 'x');
  REQUIRE(!strcmp(buffer, "x"));
#endif

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+.0d", 0);
  REQUIRE(!strcmp(buffer, "+"));
}


TEST_CASE(0_flag, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0d", 42);
  REQUIRE(!strcmp(buffer, "42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0ld", 42L);
  REQUIRE(!strcmp(buffer, "42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0d", -42);
  REQUIRE(!strcmp(buffer, "-42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%05d", 42);
  REQUIRE(!strcmp(buffer, "00042"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%05d", -42);
  REQUIRE(!strcmp(buffer, "-0042"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%015d", 42);
  REQUIRE(!strcmp(buffer, "000000000000042"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%015d", -42);
  REQUIRE(!strcmp(buffer, "-00000000000042"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%015.2f", 42.1234);
  REQUIRE(!strcmp(buffer, "000000000042.12"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%015.3f", 42.9876);
  REQUIRE(!strcmp(buffer, "00000000042.988"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%015.5f", -42.9876);
  REQUIRE(!strcmp(buffer, "-00000042.98760"));
}


TEST_CASE(minus_flag, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-d", 42);
  REQUIRE(!strcmp(buffer, "42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-d", -42);
  REQUIRE(!strcmp(buffer, "-42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-5d", 42);
  REQUIRE(!strcmp(buffer, "42   "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-5d", -42);
  REQUIRE(!strcmp(buffer, "-42  "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-15d", 42);
  REQUIRE(!strcmp(buffer, "42             "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-15d", -42);
  REQUIRE(!strcmp(buffer, "-42            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-0d", 42);
  REQUIRE(!strcmp(buffer, "42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-0d", -42);
  REQUIRE(!strcmp(buffer, "-42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-05d", 42);
  REQUIRE(!strcmp(buffer, "42   "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-05d", -42);
  REQUIRE(!strcmp(buffer, "-42  "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-015d", 42);
  REQUIRE(!strcmp(buffer, "42             "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-015d", -42);
  REQUIRE(!strcmp(buffer, "-42            "));

#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-d", 42);
  REQUIRE(!strcmp(buffer, "42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-d", -42);
  REQUIRE(!strcmp(buffer, "-42"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-5d", 42);
  REQUIRE(!strcmp(buffer, "42   "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-5d", -42);
  REQUIRE(!strcmp(buffer, "-42  "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-15d", 42);
  REQUIRE(!strcmp(buffer, "42             "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-15d", -42);
  REQUIRE(!strcmp(buffer, "-42            "));

#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-15.3e", -42.);
#if 1// PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  REQUIRE(!strcmp(buffer, "-4.200e+01     "));
#else
  REQUIRE(!strcmp(buffer, "e"));
#endif
#endif

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-15.3g", -42.);
#if 1// PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  REQUIRE(!strcmp(buffer, "-42            "));
#else
  REQUIRE(!strcmp(buffer, "g"));
#endif
#endif
}


TEST_CASE(hash_flag, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#.0x", 0);
  REQUIRE(!strcmp(buffer, ""));
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#.1x", 0);
  REQUIRE(!strcmp(buffer, "0"));
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#.0llx", (long long)0);
  REQUIRE(!strcmp(buffer, ""));
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#.8x", 0x614e);
  REQUIRE(!strcmp(buffer, "0x0000614e"));
#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_B_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer,"%#b", 6);
  REQUIRE(!strcmp(buffer, "0b110"));
#endif
}


TEST_CASE(specifier, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "Hello testing");
  REQUIRE(!strcmp(buffer, "Hello testing"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%s", "Hello testing");
  REQUIRE(!strcmp(buffer, "Hello testing"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%d", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%d", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%i", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%i", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%u", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%u", 4294966272U);
  REQUIRE(!strcmp(buffer, "4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%o", 511);
  REQUIRE(!strcmp(buffer, "777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%o", 4294966785U);
  REQUIRE(!strcmp(buffer, "37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%x", 305441741);
  REQUIRE(!strcmp(buffer, "1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%x", 3989525555U);
  REQUIRE(!strcmp(buffer, "edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%X", 305441741);
  REQUIRE(!strcmp(buffer, "1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%X", 3989525555U);
  REQUIRE(!strcmp(buffer, "EDCB5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%%");
  REQUIRE(!strcmp(buffer, "%"));
}


TEST_CASE(width, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1s", "Hello testing");
  REQUIRE(!strcmp(buffer, "Hello testing"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1d", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1d", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1i", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1i", -1024);
  REQUIRE(!strcmp(buffer, "-1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1u", 1024);
  REQUIRE(!strcmp(buffer, "1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1u", 4294966272U);
  REQUIRE(!strcmp(buffer, "4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1o", 511);
  REQUIRE(!strcmp(buffer, "777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1o", 4294966785U);
  REQUIRE(!strcmp(buffer, "37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1x", 305441741);
  REQUIRE(!strcmp(buffer, "1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1x", 3989525555U);
  REQUIRE(!strcmp(buffer, "edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1X", 305441741);
  REQUIRE(!strcmp(buffer, "1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1X", 3989525555U);
  REQUIRE(!strcmp(buffer, "EDCB5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%1c", 'x');
  REQUIRE(!strcmp(buffer, "x"));
}


TEST_CASE(width_20, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20s", "Hello");
  REQUIRE(!strcmp(buffer, "               Hello"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20d", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20d", -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20i", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20i", -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20u", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20u", 4294966272U);
  REQUIRE(!strcmp(buffer, "          4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20o", 511);
  REQUIRE(!strcmp(buffer, "                 777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20o", 4294966785U);
  REQUIRE(!strcmp(buffer, "         37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20x", 305441741);
  REQUIRE(!strcmp(buffer, "            1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20x", 3989525555U);
  REQUIRE(!strcmp(buffer, "            edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20X", 305441741);
  REQUIRE(!strcmp(buffer, "            1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20X", 3989525555U);
  REQUIRE(!strcmp(buffer, "            EDCB5433"));

#ifndef YALIBCT_DISABLE_PRINTF_C_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20c", 'x');
  REQUIRE(!strcmp(buffer, "                   x"));
#endif
}


TEST_CASE(width_asterisk_20, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*s", 20, "Hello");
  REQUIRE(!strcmp(buffer, "               Hello"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*d", 20, 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*d", 20, -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*i", 20, 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*i", 20, -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*u", 20, 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*u", 20, 4294966272U);
  REQUIRE(!strcmp(buffer, "          4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*o", 20, 511);
  REQUIRE(!strcmp(buffer, "                 777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*o", 20, 4294966785U);
  REQUIRE(!strcmp(buffer, "         37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*x", 20, 305441741);
  REQUIRE(!strcmp(buffer, "            1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*x", 20, 3989525555U);
  REQUIRE(!strcmp(buffer, "            edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*X", 20, 305441741);
  REQUIRE(!strcmp(buffer, "            1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*X", 20, 3989525555U);
  REQUIRE(!strcmp(buffer, "            EDCB5433"));

#ifndef YALIBCT_DISABLE_PRINTF_C_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*c", 20,'x');
  REQUIRE(!strcmp(buffer, "                   x"));
#endif
}


TEST_CASE(width_minus_20, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20s", "Hello");
  REQUIRE(!strcmp(buffer, "Hello               "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20d", 1024);
  REQUIRE(!strcmp(buffer, "1024                "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20d", -1024);
  REQUIRE(!strcmp(buffer, "-1024               "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20i", 1024);
  REQUIRE(!strcmp(buffer, "1024                "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20i", -1024);
  REQUIRE(!strcmp(buffer, "-1024               "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20u", 1024);
  REQUIRE(!strcmp(buffer, "1024                "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20.4f", 1024.1234);
  REQUIRE(!strcmp(buffer, "1024.1234           "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20u", 4294966272U);
  REQUIRE(!strcmp(buffer, "4294966272          "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20o", 511);
  REQUIRE(!strcmp(buffer, "777                 "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20o", 4294966785U);
  REQUIRE(!strcmp(buffer, "37777777001         "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20x", 305441741);
  REQUIRE(!strcmp(buffer, "1234abcd            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20x", 3989525555U);
  REQUIRE(!strcmp(buffer, "edcb5433            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20X", 305441741);
  REQUIRE(!strcmp(buffer, "1234ABCD            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20X", 3989525555U);
  REQUIRE(!strcmp(buffer, "EDCB5433            "));

#ifndef YALIBCT_DISABLE_PRINTF_C_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-20c", 'x');
  REQUIRE(!strcmp(buffer, "x                   "));
#endif

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "|%5d| |%-2d| |%5d|", 9, 9, 9);
  REQUIRE(!strcmp(buffer, "|    9| |9 | |    9|"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "|%5d| |%-2d| |%5d|", 10, 10, 10);
  REQUIRE(!strcmp(buffer, "|   10| |10| |   10|"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "|%5d| |%-12d| |%5d|", 9, 9, 9);
  REQUIRE(!strcmp(buffer, "|    9| |9           | |    9|"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "|%5d| |%-12d| |%5d|", 10, 10, 10);
  REQUIRE(!strcmp(buffer, "|   10| |10          | |   10|"));
}


TEST_CASE(width_0_minus_20, "[]" ) {
  char buffer[100];

#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20s", "Hello");
  REQUIRE(!strcmp(buffer, "Hello               "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20d", 1024);
  REQUIRE(!strcmp(buffer, "1024                "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20d", -1024);
  REQUIRE(!strcmp(buffer, "-1024               "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20i", 1024);
  REQUIRE(!strcmp(buffer, "1024                "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20i", -1024);
  REQUIRE(!strcmp(buffer, "-1024               "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20u", 1024);
  REQUIRE(!strcmp(buffer, "1024                "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20u", 4294966272U);
  REQUIRE(!strcmp(buffer, "4294966272          "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20o", 511);
  REQUIRE(!strcmp(buffer, "777                 "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20o", 4294966785U);
  REQUIRE(!strcmp(buffer, "37777777001         "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20x", 305441741);
  REQUIRE(!strcmp(buffer, "1234abcd            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20x", 3989525555U);
  REQUIRE(!strcmp(buffer, "edcb5433            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20X", 305441741);
  REQUIRE(!strcmp(buffer, "1234ABCD            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20X", 3989525555U);
  REQUIRE(!strcmp(buffer, "EDCB5433            "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%0-20c", 'x');
  REQUIRE(!strcmp(buffer, "x                   "));
#endif
}


TEST_CASE(padding_20, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020d", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020d", -1024);
  REQUIRE(!strcmp(buffer, "-0000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020i", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020i", -1024);
  REQUIRE(!strcmp(buffer, "-0000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020u", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020u", 4294966272U);
  REQUIRE(!strcmp(buffer, "00000000004294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020o", 511);
  REQUIRE(!strcmp(buffer, "00000000000000000777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020o", 4294966785U);
  REQUIRE(!strcmp(buffer, "00000000037777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020x", 305441741);
  REQUIRE(!strcmp(buffer, "0000000000001234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020x", 3989525555U);
  REQUIRE(!strcmp(buffer, "000000000000edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020X", 305441741);
  REQUIRE(!strcmp(buffer, "0000000000001234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%020X", 3989525555U);
  REQUIRE(!strcmp(buffer, "000000000000EDCB5433"));
}


TEST_CASE(padding_dot_20, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20d", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20d", -1024);
  REQUIRE(!strcmp(buffer, "-00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20i", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20i", -1024);
  REQUIRE(!strcmp(buffer, "-00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20u", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20u", 4294966272U);
  REQUIRE(!strcmp(buffer, "00000000004294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20o", 511);
  REQUIRE(!strcmp(buffer, "00000000000000000777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20o", 4294966785U);
  REQUIRE(!strcmp(buffer, "00000000037777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20x", 305441741);
  REQUIRE(!strcmp(buffer, "0000000000001234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20x", 3989525555U);
  REQUIRE(!strcmp(buffer, "000000000000edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20X", 305441741);
  REQUIRE(!strcmp(buffer, "0000000000001234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.20X", 3989525555U);
  REQUIRE(!strcmp(buffer, "000000000000EDCB5433"));
}


TEST_CASE(padding_hash_020, "[]" ) {
  char buffer[100];

#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020d", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020d", -1024);
  REQUIRE(!strcmp(buffer, "-0000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020i", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020i", -1024);
  REQUIRE(!strcmp(buffer, "-0000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020u", 1024);
  REQUIRE(!strcmp(buffer, "00000000000000001024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020u", 4294966272U);
  REQUIRE(!strcmp(buffer, "00000000004294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020o", 511);
  REQUIRE(!strcmp(buffer, "00000000000000000777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020o", 4294966785U);
  REQUIRE(!strcmp(buffer, "00000000037777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020x", 305441741);
  REQUIRE(!strcmp(buffer, "0x00000000001234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020x", 3989525555U);
  REQUIRE(!strcmp(buffer, "0x0000000000edcb5433"));

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_X_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020X", 305441741);
  REQUIRE(!strcmp(buffer, "0X00000000001234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#020X", 3989525555U);
  REQUIRE(!strcmp(buffer, "0X0000000000EDCB5433"));
#endif
#endif
}


TEST_CASE(padding_hash_20, "[]" ) {
  char buffer[100];

#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20d", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20d", -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20i", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20i", -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20u", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20u", 4294966272U);
  REQUIRE(!strcmp(buffer, "          4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20o", 511);
  REQUIRE(!strcmp(buffer, "                0777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20o", 4294966785U);
  REQUIRE(!strcmp(buffer, "        037777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20x", 305441741);
  REQUIRE(!strcmp(buffer, "          0x1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20x", 3989525555U);
  REQUIRE(!strcmp(buffer, "          0xedcb5433"));

#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_X_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20X", 305441741);
  REQUIRE(!strcmp(buffer, "          0X1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%#20X", 3989525555U);
  REQUIRE(!strcmp(buffer, "          0XEDCB5433"));
#endif
#endif
}


TEST_CASE(padding_20_dot_5, "[]" ) {
  char buffer[100];

#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5d", 1024);
  REQUIRE(!strcmp(buffer, "               01024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5d", -1024);
  REQUIRE(!strcmp(buffer, "              -01024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5i", 1024);
  REQUIRE(!strcmp(buffer, "               01024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5i", -1024);
  REQUIRE(!strcmp(buffer, "              -01024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5u", 1024);
  REQUIRE(!strcmp(buffer, "               01024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5u", 4294966272U);
  REQUIRE(!strcmp(buffer, "          4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5o", 511);
  REQUIRE(!strcmp(buffer, "               00777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5o", 4294966785U);
  REQUIRE(!strcmp(buffer, "         37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5x", 305441741);
  REQUIRE(!strcmp(buffer, "            1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.10x", 3989525555U);
  REQUIRE(!strcmp(buffer, "          00edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.5X", 305441741);
  REQUIRE(!strcmp(buffer, "            1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.10X", 3989525555U);
  REQUIRE(!strcmp(buffer, "          00EDCB5433"));
#endif
}


TEST_CASE(padding_neg_numbers, "[]" ) {
  char buffer[100];

  // space padding
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 1d", -5);
  REQUIRE(!strcmp(buffer, "-5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 2d", -5);
  REQUIRE(!strcmp(buffer, "-5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 3d", -5);
  REQUIRE(!strcmp(buffer, " -5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 4d", -5);
  REQUIRE(!strcmp(buffer, "  -5"));

  // zero padding
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%01d", -5);
  REQUIRE(!strcmp(buffer, "-5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%02d", -5);
  REQUIRE(!strcmp(buffer, "-5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%03d", -5);
  REQUIRE(!strcmp(buffer, "-05"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%04d", -5);
  REQUIRE(!strcmp(buffer, "-005"));
}


TEST_CASE(float_padding_neg_numbers, "[]" ) {
  char buffer[100];

  // space padding
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 3.1f", -5.);
  REQUIRE(!strcmp(buffer, "-5.0"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 4.1f", -5.);
  REQUIRE(!strcmp(buffer, "-5.0"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 5.1f", -5.);
  REQUIRE(!strcmp(buffer, " -5.0"));

#if 1// PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 6.1g", -5.);
  REQUIRE(!strcmp(buffer, "    -5"));

#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 6.1e", -5.);
  REQUIRE(!strcmp(buffer, "-5.0e+00"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% 10.1e", -5.);
  REQUIRE(!strcmp(buffer, "  -5.0e+00"));
#endif
#endif

  // zero padding
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%03.1f", -5.);
  REQUIRE(!strcmp(buffer, "-5.0"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%04.1f", -5.);
  REQUIRE(!strcmp(buffer, "-5.0"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%05.1f", -5.);
  REQUIRE(!strcmp(buffer, "-05.0"));

  // zero padding no decimal point
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%01.0f", -5.);
  REQUIRE(!strcmp(buffer, "-5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%02.0f", -5.);
  REQUIRE(!strcmp(buffer, "-5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%03.0f", -5.);
  REQUIRE(!strcmp(buffer, "-05"));

#if 1// PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%010.1e", -5.);
  REQUIRE(!strcmp(buffer, "-005.0e+00"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%07.0E", -5.);
  REQUIRE(!strcmp(buffer, "-05E+00"));
#endif

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%03.0g", -5.);
  REQUIRE(!strcmp(buffer, "-05"));
#endif
}

TEST_CASE(length, "[]" ) {
  char buffer[100];

#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.0s", "Hello testing");
  REQUIRE(!strcmp(buffer, ""));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0s", "Hello testing");
  REQUIRE(!strcmp(buffer, "                    "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.s", "Hello testing");
  REQUIRE(!strcmp(buffer, ""));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.s", "Hello testing");
  REQUIRE(!strcmp(buffer, "                    "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0d", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0d", -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.d", 0);
  REQUIRE(!strcmp(buffer, "                    "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0i", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.i", -1024);
  REQUIRE(!strcmp(buffer, "               -1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.i", 0);
  REQUIRE(!strcmp(buffer, "                    "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.u", 1024);
  REQUIRE(!strcmp(buffer, "                1024"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0u", 4294966272U);
  REQUIRE(!strcmp(buffer, "          4294966272"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.u", 0U);
  REQUIRE(!strcmp(buffer, "                    "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.o", 511);
  REQUIRE(!strcmp(buffer, "                 777"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0o", 4294966785U);
  REQUIRE(!strcmp(buffer, "         37777777001"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.o", 0U);
  REQUIRE(!strcmp(buffer, "                    "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.x", 305441741);
  REQUIRE(!strcmp(buffer, "            1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%50.x", 305441741);
  REQUIRE(!strcmp(buffer, "                                          1234abcd"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%50.x%10.u", 305441741, 12345);
  REQUIRE(!strcmp(buffer, "                                          1234abcd     12345"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0x", 3989525555U);
  REQUIRE(!strcmp(buffer, "            edcb5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.x", 0U);
  REQUIRE(!strcmp(buffer, "                    "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.X", 305441741);
  REQUIRE(!strcmp(buffer, "            1234ABCD"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.0X", 3989525555U);
  REQUIRE(!strcmp(buffer, "            EDCB5433"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%20.X", 0U);
  REQUIRE(!strcmp(buffer, "                    "));

#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%02.0u", 0U);
  REQUIRE(!strcmp(buffer, "  "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%02.0d", 0);
  REQUIRE(!strcmp(buffer, "  "));
#endif
#endif
}


TEST_CASE(float, "[]" ) {
  char buffer[100];

  // test special-case floats using math.h macros
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%8f", NAN);
  REQUIRE(!strcmp(buffer, "     nan"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%8f", INFINITY);
  REQUIRE(!strcmp(buffer, "     inf"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%-8f", -INFINITY);
  REQUIRE(!strcmp(buffer, "-inf    "));

#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+8e", INFINITY);
  REQUIRE(!strcmp(buffer, "    +inf"));
#endif

#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_F_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.4f", 3.1415354);
  REQUIRE(!strcmp(buffer, "3.1415"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.3f", 30343.1415354);
  REQUIRE(!strcmp(buffer, "30343.142"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.0f", 34.1415354);
  REQUIRE(!strcmp(buffer, "34"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.0f", 1.3);
  REQUIRE(!strcmp(buffer, "1"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.0f", 1.55);
  REQUIRE(!strcmp(buffer, "2"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.1f", 1.64);
  REQUIRE(!strcmp(buffer, "1.6"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.2f", 42.8952);
  REQUIRE(!strcmp(buffer, "42.90"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.9f", 42.8952);
  REQUIRE(!strcmp(buffer, "42.895200000"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.10f", 42.895223);
  REQUIRE(!strcmp(buffer, "42.8952230000"));

  // this testcase used to check that the precision is truncated to 9 digits.
  // a perfect working float should return the whole number
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.12f", 42.89522312345678);
  REQUIRE(!strcmp(buffer, "42.895223123457"));

  // this testcase used to check that the precision is truncated AND rounded to 9 digits.
  // a perfect working float should return the whole number
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.12f", 42.89522387654321);
  REQUIRE(!strcmp(buffer, "42.895223876543"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%6.2f", 42.8952);
  REQUIRE(!strcmp(buffer, " 42.90"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+6.2f", 42.8952);
  REQUIRE(!strcmp(buffer, "+42.90"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+5.1f", 42.9252);
  REQUIRE(!strcmp(buffer, "+42.9"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%f", 42.5);
  REQUIRE(!strcmp(buffer, "42.500000"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.1f", 42.5);
  REQUIRE(!strcmp(buffer, "42.5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%f", 42167.0);
  REQUIRE(!strcmp(buffer, "42167.000000"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.9f", -12345.987654321);
  REQUIRE(!strcmp(buffer, "-12345.987654321"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.1f", 3.999);
  REQUIRE(!strcmp(buffer, "4.0"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.0f", 3.5);
  REQUIRE(!strcmp(buffer, "4"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.0f", 4.5);
  REQUIRE(!strcmp(buffer, "4"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.0f", 3.49);
  REQUIRE(!strcmp(buffer, "3"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.1f", 3.49);
  REQUIRE(!strcmp(buffer, "3.5"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "a%-5.1f", 0.5);
  REQUIRE(!strcmp(buffer, "a0.5  "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "a%-5.1fend", 0.5);
  REQUIRE(!strcmp(buffer, "a0.5  end"));
#endif

#if 1// PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%G", 12345.678);
  REQUIRE(!strcmp(buffer, "12345.7"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.7G", 12345.678);
  REQUIRE(!strcmp(buffer, "12345.68"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.5G", 123456789.);
  REQUIRE(!strcmp(buffer, "1.2346E+08"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.6G", 12345.);
  REQUIRE(!strcmp(buffer, "12345"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+12.4g", 123456789.);
  REQUIRE(!strcmp(buffer, "  +1.235e+08"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.2G", 0.001234);
  REQUIRE(!strcmp(buffer, "0.0012"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+10.4G", 0.001234);
  REQUIRE(!strcmp(buffer, " +0.001234"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+012.4g", 0.00001234);
  REQUIRE(!strcmp(buffer, "+001.234e-05"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.3g", -1.2345e-308);
  REQUIRE(!strcmp(buffer, "-1.23e-308"));
#endif

#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%+.3E", 1.23e+308);
  REQUIRE(!strcmp(buffer, "+1.230E+308"));
#endif
#endif

  // the following comment appears to be wrong...
  // out of range for float: should switch to exp notation if supported, else empty
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.1f", 1E20);
#if 1// PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  REQUIRE(!strcmp(buffer, "100000000000000000000.0"));
#else
  REQUIRE(!strcmp(buffer, ""));
#endif

  // brute force float
  // uh yeah no I'm not gonna make this C++ just like that lmfao
  /*bool fail = false;
  std::stringstream str;
  str.precision(5);
  for (float i = -100000; i < 100000; i += 1) {
    sprintf(buffer, "%.5f", i / 10000);
    str.str("");
    str << std::fixed << i / 10000;
    fail = fail || !!strcmp(buffer, str.str().c_str());
  }
  REQUIRE(!fail);


#if 1//ndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  // brute force exp
  str.setf(std::ios::scientific, std::ios::floatfield);
  for (float i = -1e20; i < 1e20; i += 1e15) {
    sprintf(buffer, "%.5f", i);
    str.str("");
    str << i;
    fail = fail || !!strcmp(buffer, str.str().c_str());
  }
  REQUIRE(!fail);
#endif*/
}


TEST_CASE(types, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%i", 0);
  REQUIRE(!strcmp(buffer, "0"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%i", 1234);
  REQUIRE(!strcmp(buffer, "1234"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%i", 32767);
  REQUIRE(!strcmp(buffer, "32767"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%i", -32767);
  REQUIRE(!strcmp(buffer, "-32767"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%li", 30L);
  REQUIRE(!strcmp(buffer, "30"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%li", -2147483647L);
  REQUIRE(!strcmp(buffer, "-2147483647"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%li", 2147483647L);
  REQUIRE(!strcmp(buffer, "2147483647"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lli", 30LL);
  REQUIRE(!strcmp(buffer, "30"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lli", -9223372036854775807LL);
  REQUIRE(!strcmp(buffer, "-9223372036854775807"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lli", 9223372036854775807LL);
  REQUIRE(!strcmp(buffer, "9223372036854775807"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lu", 100000L);
  REQUIRE(!strcmp(buffer, "100000"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lu", 0xFFFFFFFFL);
  REQUIRE(!strcmp(buffer, "4294967295"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%llu", 281474976710656LLU);
  REQUIRE(!strcmp(buffer, "281474976710656"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%llu", 18446744073709551615LLU);
  REQUIRE(!strcmp(buffer, "18446744073709551615"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%zu", 2147483647UL);
  REQUIRE(!strcmp(buffer, "2147483647"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%zd", 2147483647UL);
  REQUIRE(!strcmp(buffer, "2147483647"));

  if (sizeof(size_t) == sizeof(long)) {
    ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%zi", -2147483647L);
    REQUIRE(!strcmp(buffer, "-2147483647"));
  }
  else {
    ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%zi", -2147483647LL);
    REQUIRE(!strcmp(buffer, "-2147483647"));
  }

#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_B_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%b", 60000);
  REQUIRE(!strcmp(buffer, "1110101001100000"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lb", 12345678L);
  REQUIRE(!strcmp(buffer, "101111000110000101001110"));
#endif

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%o", 60000);
  REQUIRE(!strcmp(buffer, "165140"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lo", 12345678L);
  REQUIRE(!strcmp(buffer, "57060516"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lx", 0x12345678L);
  REQUIRE(!strcmp(buffer, "12345678"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%llx", 0x1234567891234567LLU);
  REQUIRE(!strcmp(buffer, "1234567891234567"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lx", 0xabcdefabL);
  REQUIRE(!strcmp(buffer, "abcdefab"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%lX", 0xabcdefabL);
  REQUIRE(!strcmp(buffer, "ABCDEFAB"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%c", 'v');
  REQUIRE(!strcmp(buffer, "v"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%cv", 'w');
  REQUIRE(!strcmp(buffer, "wv"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%s", "A Test");
  REQUIRE(!strcmp(buffer, "A Test"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%hhu", 0xFFFFUL);
  REQUIRE(!strcmp(buffer, "255"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%hu", 0x123456UL);
  REQUIRE(!strcmp(buffer, "13398"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%s%hhi %hu", "Test", 10000, 0xFFFFFFFF);
  REQUIRE(!strcmp(buffer, "Test16 65535"));

#ifndef YALIBCT_DISABLE_PRINTF_T_LENGTH_MODIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%tx", &buffer[10] - &buffer[0]);
  REQUIRE(!strcmp(buffer, "a"));
#endif

// TBD
  if (sizeof(intmax_t) == sizeof(long)) {
    ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%ji", -2147483647L);
    REQUIRE(!strcmp(buffer, "-2147483647"));
  }
  else {
    ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%ji", -2147483647LL);
    REQUIRE(!strcmp(buffer, "-2147483647"));
  }
}


TEST_CASE(pointer, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%p", (void*)0x1234U);
  if (sizeof(void*) == 4U) {
    REQUIRE(!strcmp(buffer, "0x1234"));
  }
  else {
    REQUIRE(!strcmp(buffer, "0x1234"));
  }

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%p", (void*)0x12345678U);
  if (sizeof(void*) == 4U) {
    REQUIRE(!strcmp(buffer, "0x12345678"));
  }
  else {
    REQUIRE(!strcmp(buffer, "0x12345678"));
  }

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%p-%p", (void*)0x12345678U, (void*)0x7EDCBA98U);
  if (sizeof(void*) == 4U) {
    REQUIRE(!strcmp(buffer, "0x12345678-0x7edcba98"));
  }
  else {
    REQUIRE(!strcmp(buffer, "0x12345678-0x7edcba98"));
  }

  if (sizeof(uintptr_t) == sizeof(uint64_t)) {
    ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%p", (void*)(uintptr_t)0xFFFFFFFFU);
    REQUIRE(!strcmp(buffer, "0xffffffff"));
  }
  else {
    ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%p", (void*)(uintptr_t)0xFFFFFFFFU);
    REQUIRE(!strcmp(buffer, "0xffffffff"));
  }
}


TEST_CASE(unknown_flag, "[]" ) {
  char buffer[100];

  // UB and not widely supported
  /*ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%kmarco", 42, 37);
  REQUIRE(!strcmp(buffer, "%kmarco"));*/
}


TEST_CASE(string_length, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.4s", "This is a test");
  REQUIRE(!strcmp(buffer, "This"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.4s", "test");
  REQUIRE(!strcmp(buffer, "test"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.7s", "123");
  REQUIRE(!strcmp(buffer, "123"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.7s", "");
  REQUIRE(!strcmp(buffer, ""));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.4s%.2s", "123456", "abcdef");
  REQUIRE(!strcmp(buffer, "1234ab"));

  // UB and not widely supported
  /*ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.4.2s", "123456");
  REQUIRE(!strcmp(buffer, "%.4.2s"));*/

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.*s", 3, "123456");
  REQUIRE(!strcmp(buffer, "123"));
}


TEST_CASE(buffer_length, "[]" ) {
  char buffer[100];
  int ret;

  // UB and not widely supported
  /*ret = snprintf(NULL, 10, "%s", "Test");
  REQUIRE(ret == 4);*/
  ret = snprintf(NULL, 0, "%s", "Test");
  REQUIRE(ret == 4);

  buffer[0] = (char)0xA5;
  ret = snprintf(buffer, 0, "%s", "Test");
  REQUIRE(buffer[0] == (char)0xA5);
  REQUIRE(ret == 4);

  buffer[0] = (char)0xCC;
  assert(snprintf(buffer, 1, "%s", "Test") == 4);
  REQUIRE(buffer[0] == '\0');

  assert(snprintf(buffer, 2, "%s", "Hello") == 5);
  REQUIRE(!strcmp(buffer, "H"));
}


TEST_CASE(ret_value, "[]" ) {
  char buffer[100] ;
  int ret;

  ret = snprintf(buffer, 6, "0%s", "1234");
  REQUIRE(!strcmp(buffer, "01234"));
  REQUIRE(ret == 5);

  ret = snprintf(buffer, 6, "0%s", "12345");
  REQUIRE(!strcmp(buffer, "01234"));
  REQUIRE(ret == 6);  // '5' is truncated

  ret = snprintf(buffer, 6, "0%s", "1234567");
  REQUIRE(!strcmp(buffer, "01234"));
  REQUIRE(ret == 8);  // '567' are truncated

  ret = snprintf(buffer, 10, "hello, world");
  REQUIRE(ret == 12);

  ret = snprintf(buffer, 3, "%d", 10000);
  REQUIRE(ret == 5);
  REQUIRE(strlen(buffer) == 2U);
  REQUIRE(buffer[0] == '1');
  REQUIRE(buffer[1] == '0');
  REQUIRE(buffer[2] == '\0');
}


TEST_CASE(misc, "[]" ) {
  char buffer[100];

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%u%u%ctest%d %s", 5, 3000, 'a', -20, "bit");
  REQUIRE(!strcmp(buffer, "53000atest-20 bit"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.*f", 2, 0.33333333);
  REQUIRE(!strcmp(buffer, "0.33"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.*d", -1, 1);
  REQUIRE(!strcmp(buffer, "1"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.3s", "foobar");
  REQUIRE(!strcmp(buffer, "foo"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "% .0d", 0);
  REQUIRE(!strcmp(buffer, " "));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%10.5d", 4);
  REQUIRE(!strcmp(buffer, "     00004"));

  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%*sx", -3, "hi");
  REQUIRE(!strcmp(buffer, "hi x"));

#if 1// PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.*g", 2, 0.33333333);
  REQUIRE(!strcmp(buffer, "0.33"));
#endif

#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
  ASSERT_STRLEN_EQ_BUFFER_sprintf(buffer, "%.*e", 2, 0.33333333);
  REQUIRE(!strcmp(buffer, "3.33e-01"));
#endif
#endif
}

int main()
{
    test_printf();
    test_snprintf();
    test_vprintf();
    test_vsnprintf();
    test_space_flag();
    test_plus_flag();
    test_0_flag();
    test_minus_flag();
    test_hash_flag();
    test_specifier();
    test_width();
    test_width_20();
    test_width_asterisk_20();
    test_width_minus_20();
    test_width_0_minus_20();
    test_padding_20();
    test_padding_dot_20();
    test_padding_hash_020();
    test_padding_hash_20();
    test_padding_20_dot_5();
    test_padding_neg_numbers();
    test_float_padding_neg_numbers();
    test_length();
    test_float();
    test_types();
    test_pointer();
    test_unknown_flag();
    test_string_length();
    test_buffer_length();
    test_ret_value();
    test_misc();
}
