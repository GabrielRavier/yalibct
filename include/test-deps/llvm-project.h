#pragma once

#include <assert.h>

#define TEST(a, b) static void a ## b()
#define constexpr
#define __llvm_libc
#define EXPECT_EQ(a, b) assert((a) == (b))
#define static_cast
