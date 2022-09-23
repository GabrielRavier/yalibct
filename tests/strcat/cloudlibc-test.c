// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#define _CLOUDLIBC_UNSAFE_STRING_FUNCTIONS

#include "test-deps/gtest.h"
#include <string.h>

TEST(strcat, example) {
  char buf[] = "\0AAAAAAAAA";
  ASSERT_EQ(buf, strcat(buf, ""));
  assert(memcmp(buf, "\0AAAAAAAAA", 10) == 0);
  ASSERT_EQ(buf, strcat(buf, "Hello"));
  assert(memcmp(buf, "Hello\0AAAA", 10) == 0);
  ASSERT_EQ(buf, strcat(buf, ""));
  assert(memcmp(buf, "Hello\0AAAA", 10) == 0);
  ASSERT_EQ(buf, strcat(buf, "!!!!"));
  assert(memcmp(buf, "Hello!!!!\0", 10) == 0);
}

int main(void)
{
    strcat_example();
}
