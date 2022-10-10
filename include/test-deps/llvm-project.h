// Based on code with this license:
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <assert.h>

#define TEST(a, b) static void a ## b()
#define constexpr
#define __llvm_libc
#define EXPECT_EQ(a, b) assert((a) == (b))
#define EXPECT_NE(a, b) assert((a) != (b))
#define static_cast
#define ASSERT_EQ(LHS, RHS) EXPECT_EQ((LHS), (RHS))
#define ASSERT_STREQ(LHS, RHS) assert(strcmp((LHS), (RHS)) == 0)
