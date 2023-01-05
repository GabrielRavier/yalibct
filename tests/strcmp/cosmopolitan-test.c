/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "test-deps/cosmopolitan.h"
#include "test-lib/portable-symbols/zerosize_ptr.h"
#include "test-lib/portable-symbols/wcscmp.h"
#include "test-lib/portable-symbols/wcscasecmp.h"
#include "test-lib/portable-symbols/strcasecmp.h"

#ifndef __clang__
YALIBCT_DIAGNOSTIC_IGNORE("-Wstringop-overread")
#endif

int (*memcmpi)(const void *, const void *, size_t) = memcmp;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § emptiness                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strcmp, emptyString) {
  EXPECT_EQ(0, strcmp("", ""));
  EXPECT_NE(0, strcmp("", "a"));
}

TEST(strcasecmp, emptyString) {
  EXPECT_EQ(0, strcasecmp("", ""));
  EXPECT_NE(0, strcasecmp("", "a"));
}

TEST(wcscmp, emptyString) {
  EXPECT_EQ(0, wcscmp(L"", L""));
  EXPECT_NE(0, wcscmp(L"", L"a"));
}

TEST(wcscasecmp, emptyString) {
  EXPECT_EQ(0, wcscasecmp(L"", L""));
  EXPECT_NE(0, wcscasecmp(L"", L"a"));
}

TEST(strncmp, nullString) {
  char *s1 = zerosize_ptr();
  char *s2 = zerosize_ptr();
  ASSERT_NE(s1, s2);
  ASSERT_EQ(0, strncmp(s1, s2, 0));
}

#ifndef __clang__
YALIBCT_DIAGNOSTIC_IGNORE("-Wstringop-overflow")
#endif

TEST(strncmp, emptyString) {
  char *s1 = strcpy(malloc(1), "");
  char *s2 = strcpy(malloc(1), "");
  ASSERT_EQ(0, strncmp(s1, s2, 0));
  ASSERT_EQ(0, strncmp(s1, s2, 1));
  ASSERT_EQ(0, strncmp(s1, s2, -1));
  ASSERT_EQ(0, strncmp(s1, s1, -1));
  ASSERT_EQ(0, strncmp(s2, s2, -1));
  free(s2);
  free(s1);
}

TEST(strncasecmp, emptyString) {
  char *s1 = strcpy(malloc(1), "");
  char *s2 = strcpy(malloc(1), "");
  ASSERT_EQ(0, strncasecmp(s1, s2, 0));
  ASSERT_EQ(0, strncasecmp(s1, s2, 1));
  ASSERT_EQ(0, strncasecmp(s1, s2, -1));
  ASSERT_EQ(0, strncasecmp(s1, s1, -1));
  ASSERT_EQ(0, strncasecmp(s2, s2, -1));
  free(s2);
  free(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § inequality                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testInequality) {
  char *s1 = strcpy(malloc(2), "1");
  char *s2 = strcpy(malloc(1), "");
  ASSERT_EQ(0, strncmp(s1, s2, 0));
  ASSERT_EQ('1', strncmp(s1, s2, 1));
  ASSERT_EQ(-'1', strncmp(s2, s1, 1));
  free(s2);
  free(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § does it work?                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static const char str_char_min_with_null[] = { CHAR_MIN, '\0' };

TEST(memcmp, test) {
  EXPECT_EQ(memcmpi(str_char_min_with_null, str_char_min_with_null, 1), 0);
  EXPECT_LT(memcmpi("\177", str_char_min_with_null, 1), 0);
  EXPECT_GT(memcmpi(str_char_min_with_null, "\177", 1), 0);
  EXPECT_EQ(memcmpi("", "", 0), 0);
  EXPECT_EQ(memcmpi("a", "a", 1), 0);
  EXPECT_GT(memcmpi("a", "A", 1), 0);
  EXPECT_LT(memcmpi("A", "a", 1), 0);
  EXPECT_LT(memcmpi("\001", "\377", 1), 0);
  EXPECT_GT(memcmpi("\377", "\001", 1), 0);
  EXPECT_EQ(memcmpi("a", "aa", 1), 0);
  EXPECT_EQ(memcmpi("aa", "a", 1), 0);
  EXPECT_LT(memcmpi("a", "aa", 2), 0);
  EXPECT_GT(memcmpi("aa", "a", 2), 0);
  EXPECT_LT(memcmpi("aaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaa\377", 16), 0);
  EXPECT_GT(memcmpi("aaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaa\001", 16), 0);
  EXPECT_LT(memcmpi("aaaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaaa\377", 17), 0);
  EXPECT_GT(memcmpi("aaaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaaa\001", 17), 0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377", 32),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001", 32),
            0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377", 33),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001", 33),
            0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377", 34),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001", 34),
            0);
}

