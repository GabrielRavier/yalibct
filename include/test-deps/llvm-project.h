// Based on code with this license:
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <assert.h>

#define TEST(a, b) static void a ## b()
#define constexpr
#define __llvm_libc
#define EXPECT_EQ(LHS, RHS) assert((LHS) == (RHS))
#define EXPECT_STREQ(LHS, RHS) assert(strcmp((LHS), (RHS)) == 0)
#define EXPECT_NE(LHS, RHS) assert((LHS) != (RHS))
#define static_cast
#define ASSERT_EQ EXPECT_EQ
#define ASSERT_STREQ EXPECT_STREQ
#define ASSERT_LT EXPECT_LT
#define ASSERT_GT EXPECT_GT
#define EXPECT_LT(LHS, RHS) assert((LHS) < (RHS))
#define EXPECT_GT(LHS, RHS) assert((LHS) > (RHS))
