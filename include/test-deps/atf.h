// Derived directly from work with this license:
/* Copyright (c) 2008 The NetBSD Foundation, Inc.
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
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  */

#pragma once

#include "test-lib/chdir-to-tmpdir.h"
#include "test-lib/hedley.h"
#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/printf.h"
#include "test-lib/portable-symbols/getopt.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdbool.h>

struct atf_tc;

typedef void (*atf_tc_head_t)(struct atf_tc *);
typedef void (*atf_tc_body_t)(const struct atf_tc *);
typedef void (*atf_tc_cleanup_t)(const struct atf_tc *);

/* ---------------------------------------------------------------------
 * The "atf_tc_pack" type.
 * --------------------------------------------------------------------- */

/* For static initialization only. */
struct atf_tc_pack {
    const char *m_ident;

    const char *const *m_config;

    atf_tc_head_t m_head;
    atf_tc_body_t m_body;
    atf_tc_cleanup_t m_cleanup;
};
typedef const struct atf_tc_pack atf_tc_pack_t;

/* ---------------------------------------------------------------------
 * The "atf_tc" type.
 * --------------------------------------------------------------------- */

struct atf_tc_impl;
struct atf_tc {
    struct atf_tc_impl *pimpl;
};
typedef struct atf_tc atf_tc_t;

#define ATF_TC_WITHOUT_HEAD(tc) \
    static void atfu_ ## tc ## _body(const atf_tc_t *); \
    static atf_tc_t atfu_ ## tc ## _tc; \
    static atf_tc_pack_t atfu_ ## tc ## _tc_pack = { \
        .m_ident = #tc, \
        .m_head = NULL, \
        .m_body = atfu_ ## tc ## _body, \
        .m_cleanup = NULL, \
    }

#define ATF_TC_WITH_CLEANUP(tc) \
    static void atfu_ ## tc ## _head(atf_tc_t *); \
    static void atfu_ ## tc ## _body(const atf_tc_t *); \
    static void atfu_ ## tc ## _cleanup(const atf_tc_t *); \
    static atf_tc_t atfu_ ## tc ## _tc; \
    static atf_tc_pack_t atfu_ ## tc ## _tc_pack = { \
        .m_ident = #tc, \
        .m_head = atfu_ ## tc ## _head, \
        .m_body = atfu_ ## tc ## _body, \
        .m_cleanup = atfu_ ## tc ## _cleanup, \
    }

#define ATF_DEFS_ATTRIBUTE_UNUSED YALIBCT_ATTRIBUTE_UNUSED

#define ATF_TC_HEAD(tc, tcptr) \
    static \
    void \
    atfu_ ## tc ## _head(atf_tc_t *tcptr ATF_DEFS_ATTRIBUTE_UNUSED)