TEST(strcmp, testItWorks) {
  EXPECT_EQ(strcmp("", ""), 0);
  EXPECT_EQ(strcmp("a", "a"), 0);
  EXPECT_GT(strcmp("a", "A"), 0);
  EXPECT_LT(strcmp("A", "a"), 0);
  EXPECT_LT(strcmp("\001", "\377"), 0);
  EXPECT_GT(strcmp("\377", "\001"), 0);
  EXPECT_LT(strcmp("a", "aa"), 0);
  EXPECT_GT(strcmp("aa", "a"), 0);
  EXPECT_LT(strcmp("a\000", "aa\000"), 0);
  EXPECT_GT(strcmp("aa\000", "a\000"), 0);
  EXPECT_LT(strcmp("aaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmp("aaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmp("aaaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmp("aaaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
}

TEST(strcasecmp, testItWorks) {
  EXPECT_EQ(strcasecmp("", ""), 0);
  EXPECT_EQ(strcasecmp("a", "a"), 0);
  EXPECT_EQ(strcasecmp("a", "A"), 0);
  EXPECT_EQ(strcasecmp("A", "a"), 0);
  EXPECT_LT(strcasecmp("a", "z"), 0);
  EXPECT_GT(strcasecmp("z", "a"), 0);
  EXPECT_LT(strcasecmp("\001", "\377"), 0);
  EXPECT_GT(strcasecmp("\377", "\001"), 0);
  EXPECT_LT(strcasecmp("a", "aa"), 0);
  EXPECT_GT(strcasecmp("aa", "a"), 0);
  EXPECT_LT(strcasecmp("a\000", "aa\000"), 0);
  EXPECT_GT(strcasecmp("aa\000", "a\000"), 0);
  EXPECT_LT(strcasecmp("aaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcasecmp("aaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcasecmp("aaaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcasecmp("aaaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
}

TEST(wcscmp, testItWorks) {
  EXPECT_EQ(wcscmp(L"", L""), 0);
  EXPECT_EQ(wcscmp(L"a", L"a"), 0);
  EXPECT_GT(wcscmp(L"a", L"A"), 0);
  EXPECT_LT(wcscmp(L"A", L"a"), 0);
  EXPECT_LT(wcscmp(L"\001", L"\377"), 0);
  EXPECT_GT(wcscmp(L"\377", L"\001"), 0);
  EXPECT_LT(wcscmp(L"a", L"aa"), 0);
  EXPECT_GT(wcscmp(L"aa", L"a"), 0);
  EXPECT_LT(wcscmp(L"a", L"aa"), 0);
  EXPECT_GT(wcscmp(L"aa", L"a"), 0);
  EXPECT_LT(wcscmp(L"aaaaaaaaaaaaaaa\001", L"aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(wcscmp(L"aaaaaaaaaaaaaaa\377", L"aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(wcscmp(L"aaaaaaaaaaaaaaaa\001", L"aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(wcscmp(L"aaaaaaaaaaaaaaaa\377", L"aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
}

TEST(strcasecmp, testItWorksCase) {
  EXPECT_EQ(0, strcasecmp("hello", "HELLO"));
  EXPECT_EQ(0, strcasecmp("hello", "Hello"));
  EXPECT_EQ(0, strcasecmp("hello", "hello"));
  EXPECT_NE(0, strcasecmp("hello", "yello"));
}

TEST(wcscasecmp, testItWorksCase) {
  EXPECT_EQ(0, wcscasecmp(L"hello", L"HELLO"));
  EXPECT_EQ(0, wcscasecmp(L"hello", L"Hello"));
  EXPECT_EQ(0, wcscasecmp(L"hello", L"hello"));
  EXPECT_NE(0, wcscasecmp(L"hello", L"yello"));
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § nontrivial length                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testEqualManyNs) {
  char *s1 = malloc(PAGESIZE);
  char *s2 = malloc(PAGESIZE);
  memset(s1, 7, PAGESIZE);
  memset(s2, 7, PAGESIZE);
  s1[PAGESIZE - 1] = '\0';
  s2[PAGESIZE - 1] = '\0';
  for (unsigned i = 1; i <= 128; ++i) {
    ASSERT_EQ(0, strncmp(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 0));
    ASSERT_EQ(0, strncmp(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 1));
  }
  free(s2);
  free(s1);
}

TEST(strncmp, testNotEqualManyNs) {
  char *s1 = malloc(PAGESIZE);
  char *s2 = malloc(PAGESIZE);
  for (unsigned i = 1; i <= 128; ++i) {
    memset(s1, 7, PAGESIZE);
    memset(s2, 7, PAGESIZE);
    s1[PAGESIZE - 1] = (unsigned char)0;
    s2[PAGESIZE - 1] = (unsigned char)255;
    ASSERT_EQ(-255, strncmp(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 0));
    ASSERT_EQ(-255, strncmp(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 1));
  }
  free(s2);
  free(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § nul termination vs. explicit length        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testStringNulTerminatesBeforeExplicitLength) {
  const char kRdi[] = "";
  const char kRsi[] = "TZ=America/Los_Angeles";
  char *rdi = memcpy(malloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char *rsi = memcpy(malloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncmp(rdi, rdi, rdx), 0);
  EXPECT_LT(strncmp(rdi, rsi, rdx), 0);
  EXPECT_GT(strncmp(rsi, rdi, rdx), 0);
  free(rsi);
  free(rdi);
}

TEST(strncasecmp, testStringNulTerminatesBeforeExplicitLength) {
  const char kRdi[] = "";
  const char kRsi[] = "TZ=America/Los_Angeles";
  char *rdi = memcpy(malloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char *rsi = memcpy(malloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncasecmp(rdi, rdi, rdx), 0);
  EXPECT_LT(strncasecmp(rdi, rsi, rdx), 0);
  EXPECT_GT(strncasecmp(rsi, rdi, rdx), 0);
  free(rsi);
  free(rdi);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § two's complement bane                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strcmp, testTwosComplementBane_hasUnsignedBehavior) {
  EXPECT_EQ(strcmp("\200", "\200"), 0);
  EXPECT_LT(strcmp("\x7f", "\x80"), 0);
  EXPECT_GT(strcmp("\x80", "\x7f"), 0);
}

TEST(strcasecmp, testTwosComplementBane_hasUnsignedBehavior) {
  EXPECT_EQ(strcasecmp("\200", "\200"), 0);
  EXPECT_LT(strcasecmp("\x7f", "\x80"), 0);
  EXPECT_GT(strcasecmp("\x80", "\x7f"), 0);
}

TEST(memcmp, testTwosComplementBane_unsignedBehavior) {
  EXPECT_EQ(memcmpi("\200", "\200", 1), 0);
  EXPECT_LT(memcmpi("\177", "\200", 1), 0);
  EXPECT_GT(memcmpi("\200", "\177", 1), 0);
  EXPECT_EQ(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200", 34),
            0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\177",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200", 34),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\177", 34),
            0);
}

/*
  This (and the following wcsncmp test, along with the earlier
  "TwosComplementBane" tests, but those aren't relevant as they typically pass
  just fine) is testing some quite peculiar behavior - wcscmp specifically
  requires that the return value depend on whether the character in the first
  string is smaller than in the other, which means when wchar_t is signed and
  just fits in an int, a simple subtraction can't be used in the implementation
  when returning as it may overflow (whereas this is fine for strcmp, unless char
  is the same size as int). This appears to be how many implementations implement
  wcscmp, and it gives effectively bogus results on very large wchar_t values...


  Or is this wrong in assuming this to be non-standard behaviour ? Requires
  further study, but my reading of the standard indicates that most
  implementations are wrong...

  Quote from the C standard:
  > Unless explicitly stated otherwise, the functions described in this
  > subclause order two wide characters the same way as two integers of the
  > underlying integer type designated by `wchar_t`.
  > [...]
  > The `wcscmp` function returns an integer greater than, equal to, or less
  > than zero, accordingly as the wide string pointed to by s1 is greater than,
  > equal to, or less than the wide string pointed to by s2.
  - N3047 (latest C draft as of the time of writing)

  See also https://www.openwall.com/lists/musl/2023/01/04/1
*/
TEST(wcscmp, testTwosComplementBane) {
  wchar_t B1[2];
  wchar_t B2[2];
  B1[1] = L'\0';
  B2[1] = L'\0';
  B1[0] = WCHAR_MIN;
  B2[0] = WCHAR_MIN;
  EXPECT_EQ(wcscmp(B1, B2), 0);
  B1[0] = WCHAR_MAX;
  EXPECT_LT(0, wcscmp(B1, B2));
  B1[0] = WCHAR_MIN;
  B2[0] = WCHAR_MAX;
  EXPECT_LT(wcscmp(B1, B2), 0);
}

TEST(wcsncmp, testTwosComplementBane) {
  wchar_t B1;
  wchar_t B2;
  B1 = WCHAR_MIN;
  B2 = WCHAR_MIN;
  EXPECT_EQ(wcsncmp(&B1, &B2, 1), 0);
  B1 = WCHAR_MAX;
  EXPECT_GT(wcsncmp(&B1, &B2, 1), 0);
  B1 = WCHAR_MIN;
  B2 = WCHAR_MAX;
  EXPECT_LT(wcsncmp(&B1, &B2, 1), 0);
}

int main()
{
  strcmp_emptyString();
  strcasecmp_emptyString();
  wcscmp_emptyString();
  HEDLEY_CONCAT(wcscasecmp, _emptyString)();
  strncmp_nullString();
  strncmp_emptyString();
  strncasecmp_emptyString();
  strncmp_testInequality();
  memcmp_test();
  strcmp_testItWorks();
  strcasecmp_testItWorks();
  wcscmp_testItWorks();
  strcasecmp_testItWorksCase();
  HEDLEY_CONCAT(wcscasecmp, _testItWorksCase)();
  strncmp_testEqualManyNs();
  strncmp_testNotEqualManyNs();
  strncmp_testStringNulTerminatesBeforeExplicitLength();
  strncasecmp_testStringNulTerminatesBeforeExplicitLength();
  strcmp_testTwosComplementBane_hasUnsignedBehavior();
  strcasecmp_testTwosComplementBane_hasUnsignedBehavior();
  memcmp_testTwosComplementBane_unsignedBehavior();
  wcscmp_testTwosComplementBane();
  wcsncmp_testTwosComplementBane();
}
