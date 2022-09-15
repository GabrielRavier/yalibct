// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include "test-deps/gtest.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

TEST(printf_scanf, examples) {
  char buf[11];
  ASSERT_EQ(10, snprintf(buf, sizeof(buf), "%" PRIdLEAST16 " %" PRIdPTR,
                         (int_least16_t)12345, (intptr_t)6789));
  ASSERT_STREQ("12345 6789", buf);

  uint_fast16_t v;
  ASSERT_EQ(1, sscanf(buf, "%" SCNdFAST16, &v));
  ASSERT_EQ(12345, v);
}

int main()
{
    printf_scanf_examples();
}
