/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include <stdint.h>

#define S(x) ((uintptr_t)(x))

/**
 * returns random bytes that don't have exclamation mark
 * since that would disable memory safety in the fuzzing
 */
static uint64_t Rando(void) {
  uint64_t x;
  do x = lemur64();
  while (((x ^ READ64LE("!!!!!!!!")) - 0x0101010101010101) &
         ~(x ^ READ64LE("!!!!!!!!")) & 0x8080808080808080);
  return x;
}

static const struct {
  const char *want;
  const char *fmt;
  uintptr_t arg1;
  uintptr_t arg2;
} V[] = {
    // UB and not widely supported
    /*    {"!!WONTFMT", (const char *)31337, 123},              //
        {"!!31337", "%s", 0x31337},                           //
        {"!!1", "%#s", 1},                                    //
        {"!!feff800000031337", "%s", 0xfeff800000031337ull},  //
        {"!!ffff800000031337", "%s", 0xffff800000031337ull},  //*/
    {"123", "%d", 123},                        //
    {"2147483647", "%d", INT_MAX},             //
    {"-2147483648", "%d", INT_MIN},            //
    {"9223372036854775807", "%ld", LONG_MAX},  //
    {"-9223372036854775808", "%ld", LONG_MIN}, //
// UB and not widely supported
    /*    {"9'223'372'036'854'775'807", "%'ld", LONG_MAX},      //
    {"-9'223'372'036'854'775'808", "%'ld", LONG_MIN},     //
    {"9,223,372,036,854,775,807", "%,ld", LONG_MAX},      //
    {"-9,223,372,036,854,775,808", "%,ld", LONG_MIN},     //
    {"9_223_372_036_854_775_807", "%_ld", LONG_MAX},      //
    {"-9_223_372_036_854_775_808", "%_ld", LONG_MIN},     //
    {"true", "%hhhd", 0xffff},                            //
    {"true", "%hhhd", 0xff00},                            //
    {"false", "%hhhd"},                                   //
    {"fa", "%hhh.2d"},                                    //*/
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
    {"  0x001337", "%#010.6x", 0x1337},                   //
#ifndef YALIBCT_DISABLE_PRINTF_MINUS_FLAG_TESTS
    {"0x001337  ", "%#-010.6x", 0x1337},                  //
    {"0x1337    ", "%#-010.2x", 0x1337},                  //
#endif
    {"    0x1337", "%#010.2x", 0x1337},                   //
#endif
    {"0000001337", "%010d", 1337},                        //
    {"+000001337", "%+010d", 1337},                       //
    {"    001337", "%010.6d", 1337},                      //
    {"   +001337", "%+010.6d", 1337},                     //
    {"    001337", "%010.6x", 0x1337},                    //
    {"      1337", "%010.2x", 0x1337},                    //
#ifndef YALIBCT_DISABLE_PRINTF_MINUS_FLAG_TESTS
    {"1337      ", "%-010d", 1337},                       //
    {"001337    ", "%-010.6d", 1337},                     //
    {"+1337     ", "%+-010d", 1337},                      //
    {"+001337   ", "%+-010.6d", 1337},                    //
    {"001337    ", "%-010.6x", 0x1337},                   //
    {"1337      ", "%-010.2x", 0x1337},                   //
#endif
#endif
    // UB and not widely supported
    //{"000001'337", "%'010d", 1337},                       //
    {"      1337", "%*d", 10, 1337},                      //
    {"1337      ", "%*d", -10, 1337},                     //
#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
    {"0", "%#x"},                                         //
    {"0", "%#o"},                                         //
#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_B_CONVERSION_SPECIFIER_TESTS
    {"0", "%#b"},                                         //
#endif
    {"0", "%#d"},                                         //
#endif
    // Implementation-defined and not widely supported
    //{"0", "%p"},                                          //
#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
    {"00000000", "%#.8x"},                                //
#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_B_CONVERSION_SPECIFIER_TESTS
    {"00000000", "%#.8b"},                                //
#endif
    {"00000000", "%#.8o"},                                //
#endif
    {"  123", "%5d", 123},                                //
    {" -123", "%5d", -123},                               //
    {"  123", "%*d", 5, 123},                             //
    {" -123", "%*d", 5, -123},                            //
    {"123  ", "%-5d", 123},                               //
    {"-123 ", "%-5d", -123},                              //
    {" +123", "%+5d", 123},                               //
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
    {"00123", "%05d", 123},                               //
    {"-0123", "%05d", -123},                              //
#endif
    {"    0", "%5d"},                                     //
    {"   +0", "%+5d"},                                    //
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
    {"00000", "%05d"},                                    //
#endif
    {"            deadbeef", "%20x", 0xdeadbeef},         //
    {"          0xdeadbeef", "%20p", 0xdeadbeef},         //
#ifndef YALIBCT_DISABLE_PRINTF_LOWERCASE_B_CONVERSION_SPECIFIER_TESTS
    {"101", "%b", 0b101},                                 //
#endif
    {"123", "%x", 0x123},                                 //
    {"deadbeef", "%x", 0xdeadbeef},                       //
    {"DEADBEEF", "%X", 0xdeadbeef},                       //
#ifndef YALIBCT_DISABLE_PRINTF_H_LENGTH_MODIFIER_TESTS
    {"0", "%hd", INT_MIN},                                //
#endif
    {"123", "%o", 0123},                                  //
    {"+0", "%+d"},                                        //
    {"+123", "%+d", 123},                                 //
    {"-123", "%+d", -123},                                //
    {" 0", "% d"},                                        //
    {" 123", "% d", 123},                                 //
    {"-123", "% d", -123},                                //
    {"x", "%c", 'x'},                                     //
    // UB and not widely supported
    //{"☺", "%hc", u'☺'},                                   //
    // Implementation-defined and not widely supported
    /*{"☺", "%lc", L'☺'},                                   //
      {"☺", "%C", L'☺'},                                    //*/
    {"0x31337", "%p", 0x31337},                           //
    {"0xffff800000031337", "%p", 0xffff800000031337ull},  //
    {"0xfeff800000031337", "%p", 0xfeff800000031337ull},  //
#ifndef YALIBCT_DISABLE_PRINTF_H_LENGTH_MODIFIER_TESTS
    {"65535", "%hu", 0xffffffffu},                        //
    {"0", "%hu", 0x80000000u},                            //
    {"123", "%hd", 123},                                  //
    {"32767", "%hd", SHRT_MAX},                           //
    {"-32768", "%hd", SHRT_MIN},                          //
#endif
#ifndef YALIBCT_DISABLE_PRINTF_HH_LENGTH_MODIFIER_TESTS
    {"-1", "%hhd", 0xffff},                               //
    {"-128", "%hhd", 0xff80},                             //
    {"255", "%hhu", 0xffffffffu},                         //
#endif
    // UB and not widely supported
    /*{"'x'", "%#c", 'x'},                                  //
    {"u'☺'", "%#hc", u'☺'},                               //
    {"L'☺'", "%#lc", L'☺'},                               //
    {"L'☺'", "%#C", L'☺'},                                //
    {"L'\\''", "%#C", L'\''},                             //*/
    {"hello world\n", "%s", S("hello world\n")},          //
    {"☺☻♥♦♣♠!\n", "%s", S("☺☻♥♦♣♠!\n")},                  //
    {"\1", "%s", S("\1")},                                 //
    {"\1", "%.*s", 1, S("\1")},                           //
    // UB and not widely supported
    /*{"\\001", "%'s", S("\1")},                            //
      {"\"\\001\"", "%#s", S("\1")},                        //*/
    {"", "%.*s", 0},                                      //
    // UB and not widely supported
    /*{"☺☻♥♦♣♠!", "%hhs", S("\1\2\3\4\5\6!")},              //
    {"☺☻", "%.*hhs", 2, S("\1\2\3\4\5\6!")},              //
    {"u\"☺☻\"", "%#.*hhs", 2, S("\1\2\3\4\5\6!")},        //
    {"u\" ☻\"", "%#.*hhs", 2, S("\0\2\3\4\5\6!")},        //
    {"", "% s", S("")},                                   //
    {" a", "% s", S("a")},                                //
    {"", "% .*s", 0, S("a")},                             //
    {"", "% s"},                                          //
    {"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷", "%hs", S(u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷")},                  //*/
    // Implementation-defined and not widely supported
    //{"☺☻♥♦♣♠!", "%ls", S(L"☺☻♥♦♣♠!")},                    //
    // UB and not widely supported
    //{"HELLO", "%^s", S("hello")},                         //
    {"   eeeeeee", "%10s", S("eeeeeee")},                 //
    {"hello", "%.*s", 5, S("hello world")},               //
    {"þell", "%.*s", 5, S("þello world")},                //
    // UB and not widely supported
    //{"þello", "%.*hs", 5, S(u"þello world")},             //
#ifndef YALIBCT_DISABLE_PRINTF_FIELD_LENGTH_ON_S_CONVERSION_SPECIFIER_TESTS
    {"  þeeeeee", "%10s", S("þeeeeee")},                 //
    {"☺☻♥♦♣♠!", "%10s", S("☺☻♥♦♣♠!")},                 //
#endif
    // UB and not widely supported
    /*{"☺☻♥♦♣♠    ", "%10hs", S(u"☺☻♥♦♣♠")},                //
      {"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷  ", "%10hs", S(u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷")},              //*/
    // Implementation-defined and not widely supported
    //{"☺☻♥♦♣♠!   ", "%10ls", S(L"☺☻♥♦♣♠!")},               //
    // UB and not widely supported
    /*{"\"xx\"", "%#s", S("xx")},                           //
      {"u\"☺☺\"", "%#hs", S(u"☺☺")},                        //
    {"L\"☺☺\"", "%#ls", S(L"☺☺")},                        //
    {"\"\\\\\\\"\\177\"", "%#s", S("\\\"\177")},          //*/
    {"%%", "%%%%"},                                       //
    // UB and not widely supported
    /*{"%", "%.%"},                                         //
      {"=", "%="},                                          //*/
};

