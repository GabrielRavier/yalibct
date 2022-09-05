// Derived from work with this license:
// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2008-2014 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#pragma once

#include <assert.h>

#define BEGIN_TEST
#define END_TEST
#define EXPECT_TRUE assert
#define constexpr
#define EXPECT_EQ(a, b) assert((a) == (b))

#define UNITTEST_START_TESTCASE(x) int main() {
#define UNITTEST(str, name) name();
#define UNITTEST_END_TESTCASE(x, y, z) }
