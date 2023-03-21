//===-- Unittests for strcmp ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "test-deps/llvm-project.h"
#include <string.h>

TEST(LlvmLibcStrCmpTest, EmptyStringsShouldReturnZero) {
  const char *s1 = "";
  const char *s2 = "";
  int result = __llvm_libc strcmp(s1, s2);
  ASSERT_EQ(result, 0);

  // Verify operands reversed.
  result = __llvm_libc strcmp(s2, s1);
  ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrCmpTest, EmptyStringShouldNotEqualNonEmptyString) {
  const char *empty = "";
  const char *s2 = "abc";
  int result = __llvm_libc strcmp(empty, s2);
  // The original test is too strict according to the standard
  // This should be '\0' - 'a' = -97
  assert(result < 0); // ASSERT_EQ(result, -97);

  // Similar case if empty string is second argument.
  const char *s3 = "123";
  result = __llvm_libc strcmp(s3, empty);
  // The original test is too strict according to the standard
  // This should be '1' - '\0' = 49
  assert(result > 0); // ASSERT_EQ(result, 49);
}

TEST(LlvmLibcStrCmpTest, EqualStringsShouldReturnZero) {
  const char *s1 = "abc";
  const char *s2 = "abc";
  int result = __llvm_libc strcmp(s1, s2);
  ASSERT_EQ(result, 0);

  // Verify operands reversed.
  result = __llvm_libc strcmp(s2, s1);
  ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrCmpTest, ShouldReturnResultOfFirstDifference) {
  const char *s1 = "___B42__";
  const char *s2 = "___C55__";
  int result = __llvm_libc strcmp(s1, s2);
  // The original test is too strict according to the standard
  // This should return 'B' - 'C' = -1.
  assert(result < 0); // ASSERT_EQ(result, -1);

  // Verify operands reversed.
  result = __llvm_libc strcmp(s2, s1);
  // The original test is too strict according to the standard
  // This should return 'C' - 'B' = 1.
  assert(result > 0); // ASSERT_EQ(result, 1);
}

TEST(LlvmLibcStrCmpTest, CapitalizedLetterShouldNotBeEqual) {
  const char *s1 = "abcd";
  const char *s2 = "abCd";
  int result = __llvm_libc strcmp(s1, s2);
  // The original test is too strict according to the standard
  // 'c' - 'C' = 32.
  assert(result > 0); //ASSERT_EQ(result, 32);

  // Verify operands reversed.
  result = __llvm_libc strcmp(s2, s1);
  // The original test is too strict according to the standard
  // 'C' - 'c' = -32.
  assert(result < 0); //ASSERT_EQ(result, -32);
}

TEST(LlvmLibcStrCmpTest, UnequalLengthStringsShouldNotReturnZero) {
  const char *s1 = "abc";
  const char *s2 = "abcd";
  int result = __llvm_libc strcmp(s1, s2);
  // The original test is too strict according to the standard
  // '\0' - 'd' = -100.
  assert(result < 0); // ASSERT_EQ(result, -100);

  // Verify operands reversed.
  result = __llvm_libc strcmp(s2, s1);
  // The original test is too strict according to the standard
  // 'd' - '\0' = 100.
  assert(result > 0); // ASSERT_EQ(result, 100);
}

TEST(LlvmLibcStrCmpTest, StringArgumentSwapChangesSign) {
  const char *a = "a";
  const char *b = "b";
  int result = __llvm_libc strcmp(b, a);
  // The original test is too strict according to the standard
  // 'b' - 'a' = 1.
  assert(result > 0); //ASSERT_EQ(result, 1);

  result = __llvm_libc strcmp(a, b);
  // The original test is too strict according to the standard
  // 'a' - 'b' = -1.
  assert(result < 0); //ASSERT_EQ(result, -1);
}

TEST(LlvmLibcStrCmpTest, Case) {
  const char *s1 = "aB";
  const char *s2 = "ab";
  int result = __llvm_libc strcmp(s1, s2);
  ASSERT_LT(result, 0);

  // Verify operands reversed.
  result = __llvm_libc strcmp(s2, s1);
  ASSERT_GT(result, 0);
}

int main()
{
    LlvmLibcStrCmpTestEmptyStringsShouldReturnZero();
    LlvmLibcStrCmpTestEmptyStringShouldNotEqualNonEmptyString();
    LlvmLibcStrCmpTestEqualStringsShouldReturnZero();
    LlvmLibcStrCmpTestShouldReturnResultOfFirstDifference();
    LlvmLibcStrCmpTestCapitalizedLetterShouldNotBeEqual();
    LlvmLibcStrCmpTestUnequalLengthStringsShouldNotReturnZero();
    LlvmLibcStrCmpTestStringArgumentSwapChangesSign();
    LlvmLibcStrCmpTestCase();
}