TEST(snprintf, test) {
  char b[48], g[48];
  size_t i, j, n, rc;
  rngset(g, sizeof(g), 0, 0);
  for (i = 0; i < ARRAYLEN(V); ++i) {
    bzero(b, 48);
    n = strlen(V[i].want);
    rc = snprintf(b, 48, V[i].fmt, V[i].arg1, V[i].arg2);
    EXPECT_EQ(n, rc, "snprintf(\"%s\", %#lx, %#lx) → %zu ≠ %zu", V[i].fmt,
              V[i].arg1, V[i].arg2, rc, n);
    EXPECT_STREQ(V[i].want, b);
    memcpy(b, g, 48);
    for (j = 0; j < 40; ++j) {
      rc = snprintf(b, 0, V[i].fmt, V[i].arg1, V[i].arg2);
      ASSERT_EQ(n, rc, "snprintf(b, %zu, \"%s\", %#lx, %#lx) → %zu ≠ %zu", j,
                V[i].fmt, V[i].arg1, V[i].arg2, rc, n);
      ASSERT_EQ(READ64LE(g + j), READ64LE(b + j),
                "snprintf(b, %zu, \"%s\", %#lx, %#lx) → buffer overrun", j,
                V[i].fmt, V[i].arg1, V[i].arg2);
    }
  }
}

