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
#include "test-lib/portable-symbols/strnlen.h"

YALIBCT_DIAGNOSTIC_IGNORE("-Wcomment")
#ifndef __clang__
YALIBCT_DIAGNOSTIC_IGNORE("-Wstringop-overread")
#endif

char u8[] = "utf-8 ☻";
//char16_t u16[] = u"utf16 ☻";
wchar_t u32[] = L"utf32 ☻";

size_t strlen_pure(const char *s) {
  size_t n = 0;
  while (*s++) ++n;
  return n;
}

TEST(strlen, usageExample_c11) {
  _Alignas(16) char ugh[] = "eeeeeeeeeeeeeee\017";
  EXPECT_EQ(1, strlen_pure(ugh + 15));
  EXPECT_EQ(1, strlen(ugh + 15));
  EXPECT_EQ(6 + 3, strlen(u8));
  EXPECT_EQ(6 + 3, strlen_pure(u8));
  //EXPECT_EQ(7, strlen16(u16));
  EXPECT_EQ(7, wcslen(u32));
}

TEST(strlen, usageExample_c99) {
  EXPECT_EQ(6 + 3, strlen(u8));
  //EXPECT_EQ(7, strlen16(u16));
  EXPECT_EQ(7, wcslen(u32));
}

TEST(strlen, whatMemoryLooksLike) {
  /*EXPECT_BINEQ(u"utf-8 Γÿ╗ ", u8); /* ← thompson-pike encoded *//*
  EXPECT_BINEQ(u"u t f 1 6   ;&  ", u16);
  EXPECT_BINEQ(u"u   t   f   3   2       ;&      ", u32);*/
}

TEST(strlen, test_const) {
  const char buf[] = "hellothere";
  ASSERT_EQ(0, strlen(""));
  ASSERT_EQ(0, strnlen("e", 0));
  ASSERT_EQ(10, strlen(buf));
}

TEST(strlen, test_nonconst) {
  char buf[256];
  unsigned i;
  for (i = 0; i < 255; ++i) buf[i] = i + 1;
  buf[i] = '\0';
  ASSERT_EQ(255, strlen(buf));
}

TEST(strnlen, testconst) {
  // One of those shit functions that should never have been standardized...
  //ASSERT_EQ(0, strnlen_s(NULL, 3));
  ASSERT_EQ(0, strnlen("", 3));
  ASSERT_EQ(0, strnlen("a", 0));
  ASSERT_EQ(3, strnlen("123", 3));
  ASSERT_EQ(2, strnlen("123", 2));
  ASSERT_EQ(3, strnlen("123", 4));
}

TEST(strlen, testnonconst) {
  /* this test case is a great example of why we need:
       "m"(*(char(*)[0x7fffffff])StR)
     rather than:
       "m"(*StR) */
  char buf[256];
  unsigned i;
  for (i = 0; i < 250; ++i) buf[i] = i + 1;
  buf[i] = '\0';
  ASSERT_EQ(250, strlen(buf));
}

TEST(strnlen_s, null_ReturnsZero) {
  // Wrongly standardized
  //ASSERT_EQ(0, strnlen_s(NULL, 3));
}

TEST(strnlen, nulNotFound_ReturnsSize) {
  int sizes[] = {1, 2, 7, 8, 15, 16, 31, 32, 33};
  for (unsigned i = 0; i < ARRAYLEN(sizes); ++i) {
    char *buf = malloc(sizes[i]);
    memset(buf, ' ', sizes[i]);
    ASSERT_EQ(sizes[i], strnlen(buf, sizes[i]), "%d", sizes[i]);
    free(buf);
  }
}

TEST(strnlen_s, nulNotFound) {
  char buf[3] = {1, 2, 3};
  // Wrongly standardized
  //ASSERT_EQ(3, strnlen_s(buf, 3));
}

TEST(strlen, fuzz) {
  char *b;
  size_t n, n1, n2;
  for (n = 2; n < 1026; ++n) {
    b = rngset(calloc(1, n), n - 1, _rand64, -1);
    n1 = strlen(b);
    n2 = strlen_pure(b);
    ASSERT_EQ(n1, n2, "%#.*s", n, b);
    n1 = strlen(b + 1);
    n2 = strlen_pure(b + 1);
    ASSERT_EQ(n1, n2);
    free(b);
  }
}

int main()
{
  strlen_usageExample_c11();
  strlen_usageExample_c99();
  strlen_whatMemoryLooksLike();
  strlen_test_const();
  strlen_test_nonconst();
  HEDLEY_CONCAT(strnlen, _testconst)();
  strlen_testnonconst();
  strnlen_s_null_ReturnsZero();
  HEDLEY_CONCAT(strnlen, _nulNotFound_ReturnsSize)();
  strnlen_s_nulNotFound();
  strlen_fuzz();
}
