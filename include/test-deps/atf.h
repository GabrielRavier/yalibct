// Derived directly from work with this license:
/*
 * Automated Testing Framework (atf)
 *
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "test-lib/assert-with-message.h"
#include "test-lib/chdir-to-tmpdir.h"
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define ATF_TC(test_name)
#define ATF_TC_HEAD(test_name, idk) static void test_name ## _head ()
#define atf_tc_set_md_var(idk, str1, str2)
#define ATF_TC_BODY(test_name, idk) static void test_name ## _body ()
#define ATF_REQUIRE assert
#define ATF_CHECK assert
#define ATF_REQUIRE_STREQ(x, y) \
    ATF_REQUIRE_MSG(strcmp(x, y) == 0, "%s != %s (%s != %s)", #x, #y, x, y)
#define ATF_CHECK_STREQ ATF_REQUIRE_STREQ
#define ATF_TP_ADD_TCS(idk) static int yalibct_atf_tp_add_tcs_run_tests(); int main() { yalibct_chdir_to_tmpdir(); yalibct_atf_tp_add_tcs_run_tests(); yalibct_chdir_to_tmpdir_remove_tmpdir(); } static int yalibct_atf_tp_add_tcs_run_tests()
#define ATF_TP_ADD_TC(idk, test_name) test_name ## _head (); test_name ## _body();
#define ATF_TP_ADD_TC_NO_HEAD(idk, test_name) test_name ## _body();
#define atf_no_error() 0
#define ATF_CHECK_MSG(expr, ...) YALIBCT_ASSERT_WITH_MESSAGE(expr, __VA_ARGS__)
#define ATF_REQUIRE_MSG ATF_CHECK_MSG
#define ATF_TC_WITHOUT_HEAD(test_name)
#define atf_tc_fail(...) yalibct_internal_assert_with_message_print_failed_assertion("", "", 0, "", __VA_ARGS__)
#define __arraycount(__x)       (sizeof(__x) / sizeof(__x[0]))

static inline bool
atf_utils_compare_file(const char *name, const char *contents)
{
    const int fd = open(name, O_RDONLY);
    ATF_REQUIRE_MSG(fd != -1, "Cannot open %s", name);

    const char *pos = contents;
    ssize_t remaining = strlen(contents);

    char buffer[1024];
    ssize_t count;
    while ((count = read(fd, buffer, sizeof(buffer))) > 0 &&
           count <= remaining) {
        if (memcmp(pos, buffer, count) != 0) {
            close(fd);
            return false;
        }
        remaining -= count;
        pos += count;
    }
    close(fd);
    return count == 0 && remaining == 0;
}