TEST(snprintf, testy) {
  char b[32];
  // UB and not widely supported
  /*EXPECT_EQ(3, snprintf(b, 32, "%#s", 1));
    EXPECT_STREQ("!!1", b);*/
}

TEST(snprintf, testNonTextFmt_wontFormat) {
  // UB and not widely supported
  /*
  char b[32];
  char variable_format_string[16] = "%s";
  EXPECT_EQ(9, snprintf(b, 32, variable_format_string, NULL));
  EXPECT_STREQ("!!WONTFMT", b);*/
}

TEST(snprintf, testUnterminatedOverrun_truncatesAtPageBoundary) {
  char *m;
  char b[32];
  m = memset(_mapanon(FRAMESIZE * 2), 1, FRAMESIZE);
  EXPECT_SYS(0, 0, munmap(m + FRAMESIZE, FRAMESIZE));
  // UB and not widely supported
  /*EXPECT_EQ(12, snprintf(b, 32, "%'s", m + FRAMESIZE - 3));
    EXPECT_STREQ("\\001\\001\\001", b);*/
  EXPECT_SYS(0, 0, munmap(m, FRAMESIZE));
}

TEST(snprintf, testEmptyBuffer_determinesTrueLength) {
  EXPECT_EQ(5, snprintf(0, 0, "hello"));
}

