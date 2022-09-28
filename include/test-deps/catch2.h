// Based on code with this license:
/*
 *  Catch v2.13.8
 *  ----------------------------------------------------------
 *  Copyright (c) 2022 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <assert.h>

#define TEST_CASE(TestName, ...) static void test_ ## TestName()
#define REQUIRE assert
