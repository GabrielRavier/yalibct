// Derived from code with this license:
// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#pragma once

#include <assert.h>

#define TEST(test_suite_name, test_name) void test_suite_name ## _ ## test_name()

#define ASSERT_EQ(val1, val2) assert((val1) == (val2))
#define ASSERT_NE(val1, val2) assert((val1) != (val2))
#define ASSERT_STREQ(s1, s2) assert(strcmp((s1), (s2)) == 0)
#define ASSERT_GT(val1, val2) assert((val1) > (val2))
#define ASSERT_LT(val1, val2) assert((val1) < (val2))

#define EXPECT_GT ASSERT_GT
#define EXPECT_EQ ASSERT_EQ
#define EXPECT_NE ASSERT_NE

// Boolean assertions. Condition can be either a Boolean expression or an
// AssertionResult. For more information on how to use AssertionResult with
// these macros see comments on that class.
#define GTEST_EXPECT_TRUE(condition) GTEST_ASSERT_TRUE(condition)
#define GTEST_EXPECT_FALSE(condition) GTEST_ASSERT_FALSE(condition)
#define GTEST_ASSERT_TRUE(condition) assert((condition))
#define GTEST_ASSERT_FALSE(condition) assert(!(condition))

// Define these macros to 1 to omit the definition of the corresponding
// EXPECT or ASSERT, which clashes with some users' own code.

#if !(defined(GTEST_DONT_DEFINE_EXPECT_TRUE) && GTEST_DONT_DEFINE_EXPECT_TRUE)
#define EXPECT_TRUE(condition) GTEST_EXPECT_TRUE(condition)
#endif

#if !(defined(GTEST_DONT_DEFINE_EXPECT_FALSE) && GTEST_DONT_DEFINE_EXPECT_FALSE)
#define EXPECT_FALSE(condition) GTEST_EXPECT_FALSE(condition)
#endif

#if !(defined(GTEST_DONT_DEFINE_ASSERT_TRUE) && GTEST_DONT_DEFINE_ASSERT_TRUE)
#define ASSERT_TRUE(condition) GTEST_ASSERT_TRUE(condition)
#endif

#if !(defined(GTEST_DONT_DEFINE_ASSERT_FALSE) && GTEST_DONT_DEFINE_ASSERT_FALSE)
#define ASSERT_FALSE(condition) GTEST_ASSERT_FALSE(condition)
#endif


#define GTEST_TEST_F(test_fixture, test_name) void test_fixture ## _ ## test_name ## _internal_real_func(struct test_fixture *self); void test_fixture ## _ ## test_name() { struct test_fixture test_fixture_obj; test_fixture ## _SetUp(&test_fixture_obj); test_fixture ## _ ## test_name ## _internal_real_func(&test_fixture_obj); test_fixture ## _TearDown(&test_fixture_obj); } void test_fixture ## _ ## test_name ## _internal_real_func(struct test_fixture *self)
#if !(defined(GTEST_DONT_DEFINE_TEST_F) && GTEST_DONT_DEFINE_TEST_F)
#define TEST_F(test_fixture, test_name) GTEST_TEST_F(test_fixture, test_name)
#endif

#define GTEST_SKIP() return