TEST(snprintf, testFormatOnly_copiesString) {
  char b[6];
  EXPECT_EQ(5, snprintf(b, 6, "hello"));
  EXPECT_STREQ("hello", b);
}

TEST(snprintf, testOneChar_justNulTerminates) {
  char b[2] = {1, 2};
  EXPECT_EQ(3, snprintf(b, 1, "%d", 123));
  EXPECT_EQ(0, b[0]);
  EXPECT_EQ(2, b[1]);
}

TEST(snprintf, testTruncate_addsDotsAndReturnsTrueLength) {
  char b[15];
  EXPECT_EQ(10, snprintf(b, 15, "%p", (void *)0xdeadbeef));
  EXPECT_STREQ("0xdeadbeef", b);
  EXPECT_EQ(10, snprintf(b, 10, "%p", (void *)0xdeadbeef));
  EXPECT_STREQ("0xdeadbee", b);
}

TEST(snprintf, testTruncate_preservesNewlineFromEndOfFormatString) {
  char b[14];
  EXPECT_EQ(11, snprintf(b, 10, "%p\n", (void *)0xdeadbeef));
  EXPECT_STREQ("0xdeadbee", b);
}

TEST(snprintf, testTruncate_doesntBreakApartCharacters) {
  char b[5];
  ASSERT_EQ(6, snprintf(b, 5, "☻☻"));
  ASSERT_STREQ("\xE2\x98\xBB\xE2", b);
}

TEST(snprintf, negativeOverflowIdiom_isSafe) {
  int i, n;
  char golden[11];
  struct {
    char underflow[11];
    char buf[11];
    char overflow[11];
  } u;
  memset(golden, -1, 11);
  memset(u.underflow, -1, 11);
  memset(u.overflow, -1, 11);
  i = 0;
  n = 11;
  i += snprintf(u.buf + i, n - i, "hello");
  ASSERT_STREQ("hello", u.buf);
  i += snprintf(u.buf + i, n - i, " world");
  ASSERT_STREQ("hello worl", u.buf);
  i += snprintf(u.buf + i, n - i, " i love you");
  ASSERT_STREQ("hello worl", u.buf);
  ASSERT_EQ(i, 5 + 6 + 11);
  ASSERT_EQ(0, memcmp(golden, u.underflow, 11));
  ASSERT_EQ(0, memcmp(golden, u.overflow, 11));
}

TEST(snprintf, truncation) {
  char buf[16] = {0};
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(0, 0, "%s", "xxxxx"), 5);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(buf, 1, "%s", "xxxxx"), 5);
  EXPECT_STREQ("", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(buf, 2, "%s", "xxxxx"), 5);
  EXPECT_STREQ("x", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(buf, 3, "%s", "xxxxx"), 5);
  EXPECT_STREQ("xx", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(buf, 4, "%s", "xxxxx"), 5);
  EXPECT_STREQ("xxx", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(buf, 5, "%s", "xxxxx"), 5);
  EXPECT_STREQ("xxxx", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(buf, 6, "%s", "xxxxxxxxxxx"), 11);
  EXPECT_STREQ("xxxxx", buf);
  rngset(buf, sizeof(buf) - 1, lemur64, -1);
  EXPECT_EQ(snprintf(buf, 7, "%s", "xxxxxxxxx"), 9);
  EXPECT_STREQ("xxxxxx", buf);
}

int main()
{
  snprintf_test();
  snprintf_testy();
  snprintf_testNonTextFmt_wontFormat();
  snprintf_testUnterminatedOverrun_truncatesAtPageBoundary();
  snprintf_testEmptyBuffer_determinesTrueLength();
  snprintf_testFormatOnly_copiesString();
  snprintf_testOneChar_justNulTerminates();
  snprintf_testTruncate_addsDotsAndReturnsTrueLength();
  snprintf_testTruncate_preservesNewlineFromEndOfFormatString();
  snprintf_testTruncate_doesntBreakApartCharacters();
  snprintf_negativeOverflowIdiom_isSafe();
  snprintf_truncation();
}