#define ATF_TC_HEAD_NAME(tc) \
    (atfu_ ## tc ## _head)

#define ATF_TC_BODY(tc, tcptr) \
    static \
    void \
    atfu_ ## tc ## _body(const atf_tc_t *tcptr ATF_DEFS_ATTRIBUTE_UNUSED)

#define ATF_TC_BODY_NAME(tc) \
    (atfu_ ## tc ## _body)

#define ATF_TC_CLEANUP(tc, tcptr) \
    static \
    void \
    atfu_ ## tc ## _cleanup(const atf_tc_t *tcptr ATF_DEFS_ATTRIBUTE_UNUSED)

#define ATF_TC_CLEANUP_NAME(tc) \
                                                                       (atfu_ ## tc ## _cleanup)

#define ATF_TP_ADD_TCS(tps) \
    static atf_error_t atfu_tp_add_tcs(atf_tp_t *); \
    int atf_tp_main(int, char **, atf_error_t (*)(atf_tp_t *)); \
    \
    int \
    main(int argc, char **argv) \
    { \
        return atf_tp_main(argc, argv, atfu_tp_add_tcs); \
    } \
    static \
    atf_error_t \
    atfu_tp_add_tcs(atf_tp_t *tps)

#define ATF_REQUIRE_MSG(expression, fmt, ...) \
    do { \
        if (!(expression)) \
            atf_tc_fail_requirement(__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } while(0)

#define ATF_CHECK_MSG(expression, fmt, ...) \
    do { \
        if (!(expression)) \
            atf_tc_fail_check(__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } while(0)

#define ATF_REQUIRE(expression) \
    do { \
        if (!(expression)) \
            atf_tc_fail_requirement(__FILE__, __LINE__, "%s", \
                                    #expression " not met"); \
    } while(0)

#define ATF_CHECK(expression) \
    do { \
        if (!(expression)) \
            atf_tc_fail_check(__FILE__, __LINE__, "%s", \
                              #expression " not met"); \
    } while(0)

#define ATF_REQUIRE_EQ(expected, actual) \
    ATF_REQUIRE_MSG((expected) == (actual), "%s != %s", #expected, #actual)

#define ATF_CHECK_EQ(expected, actual) \
    ATF_CHECK_MSG((expected) == (actual), "%s != %s", #expected, #actual)

#define ATF_REQUIRE_EQ_MSG(expected, actual, fmt, ...) \
    ATF_REQUIRE_MSG((expected) == (actual), "%s != %s: " fmt, \
                    #expected, #actual, ##__VA_ARGS__)

#define ATF_CHECK_EQ_MSG(expected, actual, fmt, ...) \
    ATF_CHECK_MSG((expected) == (actual), "%s != %s: " fmt, \
                  #expected, #actual, ##__VA_ARGS__)

#define ATF_REQUIRE_STREQ(expected, actual) \
    ATF_REQUIRE_MSG(strcmp(expected, actual) == 0, "%s != %s (%s != %s)", \
                    #expected, #actual, expected, actual)

#define ATF_CHECK_STREQ(expected, actual) \
    ATF_CHECK_MSG(strcmp(expected, actual) == 0, "%s != %s (%s != %s)", \
                  #expected, #actual, expected, actual)

#define INV(x) \
    do { \
        if (!(x)) \
            atf_sanity_inv(__FILE__, __LINE__, #x); \
    } while (0)
#define PRE(x) \
    do { \
        if (!(x)) \
            atf_sanity_pre(__FILE__, __LINE__, #x); \
    } while (0)
#define POST(x) \
    do { \
        if (!(x)) \
            atf_sanity_post(__FILE__, __LINE__, #x); \
    } while (0)

#define PACKAGE_STRING "ATF_PACKAGE_STRING"
#define PACKAGE_BUGREPORT "ATF_PACKAGE_BUGREPORT"

HEDLEY_NO_RETURN
static
void
fail(const char *fmt, ...)
{
    va_list ap;
    char buf[4096];

    va_start(ap, fmt);
    assert(vsnprintf(buf, sizeof(buf), fmt, ap) == strlen(buf));
    va_end(ap);
    warnx("%s", buf);
    warnx("%s", "");
    warnx("This is probably a bug in this application or one of the "
          "libraries it uses.  If you believe this problem is caused "
          "by, or is related to " PACKAGE_STRING ", please report it "
          "to " PACKAGE_BUGREPORT " and provide as many details as "
          "possible describing how you got to this condition.");

    abort();
}

HEDLEY_NO_RETURN
void
atf_sanity_inv(const char *file, int line, const char *cond)
{
    fail("Invariant check failed at %s:%d: %s", file, line, cond);
}

HEDLEY_NO_RETURN
void
atf_sanity_pre(const char *file, int line, const char *cond)
{
    fail("Precondition check failed at %s:%d: %s", file, line, cond);
}

HEDLEY_NO_RETURN
void
atf_sanity_post(const char *file, int line, const char *cond)
{
    fail("Postcondition check failed at %s:%d: %s", file, line, cond);
}

enum expect_type {
    EXPECT_PASS,
    EXPECT_FAIL,
    EXPECT_EXIT,
    EXPECT_SIGNAL,
    EXPECT_DEATH,
    EXPECT_TIMEOUT,
};

/* ---------------------------------------------------------------------
 * The "atf_dynstr" type.
 * --------------------------------------------------------------------- */

struct atf_dynstr {
    char *m_data;
    size_t m_datasize;
    size_t m_length;
};
typedef struct atf_dynstr atf_dynstr_t;

struct context {
    const atf_tc_t *tc;
    const char *resfile;
    size_t fail_count;

    enum expect_type expect;
    atf_dynstr_t expect_reason;
    size_t expect_previous_fail_count;
    size_t expect_fail_count;
    int expect_exitcode;
    int expect_signo;
};

static struct context Current;

/* ---------------------------------------------------------------------
 * The "atf_error" type.
 * --------------------------------------------------------------------- */

struct atf_error;
typedef struct atf_error *atf_error_t;

/*
 * The "no_memory" error.
 */

/* ---------------------------------------------------------------------
 * The "atf_error" type.
 * --------------------------------------------------------------------- */

struct atf_error {
    bool m_free;
    const char *m_type;
    void *m_data;

    void (*m_format)(struct atf_error *, char *, size_t);
};

static struct atf_error no_memory_error;

atf_error_t
atf_no_error(void)
{
    return NULL;
}

bool
atf_is_error(const atf_error_t err) // NOLINT(misc-misplaced-const)
{
    return err != NULL;
}

bool
atf_error_is(const atf_error_t err, const char *type) // NOLINT(misc-misplaced-const)
{
    PRE(err != NULL);

    return strcmp(err->m_type, type) == 0;
}

const void *
atf_error_data(const atf_error_t err) // NOLINT(misc-misplaced-const)
{
    PRE(err != NULL);

    return err->m_data;
}

void
atf_error_format(const atf_error_t err, char *buf, size_t buflen) // NOLINT(misc-misplaced-const)
{
    PRE(err != NULL);
    err->m_format(err, buf, buflen);
}

static
void
no_memory_format(const atf_error_t err, char *buf, size_t buflen) // NOLINT(misc-misplaced-const)
{
    PRE(atf_error_is(err, "no_memory"));

    assert(snprintf(buf, buflen, "Not enough memory") == 17);
}

/* Theoretically, there can only be a single error intance at any given
 * point in time, because errors are raised at one point and must be
 * handled immediately.  If another error has to be raised during the
 * handling process, something else has to be done with the previous
 * error.
 *
 * This is per-thread information and will break threaded tests, but we
 * currently do not have any threading support; therefore, this is fine. */
static bool error_on_flight = false;

static
void
error_format(const atf_error_t err, char *buf, size_t buflen) // NOLINT(misc-misplaced-const)
{
    PRE(err != NULL);
    assert(snprintf(buf, buflen, "Error '%s'", err->m_type) == 8 + strlen(err->m_type));
}

static
bool
error_init(atf_error_t err, const char *type, const void *data, size_t datalen,
           void (*format)(const atf_error_t, char *, size_t)) // NOLINT(misc-misplaced-const)
{
    bool ok;

    PRE(data != NULL || datalen == 0);
    PRE(datalen != 0 || data == NULL);

    err->m_free = false;
    err->m_type = type;
    err->m_format = (format == NULL) ? error_format : format;

    ok = true;
    if (data == NULL) {
        err->m_data = NULL;
    } else {
        err->m_data = malloc(datalen);
        if (err->m_data == NULL) {
            ok = false;
        } else
            memcpy(err->m_data, data, datalen);
    }

    return ok;
}

atf_error_t
atf_no_memory_error(void)
{
    PRE(!error_on_flight);

    error_init(&no_memory_error, "no_memory", NULL, 0,
               no_memory_format);

    error_on_flight = true;
    return &no_memory_error;
}

/*
 * The "libc" error.
 */

struct atf_libc_error_data {
    int m_errno;
    char m_what[4096];
};
typedef struct atf_libc_error_data atf_libc_error_data_t;

static
void
libc_format(const atf_error_t err, char *buf, size_t buflen) // NOLINT(misc-misplaced-const)
{
    const atf_libc_error_data_t *data;

    PRE(atf_error_is(err, "libc"));

    data = atf_error_data(err);
    assert(snprintf(buf, buflen, "%s: %s", data->m_what, strerror(data->m_errno)) == strlen(data->m_what) + 2 + strlen(strerror(data->m_errno)));
}

atf_error_t
atf_error_new(const char *type, void *data, size_t datalen,
              void (*format)(const atf_error_t, char *, size_t)) // NOLINT(misc-misplaced-const)
{
    atf_error_t err;

    PRE(!error_on_flight);
    PRE(data != NULL || datalen == 0);
    PRE(datalen != 0 || data == NULL);

    err = malloc(sizeof(*err));
    if (err == NULL)
        err = atf_no_memory_error();
    else {
        if (!error_init(err, type, data, datalen, format)) {
            free(err);
            err = atf_no_memory_error();
        } else {
            err->m_free = true;
            error_on_flight = true;
        }
    }

    INV(err != NULL);
    POST(error_on_flight);
    return err;
}

atf_error_t
atf_libc_error(int syserrno, const char *fmt, ...)
{
    atf_error_t err;
    atf_libc_error_data_t data;
    va_list ap;

    data.m_errno = syserrno;
    va_start(ap, fmt);
    assert(vsnprintf(data.m_what, sizeof(data.m_what), fmt, ap) == strlen(data.m_what));
    va_end(ap);

    err = atf_error_new("libc", &data, sizeof(data), libc_format);

    return err;
}

atf_error_t
atf_dynstr_init_ap(atf_dynstr_t *ad, const char *fmt, va_list ap)
{
    atf_error_t err;

    ad->m_datasize = strlen(fmt) + 1;
    ad->m_length = 0;

    do {
        va_list ap2;
        int ret;

        ad->m_datasize *= 2;
        ad->m_data = (char *)malloc(ad->m_datasize);
        if (ad->m_data == NULL) {
            err = atf_no_memory_error();
            goto out;
        }

        va_copy(ap2, ap);
        ret = vsnprintf(ad->m_data, ad->m_datasize, fmt, ap2);
        va_end(ap2);
        if (ret < 0) {
            free(ad->m_data);
	    ad->m_data = NULL;
            err = atf_libc_error(errno, "Cannot format string");
            goto out;
        }

        INV(ret >= 0);
        if ((size_t)ret >= ad->m_datasize) {
            free(ad->m_data);
            ad->m_data = NULL;
        }
        ad->m_length = ret;
    } while (ad->m_length >= ad->m_datasize);

    err = atf_no_error();
out:
    POST(atf_is_error(err) || ad->m_data != NULL);
    return err;
}

atf_error_t
atf_dynstr_init_fmt(atf_dynstr_t *ad, const char *fmt, ...)
{
    va_list ap;
    atf_error_t err;

    va_start(ap, fmt);
    err = atf_dynstr_init_ap(ad, fmt, ap);
    va_end(ap);

    return err;
}

atf_error_t
atf_dynstr_init(atf_dynstr_t *ad)
{
    atf_error_t err;

    ad->m_data = (char *)malloc(sizeof(char));
    if (ad->m_data == NULL) {
        err = atf_no_memory_error();
        goto out;
    }

    ad->m_data[0] = '\0';
    ad->m_datasize = 1;
    ad->m_length = 0;
    err = atf_no_error();

out:
    return err;
}

size_t
atf_dynstr_length(const atf_dynstr_t *ad)
{
    return ad->m_length;
}

const size_t atf_dynstr_npos = SIZE_MAX;

size_t
atf_dynstr_rfind_ch(const atf_dynstr_t *ad, char ch)
{
    size_t pos;

    for (pos = ad->m_length; pos > 0 && ad->m_data[pos - 1] != ch; pos--)
        ;

    return pos == 0 ? atf_dynstr_npos : pos - 1;
}

/*
 * Modifiers.
 */

void
atf_dynstr_fini(atf_dynstr_t *ad)
{
    INV(ad->m_data != NULL);
    free(ad->m_data);
}

char *
atf_dynstr_fini_disown(const atf_dynstr_t *ad)
{
    INV(ad->m_data != NULL);
    return ad->m_data;
}

atf_error_t
atf_text_format_ap(char **dest, const char *fmt, va_list ap)
{
    atf_error_t err;
    atf_dynstr_t tmp;
    va_list ap2;

    va_copy(ap2, ap);
    err = atf_dynstr_init_ap(&tmp, fmt, ap2);
    va_end(ap2);
    if (!atf_is_error(err))
        *dest = atf_dynstr_fini_disown(&tmp);

    return err;
}

static
atf_error_t
resize(atf_dynstr_t *ad, size_t newsize)
{
    char *newdata;
    atf_error_t err;

    PRE(newsize > ad->m_datasize);

    newdata = (char *)malloc(newsize);
    if (newdata == NULL) {
        err = atf_no_memory_error();
    } else {
        strcpy(newdata, ad->m_data);
        free(ad->m_data);
        ad->m_data = newdata;
        ad->m_datasize = newsize;
        err = atf_no_error();
    }

    return err;
}

static
atf_error_t
prepend_or_append(atf_dynstr_t *ad, const char *fmt, va_list ap,
                  bool prepend)
{
    char *aux;
    atf_error_t err;
    size_t newlen;
    va_list ap2;

    va_copy(ap2, ap);
    err = atf_text_format_ap(&aux, fmt, ap2);
    va_end(ap2);
    if (atf_is_error(err))
        goto out;
    newlen = ad->m_length + strlen(aux);

    if (newlen + sizeof(char) > ad->m_datasize) {
        err = resize(ad, newlen + sizeof(char));
        if (atf_is_error(err))
            goto out_free;
    }

    if (prepend) {
        memmove(ad->m_data + strlen(aux), ad->m_data, ad->m_length + 1);
        memcpy(ad->m_data, aux, strlen(aux));
    } else
        strcpy(ad->m_data + ad->m_length, aux);
    ad->m_length = newlen;
    err = atf_no_error();

out_free:
    free(aux);
out:
    return err;
}

atf_error_t
atf_dynstr_append_ap(atf_dynstr_t *ad, const char *fmt, va_list ap)
{
    atf_error_t err;
    va_list ap2;

    va_copy(ap2, ap);
    err = prepend_or_append(ad, fmt, ap2, false);
    va_end(ap2);

    return err;
}

void
atf_error_free(atf_error_t err)
{
    bool freeit;

    PRE(error_on_flight);
    PRE(err != NULL);

    freeit = err->m_free;

    if (err->m_data != NULL)
        free(err->m_data);

    if (freeit) {
        PRE(err != &no_memory_error);
        free(err);
    }

    error_on_flight = false;
}

static void
check_fatal_error(atf_error_t err)
{
    if (atf_is_error(err)) {
        char buf[1024];
        atf_error_format(err, buf, sizeof(buf));
        assert(fprintf(stderr, "FATAL ERROR: %s\n", buf) == 14 + strlen(buf));
        atf_error_free(err);
        abort();
    }
}

/** Formats a failure/skip reason message.
 *
 * The formatted reason is stored in out_reason.  out_reason is initialized
 * in this function and is supposed to be released by the caller.  In general,
 * the reason will eventually be fed to create_resfile, which will release
 * it.
 *
 * Errors in this function are fatal.  Rationale being: reasons are used to
 * create results files; if we can't format the reason correctly, the result
 * of the test program will be bogus.  So it's better to just exit with a
 * fatal error.
 */
static void
format_reason_ap(atf_dynstr_t *out_reason,
                 const char *source_file, const size_t source_line,
                 const char *reason, va_list ap)
{
    atf_error_t err;

    if (source_file != NULL) {
        err = atf_dynstr_init_fmt(out_reason, "%s:%zd: ", source_file,
                                  source_line);
    } else {
        PRE(source_line == 0);
        err = atf_dynstr_init(out_reason);
    }

    if (!atf_is_error(err)) {
        va_list ap2;
        va_copy(ap2, ap);
        err = atf_dynstr_append_ap(out_reason, reason, ap2);
        va_end(ap2);
    }

    check_fatal_error(err);
}

atf_error_t
atf_dynstr_prepend_fmt(atf_dynstr_t *ad, const char *fmt, ...)
{
    va_list ap;
    atf_error_t err;

    va_start(ap, fmt);
    err = prepend_or_append(ad, fmt, ap, true);
    va_end(ap);

    return err;
}
const char *
atf_dynstr_cstring(const atf_dynstr_t *ad)
{
    return ad->m_data;
}

/** Writes to a results file.
 *
 * The results file is supposed to be already open.
 *
 * This function returns an error code instead of exiting in case of error
 * because the caller needs to clean up the reason object before terminating.
 */
static atf_error_t
write_resfile(const int fd, const char *result, const int arg,
              const atf_dynstr_t *reason)
{
    static char NL[] = "\n", CS[] = ": ";
    char buf[64];
    const char *r;
    struct iovec iov[5];
    ssize_t ret;
    int count = 0;

    INV(arg == -1 || reason != NULL);

#define UNCONST(a) ((void *)(uintptr_t)(const void *)(a))
    iov[count].iov_base = UNCONST(result);
    iov[count++].iov_len = strlen(result);

    if (reason != NULL) {
        if (arg != -1) {
            iov[count].iov_base = buf;
            iov[count++].iov_len = snprintf(buf, sizeof(buf), "(%d)", arg);
        }

        iov[count].iov_base = CS;
        iov[count++].iov_len = sizeof(CS) - 1;

        r = atf_dynstr_cstring(reason);
        iov[count].iov_base = UNCONST(r);
        iov[count++].iov_len = strlen(r);
    }
#undef UNCONST

    iov[count].iov_base = NL;
    iov[count++].iov_len = sizeof(NL) - 1;

    while ((ret = writev(fd, iov, count)) == -1 && errno == EINTR)
        ; /* Retry. */
    if (ret != -1)
        return atf_no_error();

    return atf_libc_error(
        errno, "Failed to write results file; result %s, reason %s", result,
        reason == NULL ? "null" : atf_dynstr_cstring(reason));
}

/** Creates a results file.
 *
 * The input reason is released in all cases.
 *
 * An error in this function is considered to be fatal, hence why it does
 * not return any error code.
 */
static void
create_resfile(const char *resfile, const char *result, const int arg,
               atf_dynstr_t *reason)
{
    atf_error_t err;

    if (strcmp("/dev/stdout", resfile) == 0) {
        err = write_resfile(STDOUT_FILENO, result, arg, reason);
    } else if (strcmp("/dev/stderr", resfile) == 0) {
        err = write_resfile(STDERR_FILENO, result, arg, reason);
    } else {
        const int fd = open(resfile, O_WRONLY | O_CREAT | O_TRUNC,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1) {
            err = atf_libc_error(errno, "Cannot create results file '%s'",
                                 resfile);
        } else {
            err = write_resfile(fd, result, arg, reason);
            close(fd);
        }
    }

    if (reason != NULL)
        atf_dynstr_fini(reason);

    check_fatal_error(err);
}

static void
expected_failure(struct context *ctx, atf_dynstr_t *reason)
{
    check_fatal_error(atf_dynstr_prepend_fmt(reason, "%s: ",
        atf_dynstr_cstring(&ctx->expect_reason)));
    create_resfile(ctx->resfile, "expected_failure", -1, reason);
    exit(EXIT_SUCCESS);
}

#define ATF_DEFS_ATTRIBUTE_NORETURN HEDLEY_NO_RETURN

ATF_DEFS_ATTRIBUTE_NORETURN static void error_in_expect(struct context *ctx, const char *fmt, ...);
static void validate_expect(struct context *ctx);
ATF_DEFS_ATTRIBUTE_NORETURN static void expected_failure(struct context *ctx, atf_dynstr_t *reason);
ATF_DEFS_ATTRIBUTE_NORETURN static void fail_requirement(struct context *ctx, atf_dynstr_t *reason);
static void fail_check(struct context *ctx, atf_dynstr_t *reason);
static void pass(struct context *ctx);
ATF_DEFS_ATTRIBUTE_NORETURN static void skip(struct context *, atf_dynstr_t *);

/** Fails a test case if validate_expect fails. */
static void
error_in_expect(struct context *ctx, const char *fmt, ...)
{
    atf_dynstr_t reason;
    va_list ap;

    va_start(ap, fmt);
    format_reason_ap(&reason, NULL, 0, fmt, ap);
    va_end(ap);

    ctx->expect = EXPECT_PASS;  /* Ensure fail_requirement really fails. */
    fail_requirement(ctx, &reason);
}

#define UNREACHABLE INV(0)

static void
fail_requirement(struct context *ctx, atf_dynstr_t *reason)
{
    if (ctx->expect == EXPECT_FAIL) {
        expected_failure(ctx, reason);
    } else if (ctx->expect == EXPECT_PASS) {
        create_resfile(ctx->resfile, "failed", -1, reason);
        exit(EXIT_FAILURE);
    } else {
        error_in_expect(ctx, "Test case raised a failure but was not "
            "expecting one; reason was %s", atf_dynstr_cstring(reason));
    }
    UNREACHABLE;
}

static void
_atf_tc_fail_requirement(struct context *ctx, const char *file,
                         const size_t line, const char *fmt, va_list ap)
{
    va_list ap2;
    atf_dynstr_t reason;

    va_copy(ap2, ap);
    format_reason_ap(&reason, file, line, fmt, ap2);
    va_end(ap2);

    fail_requirement(ctx, &reason);
    UNREACHABLE;
}

void
atf_tc_fail_requirement(const char *file, const size_t line,
                        const char *fmt, ...)
{
    va_list ap;

    PRE(Current.tc != NULL);

    va_start(ap, fmt);
    _atf_tc_fail_requirement(&Current, file, line, fmt, ap);
    va_end(ap);
}

static void
_atf_tc_fail_check(struct context *ctx, const char *file, const size_t line,
                   const char *fmt, va_list ap)
{
    va_list ap2;
    atf_dynstr_t reason;

    va_copy(ap2, ap);
    format_reason_ap(&reason, file, line, fmt, ap2);
    va_end(ap2);

    fail_check(ctx, &reason);
}

void
atf_tc_fail_check(const char *file, const size_t line, const char *fmt, ...)
{
    va_list ap;

    PRE(Current.tc != NULL);

    va_start(ap, fmt);
    _atf_tc_fail_check(&Current, file, line, fmt, ap);
    va_end(ap);
}

#define ATF_TC_NAME(tc) \
    (atfu_ ## tc ## _tc)

#define ATF_TC_PACK_NAME(tc) \
    (atfu_ ## tc ## _tc_pack)

#define ATF_TC_WITHOUT_HEAD(tc) \
    static void atfu_ ## tc ## _body(const atf_tc_t *); \
    static atf_tc_t atfu_ ## tc ## _tc; \
    static atf_tc_pack_t atfu_ ## tc ## _tc_pack = { \
        .m_ident = #tc, \
        .m_head = NULL, \
        .m_body = atfu_ ## tc ## _body, \
        .m_cleanup = NULL, \
    }

#define ATF_TC(tc) \
    static void atfu_ ## tc ## _head(atf_tc_t *); \
    static void atfu_ ## tc ## _body(const atf_tc_t *); \
    static atf_tc_t atfu_ ## tc ## _tc; \
    static atf_tc_pack_t atfu_ ## tc ## _tc_pack = { \
        .m_ident = #tc, \
        .m_head = atfu_ ## tc ## _head, \
        .m_body = atfu_ ## tc ## _body, \
        .m_cleanup = NULL, \
    }

#define ATF_TC_HEAD(tc, tcptr) \
    static \
    void \
    atfu_ ## tc ## _head(atf_tc_t *tcptr ATF_DEFS_ATTRIBUTE_UNUSED)

#define ATF_TC_HEAD_NAME(tc) \
    (atfu_ ## tc ## _head)

/* ---------------------------------------------------------------------
 * The "atf_list" type.
 * --------------------------------------------------------------------- */

struct atf_list {
    void *m_begin;
    void *m_end;

    size_t m_size;
};
typedef struct atf_list atf_list_t;

/* ---------------------------------------------------------------------
 * The "atf_map" type.
 * --------------------------------------------------------------------- */

/* A list-based map.  Typically very inefficient, but our maps are small
 * enough. */
struct atf_map {
    atf_list_t m_list;
};
typedef struct atf_map atf_map_t;

/* ---------------------------------------------------------------------
 * The "atf_list_iter" type.
 * --------------------------------------------------------------------- */

struct atf_list_iter {
    struct atf_list *m_list;
    void *m_entry;
};
typedef struct atf_list_iter atf_list_iter_t;

/* ---------------------------------------------------------------------
 * The "atf_map_iter" type.
 * --------------------------------------------------------------------- */

struct atf_map_iter {
    struct atf_map *m_map;
    void *m_entry;
    atf_list_iter_t m_listiter;
};
typedef struct atf_map_iter atf_map_iter_t;

/* Macros. */
#define atf_list_for_each(iter, list) \
    for (iter = atf_list_begin(list); \
         !atf_equal_list_iter_list_iter((iter), atf_list_end(list)); \
         iter = atf_list_iter_next(iter))
#define atf_list_for_each_c(iter, list) \
    for (iter = atf_list_begin_c(list); \
         !atf_equal_list_citer_list_citer((iter), atf_list_end_c(list)); \
         iter = atf_list_citer_next(iter))

struct list_entry {
    struct list_entry *m_prev;
    struct list_entry *m_next;
    void *m_object;
    bool m_managed;
};

static
atf_list_iter_t
entry_to_iter(atf_list_t *l, struct list_entry *le)
{
    atf_list_iter_t iter;
    iter.m_list = l;
    iter.m_entry = le;
    return iter;
}

/*
 * Getters.
 */

atf_list_iter_t
atf_list_begin(atf_list_t *l)
{
    struct list_entry *le = l->m_begin;
    return entry_to_iter(l, le->m_next);
}

/* ---------------------------------------------------------------------
 * The "atf_list_citer" type.
 * --------------------------------------------------------------------- */

struct atf_list_citer {
    const struct atf_list *m_list;
    const void *m_entry;
};
typedef struct atf_list_citer atf_list_citer_t;

static
atf_list_citer_t
entry_to_citer(const atf_list_t *l, const struct list_entry *le)
{
    atf_list_citer_t iter;
    iter.m_list = l;
    iter.m_entry = le;
    return iter;
}

atf_list_citer_t
atf_list_begin_c(const atf_list_t *l)
{
    const struct list_entry *le = l->m_begin;
    return entry_to_citer(l, le->m_next);
}

atf_list_iter_t
atf_list_end(atf_list_t *l)
{
    return entry_to_iter(l, l->m_end);
}

atf_list_citer_t
atf_list_end_c(const atf_list_t *l)
{
    return entry_to_citer(l, l->m_end);
}

void *
atf_list_iter_data(const atf_list_iter_t iter)
{
    const struct list_entry *le = iter.m_entry;
    PRE(le != NULL);
    return le->m_object;
}

atf_list_iter_t
atf_list_iter_next(const atf_list_iter_t iter)
{
    const struct list_entry *le = iter.m_entry;
    atf_list_iter_t newiter;

    PRE(le != NULL);

    newiter = iter;
    newiter.m_entry = le->m_next;

    return newiter;
}

bool
atf_equal_list_iter_list_iter(const atf_list_iter_t i1,
                              const atf_list_iter_t i2)
{
    return i1.m_list == i2.m_list && i1.m_entry == i2.m_entry;
}

struct map_entry {
    char *m_key;
    void *m_value;
    bool m_managed;
};

atf_map_iter_t
atf_map_end(atf_map_t *m)
{
    atf_map_iter_t iter;
    iter.m_map = m;
    iter.m_entry = NULL;
    iter.m_listiter = atf_list_end(&m->m_list);
    return iter;
}

atf_map_iter_t
atf_map_find(atf_map_t *m, const char *key)
{
    atf_list_iter_t iter;

    atf_list_for_each(iter, &m->m_list) {
        struct map_entry *me = atf_list_iter_data(iter);

        if (strcmp(me->m_key, key) == 0) {
            atf_map_iter_t i;
            i.m_map = m;
            i.m_entry = me;
            i.m_listiter = iter;
            return i;
        }
    }

    return atf_map_end(m);
}

const char *
atf_map_iter_key(const atf_map_iter_t iter)
{
    const struct map_entry *me = iter.m_entry;
    PRE(me != NULL);
    return me->m_key;
}

void *
atf_map_iter_data(const atf_map_iter_t iter)
{
    const struct map_entry *me = iter.m_entry;
    PRE(me != NULL);
    return me->m_value;
}

atf_map_iter_t
atf_map_iter_next(const atf_map_iter_t iter)
{
    atf_map_iter_t newiter;

    newiter = iter;
    newiter.m_listiter = atf_list_iter_next(iter.m_listiter);
    newiter.m_entry = ((struct map_entry *)
                       atf_list_iter_data(newiter.m_listiter));

    return newiter;
}

bool
atf_equal_map_iter_map_iter(const atf_map_iter_t i1,
                            const atf_map_iter_t i2)
{
    return i1.m_map == i2.m_map && i1.m_entry == i2.m_entry;
}

static
struct map_entry *
atf_detail_map_new_entry(const char *key, void *value, bool managed)
{
    struct map_entry *me;

    me = (struct map_entry *)malloc(sizeof(*me));
    if (me != NULL) {
        me->m_key = strdup(key);
        if (me->m_key == NULL) {
            free(me);
            me = NULL;
        } else {
            me->m_value = value;
            me->m_managed = managed;
        }
    }

    return me;
}

static
void
delete_entry(struct list_entry *le)
{
    if (le->m_managed)
        free(le->m_object);

    free(le);
}

static
struct list_entry *
atf_detail_list_new_entry(void *object, bool managed)
{
    struct list_entry *le;

    le = (struct list_entry *)malloc(sizeof(*le));
    if (le != NULL) {
        le->m_prev = le->m_next = NULL;
        le->m_object = object;
        le->m_managed = managed;
    } else if (managed)
        free(object);

    return le;
}

static
struct list_entry *
new_entry_and_link(void *object, bool managed, struct list_entry *prev,
                   struct list_entry *next)
{
    struct list_entry *le;

    le = atf_detail_list_new_entry(object, managed);
    if (le != NULL) {
        le->m_prev = prev;
        le->m_next = next;

        prev->m_next = le;
        next->m_prev = le;
    }

    return le;
}

/*
 * Modifiers.
 */

atf_error_t
atf_list_append(atf_list_t *l, void *data, bool managed)
{
    const struct list_entry *le;
    struct list_entry *next, *prev;
    atf_error_t err;

    next = (struct list_entry *)l->m_end;
    prev = next->m_prev;
    le = new_entry_and_link(data, managed, prev, next);
    if (le == NULL)
        err = atf_no_memory_error();
    else {
        l->m_size++;
        err = atf_no_error();
    }

    return err;
}

/*
 * Modifiers.
 */

atf_error_t
atf_map_insert(atf_map_t *m, const char *key, void *value, bool managed)
{
    struct map_entry *me;
    atf_error_t err;
    atf_map_iter_t iter;

    iter = atf_map_find(m, key);
    if (atf_equal_map_iter_map_iter(iter, atf_map_end(m))) {
        me = atf_detail_map_new_entry(key, value, managed);
        if (me == NULL) {
            if (managed)
                free(value);
            err = atf_no_memory_error();
        }
        else {
            err = atf_list_append(&m->m_list, me, false);
            if (atf_is_error(err)) {
                if (managed)
                    free(value);
                free(me);
            }
        }
    } else {
        me = iter.m_entry;
        if (me->m_managed)
            free(me->m_value);

        INV(strcmp(me->m_key, key) == 0);
        me->m_value = value;
        me->m_managed = managed;

        err = atf_no_error();
    }

    return err;
}

/* ---------------------------------------------------------------------
 * The "atf_tc" type.
 * --------------------------------------------------------------------- */

struct atf_tc_impl {
    const char *m_ident;

    atf_map_t m_vars;
    atf_map_t m_config;

    atf_tc_head_t m_head;
    atf_tc_body_t m_body;
    atf_tc_cleanup_t m_cleanup;
};

/*
 * Modifiers.
 */

atf_error_t
atf_tc_set_md_var(atf_tc_t *tc, const char *name, const char *fmt, ...)
{
    atf_error_t err;
    char *value = NULL;
    va_list ap;

    va_start(ap, fmt);
    err = atf_text_format_ap(&value, fmt, ap);
    va_end(ap);

    if (!atf_is_error(err))
        err = atf_map_insert(&tc->pimpl->m_vars, name, value, true);
    else
        free(value);

    return err;
}

static void
_atf_tc_fail(struct context *ctx, const char *fmt, va_list ap)
{
    va_list ap2;
    atf_dynstr_t reason;

    va_copy(ap2, ap);
    format_reason_ap(&reason, NULL, 0, fmt, ap2);
    va_end(ap2);

    fail_requirement(ctx, &reason);
    UNREACHABLE;
}

void
atf_tc_fail(const char *fmt, ...)
{
    va_list ap;

    PRE(Current.tc != NULL);

    va_start(ap, fmt);
    _atf_tc_fail(&Current, fmt, ap);
    va_end(ap);
}

/** Compares a file against the given golden contents.
 *
 * \param name Name of the file to be compared.
 * \param contents Expected contents of the file.
 *
 * \return True if the file matches the contents; false otherwise. */
bool
atf_utils_compare_file(const char *name, const char *contents)
{
    const int fd = open(name, O_RDONLY);
    ATF_REQUIRE_MSG(fd != -1, "Cannot open %s", name);

    const char *pos = contents;
    ssize_t remaining = (ssize_t)strlen(contents);

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

/* ---------------------------------------------------------------------
 * The "atf_tp" type.
 * --------------------------------------------------------------------- */

struct atf_tp_impl;
struct atf_tp {
    struct atf_tp_impl *pimpl;
};
typedef struct atf_tp atf_tp_t;

#define ATF_TP_ADD_TC(tp, tc) \
    do { \
        atf_error_t atfu_err; \
        char **atfu_config = atf_tp_get_config(tp); \
        if (atfu_config == NULL) \
            return atf_no_memory_error(); \
        atfu_err = atf_tc_init_pack(&atfu_ ## tc ## _tc, \
                                    &atfu_ ## tc ## _tc_pack, \
                                    (const char *const *)atfu_config); \
        atf_utils_free_charpp(atfu_config); \
        if (atf_is_error(atfu_err)) \
            return atfu_err; \
        atfu_err = atf_tp_add_tc(tp, &atfu_ ## tc ## _tc); \
        if (atf_is_error(atfu_err)) \
            return atfu_err; \
    } while (0)

#define ATF_REQUIRE_MSG(expression, fmt, ...) \
    do { \
        if (!(expression)) \
            atf_tc_fail_requirement(__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } while(0)

#define ATF_CHECK_MSG(expression, fmt, ...) \
    do { \
        if (!(expression)) \
            atf_tc_fail_check(__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } while(0)

#define ATF_REQUIRE(expression) \
    do { \
        if (!(expression)) \
            atf_tc_fail_requirement(__FILE__, __LINE__, "%s", \
                                    #expression " not met"); \
    } while(0)

#define ATF_CHECK(expression) \
    do { \
        if (!(expression)) \
            atf_tc_fail_check(__FILE__, __LINE__, "%s", \
                              #expression " not met"); \
    } while(0)

#define ATF_REQUIRE_EQ(expected, actual) \
    ATF_REQUIRE_MSG((expected) == (actual), "%s != %s", #expected, #actual)

#define ATF_CHECK_EQ(expected, actual) \
    ATF_CHECK_MSG((expected) == (actual), "%s != %s", #expected, #actual)

size_t
atf_list_size(const atf_list_t *l)
{
    return l->m_size;
}

size_t
atf_map_size(const atf_map_t *m)
{
    return atf_list_size(&m->m_list);
}

/* ---------------------------------------------------------------------
 * The "atf_map_citer" type.
 * --------------------------------------------------------------------- */

struct atf_map_citer {
    const struct atf_map *m_map;
    const void *m_entry;
    atf_list_citer_t m_listiter;
};
typedef struct atf_map_citer atf_map_citer_t;

/* Macros. */
#define atf_map_for_each(iter, map) \
    for (iter = atf_map_begin(map); \
         !atf_equal_map_iter_map_iter((iter), atf_map_end(map)); \
         iter = atf_map_iter_next(iter))
#define atf_map_for_each_c(iter, map) \
    for (iter = atf_map_begin_c(map); \
         !atf_equal_map_citer_map_citer((iter), atf_map_end_c(map)); \
         iter = atf_map_citer_next(iter))

/*
 * Getters.
 */

const void *
atf_list_citer_data(const atf_list_citer_t citer)
{
    const struct list_entry *le = citer.m_entry;
    PRE(le != NULL);
    return le->m_object;
}

atf_list_citer_t
atf_list_citer_next(const atf_list_citer_t citer)
{
    const struct list_entry *le = citer.m_entry;
    atf_list_citer_t newciter;

    PRE(le != NULL);

    newciter = citer;
    newciter.m_entry = le->m_next;

    return newciter;
}

bool
atf_equal_list_citer_list_citer(const atf_list_citer_t i1,
                                const atf_list_citer_t i2)
{
    return i1.m_list == i2.m_list && i1.m_entry == i2.m_entry;
}

atf_map_citer_t
atf_map_begin_c(const atf_map_t *m)
{
    atf_map_citer_t citer;
    citer.m_map = m;
    citer.m_listiter = atf_list_begin_c(&m->m_list);
    citer.m_entry = atf_list_citer_data(citer.m_listiter);
    return citer;
}

/*
 * Getters.
 */

const char *
atf_map_citer_key(const atf_map_citer_t citer)
{
    const struct map_entry *me = citer.m_entry;
    PRE(me != NULL);
    return me->m_key;
}

const void *
atf_map_citer_data(const atf_map_citer_t citer)
{
    const struct map_entry *me = citer.m_entry;
    PRE(me != NULL);
    return me->m_value;
}

atf_map_citer_t
atf_map_citer_next(const atf_map_citer_t citer)
{
    atf_map_citer_t newciter;

    newciter = citer;
    newciter.m_listiter = atf_list_citer_next(citer.m_listiter);
    newciter.m_entry = ((const struct map_entry *)
                        atf_list_citer_data(newciter.m_listiter));

    return newciter;
}

bool
atf_equal_map_citer_map_citer(const atf_map_citer_t i1,
                              const atf_map_citer_t i2)
{
    return i1.m_map == i2.m_map && i1.m_entry == i2.m_entry;
}

atf_map_citer_t
atf_map_end_c(const atf_map_t *m)
{
    atf_map_citer_t iter;
    iter.m_map = m;
    iter.m_entry = NULL;
    iter.m_listiter = atf_list_end_c(&m->m_list);
    return iter;
}

/** Frees an dynamically-allocated "argv" array.
 *
 * \param argv A dynamically-allocated array of dynamically-allocated
 *     strings. */
void
atf_utils_free_charpp(char **argv)
{
    char **ptr;

    for (ptr = argv; *ptr != NULL; ptr++)
        free(*ptr);

    free((void *)argv);
}

char **
atf_map_to_charpp(const atf_map_t *l)
{
    char **array;
    atf_map_citer_t iter;
    size_t i;

    array = (char **)malloc(sizeof(char *) * (atf_map_size(l) * 2 + 1));
    if (array == NULL)
        goto out;

    i = 0;
    atf_map_for_each_c(iter, l) {
        array[i] = strdup(atf_map_citer_key(iter));
        if (array[i] == NULL) {
            atf_utils_free_charpp(array);
            array = NULL;
            goto out;
        }

        array[i + 1] = strdup((const char *)atf_map_citer_data(iter));
        if (array[i + 1] == NULL) {
            atf_utils_free_charpp(array);
            array = NULL;
            goto out;
        }

        i += 2;
    }
    array[i] = NULL;

out:
    return array;
}

struct atf_tp_impl {
    atf_list_t m_tcs;
    atf_map_t m_config;
};

/*
 * Getters.
 */

char **
atf_tp_get_config(const atf_tp_t *tp)
{
    return atf_map_to_charpp(&tp->pimpl->m_config);
}

/*
 * Constructors and destructors.
 */

atf_error_t
atf_list_init(atf_list_t *l)
{
    struct list_entry *lebeg, *leend;

    l->m_begin = NULL;
    l->m_end = NULL;

    lebeg = atf_detail_list_new_entry(NULL, false);
    if (lebeg == NULL) {
        return atf_no_memory_error();
    }

    leend = atf_detail_list_new_entry(NULL, false);
    if (leend == NULL) {
        free(lebeg);
        return atf_no_memory_error();
    }

    lebeg->m_next = leend;
    lebeg->m_prev = NULL;

    leend->m_next = NULL;
    leend->m_prev = lebeg;

    l->m_size = 0;
    l->m_begin = lebeg;
    l->m_end = leend;

    return atf_no_error();
}

/*
 * Constructors and destructors.
 */

atf_error_t
atf_map_init(atf_map_t *m)
{
    return atf_list_init(&m->m_list);
}

void
atf_list_fini(atf_list_t *l)
{
    struct list_entry *le;
    size_t freed;

    le = (struct list_entry *)l->m_begin;
    freed = 0;
    while (le != NULL) {
        struct list_entry *lenext;

        lenext = le->m_next;
        delete_entry(le);
        le = lenext;

        freed++;
    }
    INV(freed == l->m_size + 2);
}

void
atf_map_fini(atf_map_t *m)
{
    atf_list_iter_t iter;

    atf_list_for_each(iter, &m->m_list) {
        struct map_entry *me = atf_list_iter_data(iter);

        if (me->m_managed)
            free(me->m_value);
        free(me->m_key);
        free(me);
        assert(!(((struct list_entry *)iter.m_entry)->m_managed));
    }
    atf_list_fini(&m->m_list);
}

atf_error_t
atf_map_init_charpp(atf_map_t *m, const char *const *array)
{
    atf_error_t err;
    const char *const *ptr = array;

    err = atf_map_init(m);
    if (atf_is_error(err))
        return err;

    if (array != NULL) {
        while (!atf_is_error(err) && *ptr != NULL) {
            const char *key, *value;

            key = *ptr;
            INV(key != NULL);
            ptr++;

            value = *ptr;
            if (value == NULL) {
                err = atf_libc_error(EINVAL, "List too short; no value for "
                    "key '%s' provided", key);  /* XXX: Not really libc_error */
                break;
            }
            ptr++;

            err = atf_map_insert(m, key, strdup(value), true);
        }
    }

    if (atf_is_error(err))
        atf_map_fini(m);

    return err;
}

atf_map_citer_t
atf_map_find_c(const atf_map_t *m, const char *key)
{
    atf_list_citer_t iter;

    atf_list_for_each_c(iter, &m->m_list) {
        const struct map_entry *me = atf_list_citer_data(iter);

        if (strcmp(me->m_key, key) == 0) {
            atf_map_citer_t i;
            i.m_map = m;
            i.m_entry = me;
            i.m_listiter = iter;
            return i;
        }
    }

    return atf_map_end_c(m);
}

bool
atf_tc_has_md_var(const atf_tc_t *tc, const char *name)
{
    atf_map_citer_t end, iter;

    iter = atf_map_find_c(&tc->pimpl->m_vars, name);
    end = atf_map_end_c(&tc->pimpl->m_vars);
    return !atf_equal_map_citer_map_citer(iter, end);
}

const char *
atf_tc_get_md_var(const atf_tc_t *tc, const char *name)
{
    const char *val;
    atf_map_citer_t iter;

    PRE(atf_tc_has_md_var(tc, name));
    iter = atf_map_find_c(&tc->pimpl->m_vars, name);
    val = atf_map_citer_data(iter);
    INV(val != NULL);

    return val;
}

static void
report_fatal_error(const char *msg, ...)
{
    va_list ap;
    assert(fprintf(stderr, "FATAL ERROR: ") == 13);

    va_start(ap, msg);
    assert(vfprintf(stderr, msg, ap) >= 0);
    va_end(ap);

    assert(fprintf(stderr, "\n") == 1);
    abort();
}

/*
 * Constructors/destructors.
 */

atf_error_t
atf_tc_init(atf_tc_t *tc, const char *ident, atf_tc_head_t head,
            atf_tc_body_t body, atf_tc_cleanup_t cleanup,
            const char *const *config)
{
    atf_error_t err;

    tc->pimpl = malloc(sizeof(struct atf_tc_impl));
    if (tc->pimpl == NULL) {
        err = atf_no_memory_error();
        goto err;
    }

    tc->pimpl->m_ident = ident;
    tc->pimpl->m_head = head;
    tc->pimpl->m_body = body;
    tc->pimpl->m_cleanup = cleanup;

    err = atf_map_init_charpp(&tc->pimpl->m_config, config);
    if (atf_is_error(err))
        goto err;

    err = atf_map_init(&tc->pimpl->m_vars);
    if (atf_is_error(err))
        goto err_vars;

    err = atf_tc_set_md_var(tc, "ident", ident);
    if (atf_is_error(err))
        goto err_map;

    if (cleanup != NULL) {
        err = atf_tc_set_md_var(tc, "has.cleanup", "true");
        if (atf_is_error(err))
            goto err_map;
    }

    /* XXX Should the head be able to return error codes? */
    if (tc->pimpl->m_head != NULL)
        tc->pimpl->m_head(tc);

    if (strcmp(atf_tc_get_md_var(tc, "ident"), ident) != 0) {
        report_fatal_error("Test case head modified the read-only 'ident' "
            "property");
        UNREACHABLE;
    }

    INV(!atf_is_error(err));
    return err;

err_map:
    atf_map_fini(&tc->pimpl->m_vars);
err_vars:
    atf_map_fini(&tc->pimpl->m_config);
err:
    return err;
}

atf_error_t
atf_tc_init_pack(atf_tc_t *tc, const atf_tc_pack_t *pack,
                 const char *const *config)
{
    return atf_tc_init(tc, pack->m_ident, pack->m_head, pack->m_body,
                       pack->m_cleanup, config);
}

static void
fail_check(struct context *ctx, atf_dynstr_t *reason)
{
    if (ctx->expect == EXPECT_FAIL) {
        assert(fprintf(stderr, "*** Expected check failure: %s: %s\n",
            atf_dynstr_cstring(&ctx->expect_reason),
            atf_dynstr_cstring(reason)) >= 0);
        ctx->expect_fail_count++;
    } else if (ctx->expect == EXPECT_PASS) {
        assert(fprintf(stderr, "*** Check failed: %s\n", atf_dynstr_cstring(reason)) == 19 + strlen(atf_dynstr_cstring(reason)));
        ctx->fail_count++;
    } else {
        error_in_expect(ctx, "Test case raised a failure but was not "
            "expecting one; reason was %s", atf_dynstr_cstring(reason));
    }

    atf_dynstr_fini(reason);
}

const char *
atf_tc_get_ident(const atf_tc_t *tc)
{
    return tc->pimpl->m_ident;
}

static
const atf_tc_t *
find_tc(const atf_tp_t *tp, const char *ident)
{
    const atf_tc_t *tc;
    atf_list_citer_t iter;

    tc = NULL;
    atf_list_for_each_c(iter, &tp->pimpl->m_tcs) {
        const atf_tc_t *tc2;
        tc2 = atf_list_citer_data(iter);
        if (strcmp(atf_tc_get_ident(tc2), ident) == 0) {
            tc = tc2;
            break;
        }
    }
    return tc;
}

/*
 * Modifiers.
 */

atf_error_t
atf_tp_add_tc(atf_tp_t *tp, atf_tc_t *tc)
{
    atf_error_t err;

    PRE(find_tc(tp, atf_tc_get_ident(tc)) == NULL);

    err = atf_list_append(&tp->pimpl->m_tcs, tc, false);

    POST(find_tc(tp, atf_tc_get_ident(tc)) != NULL);

    return err;
}

static const char *progname = NULL;

/* ---------------------------------------------------------------------
 * The "atf_fs_path" type.
 * --------------------------------------------------------------------- */

struct atf_fs_path {
    atf_dynstr_t m_data;
};
typedef struct atf_fs_path atf_fs_path_t;

enum tc_part {
    BODY,
    CLEANUP,
};

struct params {
    bool m_do_list;
    atf_fs_path_t m_srcdir;
    char *m_tcname;
    enum tc_part m_tcpart;
    atf_fs_path_t m_resfile;
    atf_map_t m_config;
};

atf_error_t
atf_dynstr_append_fmt(atf_dynstr_t *ad, const char *fmt, ...)
{
    va_list ap;
    atf_error_t err;

    va_start(ap, fmt);
    err = prepend_or_append(ad, fmt, ap, false);
    va_end(ap);

    return err;
}

static
atf_error_t
normalize(atf_dynstr_t *d, char *p)
{
    const char *ptr;
    char *last;
    atf_error_t err;
    bool first;

    PRE(strlen(p) > 0);
    PRE(atf_dynstr_length(d) == 0);

    if (p[0] == '/')
        err = atf_dynstr_append_fmt(d, "/");
    else
        err = atf_no_error();

    first = true;
    last = NULL; /* Silence GCC warning. */
    ptr = strtok_r(p, "/", &last);
    while (!atf_is_error(err) && ptr != NULL) {
        if (strlen(ptr) > 0) {
            err = atf_dynstr_append_fmt(d, "%s%s", first ? "" : "/", ptr);
            first = false;
        }

        ptr = strtok_r(NULL, "/", &last);
    }

    return err;
}


static
atf_error_t
normalize_ap(atf_dynstr_t *d, const char *p, va_list ap)
{
    char *str;
    atf_error_t err;
    va_list ap2;

    err = atf_dynstr_init(d);
    if (atf_is_error(err))
        goto out;

    va_copy(ap2, ap);
    err = atf_text_format_ap(&str, p, ap2);
    va_end(ap2);
    if (atf_is_error(err))
        atf_dynstr_fini(d);
    else {
        err = normalize(d, str);
        free(str);
    }

out:
    return err;
}

/*
 * Constructors/destructors.
 */

atf_error_t
atf_fs_path_init_ap(atf_fs_path_t *p, const char *fmt, va_list ap)
{
    atf_error_t err;
    va_list ap2;

    va_copy(ap2, ap);
    err = normalize_ap(&p->m_data, fmt, ap2);
    va_end(ap2);

    return err;
}

atf_error_t
atf_fs_path_init_fmt(atf_fs_path_t *p, const char *fmt, ...)
{
    va_list ap;
    atf_error_t err;

    va_start(ap, fmt);
    err = atf_fs_path_init_ap(p, fmt, ap);
    va_end(ap);

    return err;
}

atf_error_t
atf_dynstr_init_raw(atf_dynstr_t *ad, const void *mem, size_t memlen)
{
    atf_error_t err;

    if (memlen >= SIZE_MAX - 1) {
        err = atf_no_memory_error();
        goto out;
    }

    ad->m_data = (char *)malloc(memlen + 1);
    if (ad->m_data == NULL) {
        err = atf_no_memory_error();
        goto out;
    }

    ad->m_datasize = memlen + 1;
    memcpy(ad->m_data, mem, memlen);
    ad->m_data[memlen] = '\0';
    ad->m_length = strlen(ad->m_data);
    INV(ad->m_length <= memlen);
    err = atf_no_error();

out:
    return err;
}

atf_error_t
atf_dynstr_init_substr(atf_dynstr_t *ad, const atf_dynstr_t *src,
                       size_t beg, size_t end)
{
    if (beg > src->m_length)
        beg = src->m_length;

    if (end == atf_dynstr_npos || end > src->m_length)
        end = src->m_length;

    return atf_dynstr_init_raw(ad, src->m_data + beg, end - beg);
}

/*
 * Getters.
 */

atf_error_t
atf_fs_path_branch_path(const atf_fs_path_t *p, atf_fs_path_t *bp)
{
    const size_t endpos = atf_dynstr_rfind_ch(&p->m_data, '/');
    atf_error_t err;

    if (endpos == atf_dynstr_npos)
        err = atf_fs_path_init_fmt(bp, ".");
    else if (endpos == 0)
        err = atf_fs_path_init_fmt(bp, "/");
    else
        err = atf_dynstr_init_substr(&bp->m_data, &p->m_data, 0, endpos);

#if defined(HAVE_CONST_DIRNAME)
    INV(atf_equal_dynstr_cstring(&bp->m_data,
                                 dirname(atf_dynstr_cstring(&p->m_data))));
#endif /* defined(HAVE_CONST_DIRNAME) */

    return err;
}

const char *
atf_fs_path_cstring(const atf_fs_path_t *p)
{
    return atf_dynstr_cstring(&p->m_data);
}

void
atf_fs_path_fini(atf_fs_path_t *p)
{
    atf_dynstr_fini(&p->m_data);
}

static
atf_error_t
argv0_to_dir(const char *argv0, atf_fs_path_t *dir)
{
    atf_error_t err;
    atf_fs_path_t temp;

    err = atf_fs_path_init_fmt(&temp, "%s", argv0);
    if (atf_is_error(err))
        goto out;

    err = atf_fs_path_branch_path(&temp, dir);

    atf_fs_path_fini(&temp);
out:
    return err;
}

static
atf_error_t
params_init(struct params *p, const char *argv0)
{
    atf_error_t err;

    p->m_do_list = false;
    p->m_tcname = NULL;
    p->m_tcpart = BODY;

    err = argv0_to_dir(argv0, &p->m_srcdir);
    if (atf_is_error(err))
        return err;

    err = atf_fs_path_init_fmt(&p->m_resfile, "/dev/stdout");
    if (atf_is_error(err)) {
        atf_fs_path_fini(&p->m_srcdir);
        return err;
    }

    err = atf_map_init(&p->m_config);
    if (atf_is_error(err)) {
        atf_fs_path_fini(&p->m_resfile);
        atf_fs_path_fini(&p->m_srcdir);
        return err;
    }

    return err;
}

#define GETOPT_POSIX

static
atf_error_t
replace_path_param(atf_fs_path_t *param, const char *value)
{
    atf_error_t err;
    atf_fs_path_t temp;

    err = atf_fs_path_init_fmt(&temp, "%s", value);
    if (!atf_is_error(err)) {
        atf_fs_path_fini(param);
        *param = temp;
    }

    return err;
}

/* ---------------------------------------------------------------------
 * The "usage" and "user" error types.
 * --------------------------------------------------------------------- */

#define FREE_FORM_ERROR(name) \
    struct name ## _error_data { \
        char m_what[2048]; \
    }; \
    \
    static \
    void \
    name ## _format(const atf_error_t err, char *buf, size_t buflen) \
    { \
        const struct name ## _error_data *data; \
        \
        PRE(atf_error_is(err, #name)); \
        \
        data = atf_error_data(err); \
        assert(snprintf(buf, buflen, "%s", data->m_what) == strlen(data->m_what)); \
    } \
    \
    static \
    atf_error_t \
    name ## _error(const char *fmt, ...) \
    { \
        atf_error_t err; \
        struct name ## _error_data data; \
        va_list ap; \
        \
        va_start(ap, fmt); \
        assert(vsnprintf(data.m_what, sizeof(data.m_what), fmt, ap) == strlen(data.m_what)); \
        va_end(ap); \
        \
        err = atf_error_new(#name, &data, sizeof(data), name ## _format); \
        \
        return err; \
    }

FREE_FORM_ERROR(usage); // NOLINT(misc-misplaced-const)
FREE_FORM_ERROR(user); // NOLINT(misc-misplaced-const)

static
atf_error_t
parse_vflag(char *arg, atf_map_t *config)
{
    atf_error_t err;
    char *split;

    split = strchr(arg, '=');
    if (split == NULL) {
        err = usage_error("-v requires an argument of the form var=value");
        goto out;
    }

    *split = '\0';
    split++;

    err = atf_map_insert(config, arg, split, false);

out:
    return err;
}

static
atf_error_t
handle_tcarg(const char *tcarg, char **tcname, enum tc_part *tcpart)
{
    atf_error_t err;

    err = atf_no_error();

    *tcname = strdup(tcarg);
    if (*tcname == NULL) {
        err = atf_no_memory_error();
        goto out;
    }

    char *delim = strchr(*tcname, ':');
    if (delim != NULL) {
        *delim = '\0';

        delim++;
        if (strcmp(delim, "body") == 0) {
            *tcpart = BODY;
        } else if (strcmp(delim, "cleanup") == 0) {
            *tcpart = CLEANUP;
        } else {
            err = usage_error("Invalid test case part `%s'", delim);
            goto out;
        }
    }

out:
    return err;
}

static
void
params_fini(struct params *p)
{
    atf_map_fini(&p->m_config);
    atf_fs_path_fini(&p->m_resfile);
    atf_fs_path_fini(&p->m_srcdir);
    if (p->m_tcname != NULL)
        free(p->m_tcname);
}

static
atf_error_t
process_params(int argc, char **argv, struct params *p)
{
    atf_error_t err;
    int ch;
    int old_opterr;

    err = params_init(p, argv[0]);
    if (atf_is_error(err))
        goto out;

    old_opterr = opterr;
    opterr = 0;
    while (!atf_is_error(err) &&
           (ch = getopt(argc, argv, GETOPT_POSIX ":lr:s:v:")) != -1) {
        switch (ch) {
        case 'l':
            p->m_do_list = true;
            break;

        case 'r':
            err = replace_path_param(&p->m_resfile, optarg);
            break;

        case 's':
            err = replace_path_param(&p->m_srcdir, optarg);
            break;

        case 'v':
            err = parse_vflag(optarg, &p->m_config);
            break;

        case ':':
            err = usage_error("Option -%c requires an argument.", optopt);
            break;

        case '?':
        default:
            err = usage_error("Unknown option -%c.", optopt);
        }
    }
    argc -= optind;
    argv += optind;

    /* Clear getopt state just in case the test wants to use it. */
    opterr = old_opterr;
    optind = 1;
#if !defined(YALIBCT_LIBC_DOESNT_HAVE_OPTRESET)
    optreset = 1;
#endif

    if (!atf_is_error(err)) {
        if (p->m_do_list) {
            if (argc > 0)
                err = usage_error("Cannot provide test case names with -l");
        } else {
            if (argc == 0)
                ;//err = usage_error("Must provide a test case name");
            else if (argc == 1)
                err = handle_tcarg(argv[0], &p->m_tcname, &p->m_tcpart);
            else if (argc > 1) {
                err = usage_error("Cannot provide more than one test case "
                                  "name");
            }
        }
    }

    if (atf_is_error(err))
        params_fini(p);

out:
    return err;
}

atf_error_t
atf_dynstr_copy(atf_dynstr_t *dest, const atf_dynstr_t *src)
{
    atf_error_t err;

    dest->m_data = (char *)malloc(src->m_datasize);
    if (dest->m_data == NULL)
        err = atf_no_memory_error();
    else {
        memcpy(dest->m_data, src->m_data, src->m_datasize);
        dest->m_datasize = src->m_datasize;
        dest->m_length = src->m_length;
        err = atf_no_error();
    }

    return err;
}

atf_error_t
atf_fs_path_copy(atf_fs_path_t *dest, const atf_fs_path_t *src)
{
    return atf_dynstr_copy(&dest->m_data, &src->m_data);
}

bool
atf_fs_path_is_absolute(const atf_fs_path_t *p)
{
    return atf_dynstr_cstring(&p->m_data)[0] == '/';
}

/*
 * Operators.
 */

bool
atf_equal_dynstr_cstring(const atf_dynstr_t *ad, const char *str)
{
    return strcmp(ad->m_data, str) == 0;
}

bool
atf_equal_dynstr_dynstr(const atf_dynstr_t *s1, const atf_dynstr_t *s2)
{
    return strcmp(s1->m_data, s2->m_data) == 0;
}

bool
atf_fs_path_is_root(const atf_fs_path_t *p)
{
    return atf_equal_dynstr_cstring(&p->m_data, "/");
}

atf_error_t
atf_fs_getcwd(atf_fs_path_t *p)
{
    atf_error_t err;
    char cwd[PATH_MAX];

    if (getcwd(cwd, PATH_MAX) == NULL) {
        err = atf_libc_error(errno, "Cannot determine current directory");
        goto out;
    }

    err = atf_fs_path_init_fmt(p, "%s", cwd);

out:
    return err;
}

atf_error_t
atf_fs_path_append_ap(atf_fs_path_t *p, const char *fmt, va_list ap)
{
    atf_dynstr_t aux;
    atf_error_t err;
    va_list ap2;

    va_copy(ap2, ap);
    err = normalize_ap(&aux, fmt, ap2);
    va_end(ap2);
    if (!atf_is_error(err)) {
        const char *auxstr = atf_dynstr_cstring(&aux);
        const bool needslash = auxstr[0] != '/';

        err = atf_dynstr_append_fmt(&p->m_data, "%s%s",
                                    needslash ? "/" : "", auxstr);

        atf_dynstr_fini(&aux);
    }

    return err;
}

atf_error_t
atf_fs_path_append_fmt(atf_fs_path_t *p, const char *fmt, ...)
{
    va_list ap;
    atf_error_t err;

    va_start(ap, fmt);
    err = atf_fs_path_append_ap(p, fmt, ap);
    va_end(ap);

    return err;
}

atf_error_t
atf_fs_path_append_path(atf_fs_path_t *p, const atf_fs_path_t *p2)
{
    return atf_fs_path_append_fmt(p, "%s", atf_dynstr_cstring(&p2->m_data));
}

atf_error_t
atf_fs_path_to_absolute(const atf_fs_path_t *p, atf_fs_path_t *pa)
{
    atf_error_t err;

    PRE(!atf_fs_path_is_absolute(p));

    err = atf_fs_getcwd(pa);
    if (atf_is_error(err))
        goto out;

    err = atf_fs_path_append_path(pa, p);
    if (atf_is_error(err))
        atf_fs_path_fini(pa);

out:
    return err;
}

atf_error_t
atf_fs_path_leaf_name(const atf_fs_path_t *p, atf_dynstr_t *ln)
{
    size_t begpos = atf_dynstr_rfind_ch(&p->m_data, '/');
    atf_error_t err;

    if (begpos == atf_dynstr_npos)
        begpos = 0;
    else
        begpos++;

    err = atf_dynstr_init_substr(ln, &p->m_data, begpos, atf_dynstr_npos);

#if defined(HAVE_CONST_BASENAME)
    INV(atf_equal_dynstr_cstring(ln,
                                 basename(atf_dynstr_cstring(&p->m_data))));
#endif /* defined(HAVE_CONST_BASENAME) */

    return err;
}

static
atf_error_t
srcdir_strip_libtool(atf_fs_path_t *srcdir)
{
    atf_error_t err;
    atf_fs_path_t parent;

    err = atf_fs_path_branch_path(srcdir, &parent);
    if (atf_is_error(err))
        goto out;

    atf_fs_path_fini(srcdir);
    *srcdir = parent;

    INV(!atf_is_error(err));
out:
    return err;
}

const int atf_fs_access_f = 1 << 0;
const int atf_fs_access_r = 1 << 1;
const int atf_fs_access_w = 1 << 2;
const int atf_fs_access_x = 1 << 3;

bool
atf_user_is_root(void)
{
    return geteuid() == 0;
}

bool
atf_user_is_unprivileged(void)
{
    return geteuid() != 0;
}

uid_t
atf_user_euid(void)
{
    return geteuid();
}

bool
atf_user_is_member_of_group(gid_t gid)
{
    static gid_t *groups;
    static int ngroups = -1;
    bool found;
    int i;

    if (ngroups == -1 || groups == NULL) {
        groups = malloc(sizeof(gid_t) * NGROUPS_MAX);
        INV(groups != 0);
        ngroups = getgroups(NGROUPS_MAX, groups);
        INV(ngroups >= 0);
    }

    found = false;
    for (i = 0; !found && i < ngroups; i++)
        if (groups[i] == gid)
            found = true;
    return found;
}

/*
 * An implementation of access(2) but using the effective user value
 * instead of the real one.  Also avoids false positives for root when
 * asking for execute permissions, which appear in SunOS.
 */
atf_error_t
atf_fs_eaccess(const atf_fs_path_t *p, int mode)
{
    atf_error_t err;
    struct stat st;
    bool ok;

    PRE(mode & atf_fs_access_f || mode & atf_fs_access_r ||
        mode & atf_fs_access_w || mode & atf_fs_access_x);

    if (lstat(atf_fs_path_cstring(p), &st) == -1) {
        err = atf_libc_error(errno, "Cannot get information from file %s",
                             atf_fs_path_cstring(p));
        goto out;
    }

    err = atf_no_error();

    /* Early return if we are only checking for existence and the file
     * exists (stat call returned). */
    if (mode & atf_fs_access_f)
        goto out;

    ok = false;
    if (atf_user_is_root()) {
        if (!ok && !(mode & atf_fs_access_x)) {
            /* Allow root to read/write any file. */
            ok = true;
        }

        if (!ok && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
            /* Allow root to execute the file if any of its execution bits
             * are set. */
            ok = true;
        }
    } else {
        if (!ok && (atf_user_euid() == st.st_uid)) {
            ok = ((mode & atf_fs_access_r) && (st.st_mode & S_IRUSR)) ||
                 ((mode & atf_fs_access_w) && (st.st_mode & S_IWUSR)) ||
                 ((mode & atf_fs_access_x) && (st.st_mode & S_IXUSR));
        }
        if (!ok && atf_user_is_member_of_group(st.st_gid)) {
            ok = ((mode & atf_fs_access_r) && (st.st_mode & S_IRGRP)) ||
                 ((mode & atf_fs_access_w) && (st.st_mode & S_IWGRP)) ||
                 ((mode & atf_fs_access_x) && (st.st_mode & S_IXGRP));
        }
        if (!ok && ((atf_user_euid() != st.st_uid) &&
                    !atf_user_is_member_of_group(st.st_gid))) {
            ok = ((mode & atf_fs_access_r) && (st.st_mode & S_IROTH)) ||
                 ((mode & atf_fs_access_w) && (st.st_mode & S_IWOTH)) ||
                 ((mode & atf_fs_access_x) && (st.st_mode & S_IXOTH));
        }
    }

    if (!ok)
        err = atf_libc_error(EACCES, "Access check failed");

out:
    return err;
}

int
atf_libc_error_code(const atf_error_t err) // NOLINT(misc-misplaced-const)
{
    const struct atf_libc_error_data *data;

    PRE(atf_error_is(err, "libc"));

    data = atf_error_data(err);

    return data->m_errno;
}

const char *
atf_libc_error_msg(const atf_error_t err) // NOLINT(misc-misplaced-const)
{
    const struct atf_libc_error_data *data;

    PRE(atf_error_is(err, "libc"));

    data = atf_error_data(err);

    return data->m_what;
}

atf_error_t
atf_fs_exists(const atf_fs_path_t *p, bool *b)
{
    atf_error_t err;

    err = atf_fs_eaccess(p, atf_fs_access_f);
    if (atf_is_error(err)) {
        if (atf_error_is(err, "libc") && atf_libc_error_code(err) == ENOENT) {
            atf_error_free(err);
            err = atf_no_error();
            *b = false;
        }
    } else
        *b = true;

    return err;
}

static
atf_error_t
handle_srcdir(struct params *p)
{
    atf_error_t err;
    atf_dynstr_t leafname;
    atf_fs_path_t exe, srcdir;
    bool b;

    err = atf_fs_path_copy(&srcdir, &p->m_srcdir);
    if (atf_is_error(err))
        goto out;

    if (!atf_fs_path_is_absolute(&srcdir)) {
        atf_fs_path_t srcdirabs;

        err = atf_fs_path_to_absolute(&srcdir, &srcdirabs);
        if (atf_is_error(err))
            goto out_srcdir;

        atf_fs_path_fini(&srcdir);
        srcdir = srcdirabs;
    }

    err = atf_fs_path_leaf_name(&srcdir, &leafname);
    if (atf_is_error(err))
        goto out_srcdir;
    else {
        const bool libs = atf_equal_dynstr_cstring(&leafname, ".libs");
        atf_dynstr_fini(&leafname);

        if (libs) {
            err = srcdir_strip_libtool(&srcdir);
            if (atf_is_error(err))
                goto out;
        }
    }

    err = atf_fs_path_copy(&exe, &srcdir);
    if (atf_is_error(err))
        goto out_srcdir;

    err = atf_fs_path_append_fmt(&exe, "%s", progname);
    if (atf_is_error(err))
        goto out_exe;

    err = atf_fs_exists(&exe, &b);
    if (!atf_is_error(err)) {
        if (b) {
            err = atf_map_insert(&p->m_config, "srcdir",
                                 strdup(atf_fs_path_cstring(&srcdir)), true);
        } else {
            err = user_error("Cannot find the test program in the source "
                             "directory `%s'", atf_fs_path_cstring(&srcdir));
        }
    }

out_exe:
    atf_fs_path_fini(&exe);
out_srcdir:
    atf_fs_path_fini(&srcdir);
out:
    return err;
}

atf_error_t
atf_tp_init(atf_tp_t *tp, const char *const *config)
{
    atf_error_t err;

    PRE(config != NULL);

    tp->pimpl = malloc(sizeof(struct atf_tp_impl));
    if (tp->pimpl == NULL)
        return atf_no_memory_error();

    err = atf_list_init(&tp->pimpl->m_tcs);
    if (atf_is_error(err)) {
        free(tp->pimpl);
        goto out;
    }

    err = atf_map_init_charpp(&tp->pimpl->m_config, config);
    if (atf_is_error(err)) {
        atf_list_fini(&tp->pimpl->m_tcs);
        free(tp->pimpl);
        goto out;
    }

    INV(!atf_is_error(err));
out:
    return err;
}

const atf_tc_t *const *
atf_tp_get_tcs(const atf_tp_t *tp)
{
    const atf_tc_t **array;
    atf_list_citer_t iter;
    size_t i;

    array = (const atf_tc_t **)malloc(sizeof(atf_tc_t *) *
                   (atf_list_size(&tp->pimpl->m_tcs) + 1));
    if (array == NULL)
        goto out;

    i = 0;
    atf_list_for_each_c(iter, &tp->pimpl->m_tcs) {
        array[i] = atf_list_citer_data(iter);
        if (array[i] == NULL) {
            free((void *)array);
            array = NULL;
            goto out;
        }

        i++;
    }
    array[i] = NULL;

out:
    return array;
}

char **
atf_tc_get_md_vars(const atf_tc_t *tc)
{
    return atf_map_to_charpp(&tc->pimpl->m_vars);
}

static
void
list_tcs(const atf_tp_t *tp)
{
    const atf_tc_t *const *tcs;
    const atf_tc_t *const *tcsptr;

    printf("Content-Type: application/X-atf-tp; version=\"1\"\n\n");

    tcs = atf_tp_get_tcs(tp);
    INV(tcs != NULL);  /* Should be checked. */
    for (tcsptr = tcs; *tcsptr != NULL; tcsptr++) {
        const atf_tc_t *tc = *tcsptr;
        char **vars = atf_tc_get_md_vars(tc);
        char **ptr;

        INV(vars != NULL);  /* Should be checked. */

        if (tcsptr != tcs)  /* Not first. */
            printf("\n");

        for (ptr = vars; *ptr != NULL; ptr += 2) {
            if (strcmp(*ptr, "ident") == 0) {
                printf("ident: %s\n", *(ptr + 1));
                break;
            }
        }

        for (ptr = vars; *ptr != NULL; ptr += 2) {
            if (strcmp(*ptr, "ident") != 0) {
                printf("%s: %s\n", *ptr, *(ptr + 1));
            }
        }

        atf_utils_free_charpp(vars);
    }

    free((void *)tcs);
}

bool
atf_tp_has_tc(const atf_tp_t *tp, const char *id)
{
    const atf_tc_t *tc = find_tc(tp, id);
    return tc != NULL;
}

const atf_tc_t *
atf_tp_get_tc(const atf_tp_t *tp, const char *id)
{
    const atf_tc_t *tc = find_tc(tp, id);
    PRE(tc != NULL);
    return tc;
}

const char *
atf_env_get(const char *name)
{
    const char* val = getenv(name);
    PRE(val != NULL);
    return val;
}

const char *
atf_env_get_with_default(const char *name, const char *default_value)
{
    const char* val = getenv(name);
    if (val == NULL)
        return default_value;
    else
        return val;
}

bool
atf_env_has(const char *name)
{
    return getenv(name) != NULL;
}

static
void
print_warning(const char *message)
{
    assert(fprintf(stderr, "%s: WARNING: %s\n", progname, message) == strlen(progname) + strlen(message) + 12);
}

atf_error_t
atf_tc_cleanup(const atf_tc_t *tc)
{
    if (tc->pimpl->m_cleanup != NULL)
        tc->pimpl->m_cleanup(tc);
    return atf_no_error(); /* XXX */
}

atf_error_t
atf_tp_cleanup(const atf_tp_t *tp, const char *tcname)
{
    const atf_tc_t *tc;

    tc = find_tc(tp, tcname);
    PRE(tc != NULL);

    return atf_tc_cleanup(tc);
}

static void
context_init(struct context *ctx, const atf_tc_t *tc, const char *resfile)
{
    ctx->tc = tc;
    ctx->resfile = resfile;
    ctx->fail_count = 0;
    ctx->expect = EXPECT_PASS;
    check_fatal_error(atf_dynstr_init(&ctx->expect_reason));
    ctx->expect_previous_fail_count = 0;
    ctx->expect_fail_count = 0;
    ctx->expect_exitcode = 0;
    ctx->expect_signo = 0;
}

static void
format_reason_fmt(atf_dynstr_t *out_reason,
                  const char *source_file, const size_t source_line,
                  const char *reason, ...)
{
    va_list ap;

    va_start(ap, reason);
    format_reason_ap(out_reason, source_file, source_line, reason, ap);
    va_end(ap);
}

atf_error_t
atf_tc_run(const atf_tc_t *tc, const char *resfile)
{
    context_init(&Current, tc, resfile);

    tc->pimpl->m_body(tc);

    validate_expect(&Current);

    if (Current.fail_count > 0) {
        atf_dynstr_t reason;

        format_reason_fmt(&reason, NULL, 0, "%d checks failed; see output for "
            "more details", Current.fail_count);
        fail_requirement(&Current, &reason);
    } else if (Current.expect_fail_count > 0) {
        atf_dynstr_t reason;

        format_reason_fmt(&reason, NULL, 0, "%d checks failed as expected; "
            "see output for more details", Current.expect_fail_count);
        expected_failure(&Current, &reason);
    } else {
        pass(&Current);
    }
    //UNREACHABLE;
    return atf_no_error();
}

atf_error_t
atf_tp_run(const atf_tp_t *tp, const char *tcname, const char *resfile)
{
    const atf_tc_t *tc;

    tc = find_tc(tp, tcname);
    PRE(tc != NULL);

    return atf_tc_run(tc, resfile);
}

static
atf_error_t
run_tc(const atf_tp_t *tp, struct params *p, int *exitcode)
{
    atf_error_t err;

    if (!atf_tp_has_tc(tp, p->m_tcname)) {
        err = usage_error("Unknown test case `%s'", p->m_tcname);
        goto out;
    }

    if (!atf_env_has("__RUNNING_INSIDE_ATF_RUN") || strcmp(atf_env_get(
        "__RUNNING_INSIDE_ATF_RUN"), "internal-yes-value") != 0)
    {
        //print_warning("Running test cases outside of kyua(1) is unsupported");
        //print_warning("No isolation nor timeout control is being applied; you "
        //              "may get unexpected failures; see atf-test-case(4)");
    }

    switch (p->m_tcpart) {
    case BODY:
        err = atf_tp_run(tp, p->m_tcname, atf_fs_path_cstring(&p->m_resfile));
        if (atf_is_error(err)) {
            /* TODO: Handle error */
            *exitcode = EXIT_FAILURE;
            atf_error_free(err);
        } else {
            *exitcode = EXIT_SUCCESS;
        }

        break;

    case CLEANUP:
        err = atf_tp_cleanup(tp, p->m_tcname);
        if (atf_is_error(err)) {
            /* TODO: Handle error */
            *exitcode = EXIT_FAILURE;
            atf_error_free(err);
        } else {
            *exitcode = EXIT_SUCCESS;
        }

        break;

    default:
        UNREACHABLE;
    }

    INV(!atf_is_error(err));
out:
    return err;
}

void
atf_tc_fini(atf_tc_t *tc)
{
    atf_map_fini(&tc->pimpl->m_vars);
    free(tc->pimpl);
}

void
atf_tp_fini(atf_tp_t *tp)
{
    atf_list_iter_t iter;

    atf_map_fini(&tp->pimpl->m_config);

    atf_list_for_each(iter, &tp->pimpl->m_tcs) {
        atf_tc_t *tc = atf_list_iter_data(iter);
        atf_tc_fini(tc);
    }
    atf_list_fini(&tp->pimpl->m_tcs);

    free(tp->pimpl);
}

static
atf_error_t
controlled_main(int argc, char **argv,
                atf_error_t (*add_tcs_hook)(atf_tp_t *),
                int *exitcode)
{
    atf_error_t err;
    struct params p;
    atf_tp_t tp;
    char **raw_config;

    err = process_params(argc, argv, &p);
    if (atf_is_error(err))
        goto out;

    err = handle_srcdir(&p);
    if (atf_is_error(err))
        goto out_p;

    raw_config = atf_map_to_charpp(&p.m_config);
    if (raw_config == NULL) {
        err = atf_no_memory_error();
        goto out_p;
    }
    err = atf_tp_init(&tp, (const char* const*)raw_config);
    atf_utils_free_charpp(raw_config);
    if (atf_is_error(err))
        goto out_p;

    err = add_tcs_hook(&tp);
    if (atf_is_error(err))
        goto out_tp;

    if (p.m_do_list) {
        list_tcs(&tp);
        INV(!atf_is_error(err));
        *exitcode = EXIT_SUCCESS;
    } else {
        //err = run_tc(&tp, &p, exitcode);

        atf_list_citer_t iter;

        atf_list_for_each_c(iter, &tp.pimpl->m_tcs) {
            const atf_tc_t *tc = atf_list_citer_data(iter);
            struct params fake_params = {};
            fake_params.m_tcname = (char *)atf_tc_get_ident(tc);
            fake_params.m_tcpart = BODY;
            fake_params.m_resfile.m_data.m_data = (char *)"/dev/stdout";
            fake_params.m_resfile.m_data.m_length = strlen("/dev/stdout");
            fake_params.m_resfile.m_data.m_datasize = fake_params.m_resfile.m_data.m_length;

            yalibct_chdir_to_tmpdir();
            run_tc(&tp, &fake_params, exitcode);
            yalibct_chdir_to_tmpdir_remove_tmpdir();

            if (*exitcode == EXIT_FAILURE)
                break;
        }
    }

out_tp:
    atf_tp_fini(&tp);
out_p:
    params_fini(&p);
out:
    return err;
}

/* ---------------------------------------------------------------------
 * Printing functions.
 * --------------------------------------------------------------------- */

static
void
print_error(const atf_error_t err) // NOLINT(misc-misplaced-const)
{
    char buf[4096];

    PRE(atf_is_error(err));

    atf_error_format(err, buf, sizeof(buf));
    assert(fprintf(stderr, "%s: ERROR: %s\n", progname != NULL ? progname : "(progname null)", buf) == strlen(progname != NULL ? progname : "(progname null)") + strlen(buf) + 10);

    if (atf_error_is(err, "usage"))
        assert(fprintf(stderr, "%s: See atf-test-program(1) for usage details.\n",
                       progname) >= 0);
}

int
atf_tp_main(int argc, char **argv, atf_error_t (*add_tcs_hook)(atf_tp_t *))
{
    atf_error_t err;
    int exitcode;

    progname = strrchr(argv[0], '/');
    if (progname == NULL)
        progname = argv[0];
    else
        progname++;

    /* Libtool workaround: if running from within the source tree (binaries
     * that are not installed yet), skip the "lt-" prefix added to files in
     * the ".libs" directory to show the real (not temporary) name. */
    if (strncmp(progname, "lt-", 3) == 0)
        progname += 3;

    exitcode = EXIT_FAILURE; /* Silence GCC warning. */
    err = controlled_main(argc, argv, add_tcs_hook, &exitcode);
    if (atf_is_error(err)) {
        print_error(err);
        atf_error_free(err);
        exitcode = EXIT_FAILURE;
    }

    return exitcode;
}

/** Ensures that the "expect" state is correct.
 *
 * Call this function before modifying the current value of expect.
 */
static void
validate_expect(struct context *ctx)
{
    if (ctx->expect == EXPECT_DEATH) {
        error_in_expect(ctx, "Test case was expected to terminate abruptly "
            "but it continued execution");
    } else if (ctx->expect == EXPECT_EXIT) {
        error_in_expect(ctx, "Test case was expected to exit cleanly but it "
            "continued execution");
    } else if (ctx->expect == EXPECT_FAIL) {
        if (ctx->expect_fail_count == ctx->expect_previous_fail_count)
            error_in_expect(ctx, "Test case was expecting a failure but none "
                "were raised");
        else
            INV(ctx->expect_fail_count > ctx->expect_previous_fail_count);
    } else if (ctx->expect == EXPECT_PASS) {
        /* Nothing to validate. */
    } else if (ctx->expect == EXPECT_SIGNAL) {
        error_in_expect(ctx, "Test case was expected to receive a termination "
            "signal but it continued execution");
    } else if (ctx->expect == EXPECT_TIMEOUT) {
        error_in_expect(ctx, "Test case was expected to hang but it continued "
            "execution");
    } else
        UNREACHABLE;
}

static void
pass(struct context *ctx)
{
    if (ctx->expect == EXPECT_FAIL) {
        error_in_expect(ctx, "Test case was expecting a failure but got "
            "a pass instead");
    } else if (ctx->expect == EXPECT_PASS) {
        //create_resfile(ctx->resfile, "passed", -1, NULL);
        //exit(EXIT_SUCCESS);
    } else {
        error_in_expect(ctx, "Test case asked to explicitly pass but was "
            "not expecting such condition");
    }
    //UNREACHABLE;
}

#define ATF_CHECK_EQ_MSG(expected, actual, fmt, ...) \
    ATF_CHECK_MSG((expected) == (actual), "%s != %s: " fmt, \
                  #expected, #actual, ##__VA_ARGS__)

#define ATF_REQUIRE_STREQ(expected, actual) \
    ATF_REQUIRE_MSG(strcmp(expected, actual) == 0, "%s != %s (%s != %s)", \
                    #expected, #actual, expected, actual)

#define ATF_CHECK_STREQ(expected, actual) \
    ATF_CHECK_MSG(strcmp(expected, actual) == 0, "%s != %s (%s != %s)", \
                  #expected, #actual, expected, actual)

#define ATF_REQUIRE_STREQ_MSG(expected, actual, fmt, ...) \
    ATF_REQUIRE_MSG(strcmp(expected, actual) == 0, \
                    "%s != %s (%s != %s): " fmt, \
                    #expected, #actual, expected, actual, ##__VA_ARGS__)

#define ATF_CHECK_STREQ_MSG(expected, actual, fmt, ...) \
    ATF_CHECK_MSG(strcmp(expected, actual) == 0, \
                  "%s != %s (%s != %s): " fmt, \
                  #expected, #actual, expected, actual, ##__VA_ARGS__)

#define ATF_REQUIRE_MATCH(regexp, string) \
    ATF_REQUIRE_MSG(atf_utils_grep_string("%s", string, regexp), \
                    "'%s' not matched in '%s'", regexp, string);

#define ATF_CHECK_MATCH(regexp, string) \
    ATF_CHECK_MSG(atf_utils_grep_string("%s", string, regexp), \
                  "'%s' not matched in '%s'", regexp, string);

#define ATF_REQUIRE_MATCH_MSG(regexp, string, fmt, ...) \
    ATF_REQUIRE_MSG(atf_utils_grep_string("%s", string, regexp), \
                    "'%s' not matched in '%s': " fmt, regexp, string, \
                    ##__VA_ARGS__);

#define ATF_CHECK_MATCH_MSG(regexp, string, fmt, ...) \
    ATF_CHECK_MSG(atf_utils_grep_string("%s", string, regexp), \
                  "'%s' not matched in '%s': " fmt, regexp, string, \
                  ##__VA_ARGS__);

#define ATF_CHECK_ERRNO(exp_errno, bool_expr) \
    atf_tc_check_errno(__FILE__, __LINE__, exp_errno, #bool_expr, bool_expr)

#define ATF_REQUIRE_ERRNO(exp_errno, bool_expr) \
    atf_tc_require_errno(__FILE__, __LINE__, exp_errno, #bool_expr, bool_expr)

static void
errno_test(struct context *ctx, const char *file, const size_t line,
           const int exp_errno, const char *expr_str,
           const bool expr_result,
           void (*fail_func)(struct context *, atf_dynstr_t *))
{
    const int actual_errno = errno;

    if (expr_result) {
        if (exp_errno != actual_errno) {
            atf_dynstr_t reason;

            format_reason_fmt(&reason, file, line, "Expected errno %d, got %d, "
                "in %s", exp_errno, actual_errno, expr_str);
            fail_func(ctx, &reason);
        }
    } else {
        atf_dynstr_t reason;

        format_reason_fmt(&reason, file, line, "Expected true value in %s",
            expr_str);
        fail_func(ctx, &reason);
    }
}

static void
_atf_tc_require_errno(struct context *ctx, const char *file, const size_t line,
                      const int exp_errno, const char *expr_str,
                      const bool expr_result)
{
    errno_test(ctx, file, line, exp_errno, expr_str, expr_result,
        fail_requirement);
}

void
atf_tc_require_errno(const char *file, const size_t line, const int exp_errno,
                     const char *expr_str, const bool expr_result)
{
    PRE(Current.tc != NULL);

    _atf_tc_require_errno(&Current, file, line, exp_errno, expr_str,
                          expr_result);
}
