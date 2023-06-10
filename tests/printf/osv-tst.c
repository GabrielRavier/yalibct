/*
 * Copyright (C) 2014 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

// Tests for printf() and related functions.
//
// To compile on Linux, use: g++ -g -std=c++11 tst-printf.cc

#include "test-lib/portable-symbols/xmalloc.h"
#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>

#ifdef __clang__
YALIBCT_DIAGNOSTIC_IGNORE("-Wfortify-source")
#endif
YALIBCT_DIAGNOSTIC_IGNORE_WSTRINGOP_OVERFLOW

static int tests = 0, fails = 0;

static void report(bool ok, const char * msg)
{
    ++tests;
    fails += !ok;
    if (!ok)
        printf("FAIL: %s\n", msg);
    /*else
        printf("PASS: %s\n", msg);*/
}

// Strangely, it appears (?) that if the format is a constant, gcc
// doesn't call our snprintf() function at all and replaces it by
// some builtin. So we need to use this variable
const char *format = "%s";


char *print(const char* fmt, ...){
    int size = 512;
    char* buffer = xmalloc(size);
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if(size<=nsize){ //fail delete buffer and try again
        free(buffer);
        buffer = xmalloc(nsize+1); //+1 for /0
        assert(vsnprintf(buffer, size, fmt, vl) == nsize);
    }
    va_end(vl);
    return buffer;
}


int main(void)
{
    // Test that when snprintf is given a >32-bit length, including -1,
    // it works rather than not copying and returning EOVERFLOW.
    // Posix mandates this EOVERFLOW, and it makes sense, but glibc
    // doesn't do it, as this test demonstrate, and neither should we.
    const char *source = "hello";
    char dest[100] = "test";

    // Note: this test currently fails on recent versions of glibc - they seem to neither want to follow POSIX nor the C standard to the letter, which means the function doesn't either return EOVERFLOW (since (size_t)-1 > INT_MAX on most platforms) nor 5.
    // glibc instead claims that this invocation of snprintf results in undefined behavior, as (size_t)-1 is larger than the size of the destination - a proposition which might be accurate to the POSIX standard (which describes the destination buffer as being of the size specified by the second argument), but not to the current C standard as written, which only uses the second argument to describe how many characters may be written to the buffer, not to specify its size. As a result, an effort is currently underway to get a clarification from the C committee on the subject: they may decide that this is indeed undefined behavior after all, but as of now this code should be considered to conform to the standard, from our point of view, which means it won't be changed unless the C standard budges on the issue.
    int snprintf_retval;
#ifdef YALIBCT_LIBC_HAS_SNPRINTF
    snprintf_retval = snprintf(dest, -1, format, source);
    report(snprintf_retval == 5 || (snprintf_retval == -1 && errno == EOVERFLOW), "snprintf with n=-1");
    report(strcmp(source, dest) == 0 || strcmp(dest, "test") == 0, "strcmp result of snprintf with n=-1");
#else
    (void)dest;
#endif

    char dest2[100] = "test2";
    snprintf_retval = snprintf(dest2, 1ULL<<40, format, source);
    report(snprintf_retval == 5 || (snprintf_retval == -1 && errno == EOVERFLOW), "snprintf with n=2^40");
    report(strcmp(source, dest2) == 0 || strcmp(dest2, "test2") == 0, "strcmp result of snprintf with n=2^40");

    // Posix states that the "L" prefix is for long double, and "ll" is
    // for long long int, but in Linux glibc these are actually synonyms,
    // and either can be used
    // Test that the L format prefix, for "long double", works
    long double d = 123.456;
    long long int i = 123456;

    char *tmp_print;
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_L_LENGTH_MODIFIER_TESTS
    tmp_print = print("%Lf", d);
    report(!strcmp(tmp_print, "123.456000"), "Lf");
    free(tmp_print);
#endif
    // Removed these as non-standard and not widely supported
    //report(!strcmp(print("%llf", d), "123.456000"), "llf");
    //report(!strcmp(print("%Ld", i), "123456"), "Ld");
    tmp_print = print("%lld", i);
    report(!strcmp(tmp_print, "123456"), "lld");
    free(tmp_print);

    if (fails != 0)
        abort();
}
