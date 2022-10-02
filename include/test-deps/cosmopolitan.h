// Based on code with this license:
/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/

#pragma once

#include "test-lib/portable-symbols/getprogname.h"
#include "test-lib/portable-symbols/wcsncmp.h"
#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#define TEST(SUITE, NAME) void SUITE ## _ ## NAME (void)

#define EXPECT_STREQ(WANT, GOT) \
  assertStringEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, false)

#define FILIFU           __FILE__, __LINE__, __FUNCTION__,
#define FILIFU_OBJ(...)  __FILE__, __LINE__, ##__VA_ARGS__
#define FILIFU_ARGS      const char *file, int line, const char *func,
#define FILIFU_FROM(OBJ) (OBJ)->file, (OBJ)->line, __FUNCTION__,
#define FILIFU_FIELDS \
  const char *file;   \
  int line
#define testonly

#ifdef __GNUC__
#define DebugBreak() __builtin_trap()
#else
#define DebugBreak() (void)0
#endif

// Obviously this means some of the functionality from kprintf is missing... just be careful not to use those parts, I guess
#define kprintf printf

#define CLS    (!__nocolor ? "\r\e[J" : "")
#define RED    (!__nocolor ? "\e[30;101m" : "")
#define GREEN  (!__nocolor ? "\e[32m" : "")
#define UNBOLD (!__nocolor ? "\e[22m" : "")
#define RED2   (!__nocolor ? "\e[91;1m" : "")
#define BLUE1  (!__nocolor ? "\e[94;49m" : "")
#define BLUE2  (!__nocolor ? "\e[34m" : "")
#define RESET  (!__nocolor ? "\e[0m" : "")
#define SUBTLE (!__nocolor ? "\e[35m" : "")

#define GetProgramExecutableName getprogname

#define dontdiscard HEDLEY_WARN_UNUSED_RESULT

#ifndef forceinline
#ifdef __cplusplus
#define forceinline inline
#else
#if !defined(__STRICT_ANSI__) && \
    (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 302
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 403 || \
    !defined(__cplusplus) ||                              \
    (defined(__clang__) &&                                \
     (defined(__GNUC_STDC_INLINE__) || defined(__GNUC_GNU_INLINE__)))
#if defined(__GNUC_STDC_INLINE__) || defined(__cplusplus)
#define forceinline                                                 \
  static __inline __attribute__((__always_inline__, __gnu_inline__, \
                                 __unused__))
#else
#define forceinline              \
  static __inline __attribute__( \
      (__always_inline__, __unused__))
#endif /* __GNUC_STDC_INLINE__ */
#endif /* GCC >= 4.3 */
#elif defined(_MSC_VER)
#define forceinline __forceinline
#else
#define forceinline static inline
#endif /* !ANSI && GCC >= 3.2 */
#endif /* __cplusplus */
#endif /* forceinline */

#ifndef wontreturn
#if !defined(__STRICT_ANSI__) &&      \
    (__has_attribute(__noreturn__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 208)
#define wontreturn __attribute__((__noreturn__))
#else
#define wontreturn
#endif
#endif

#define MIN(X, Y)           ((Y) > (X) ? (X) : (Y))

static _Atomic(unsigned) g_testlib_ran;
static bool g_testlib_shoulddebugbreak;
static bool __nocolor;
static char g_fixturename[256];
static _Atomic(unsigned) g_testlib_failed;

static inline testonly void testlib_showerror(const char *file, int line, const char *func,
                                const char *method, const char *symbol,
                                const char *code, char *v1, char *v2) {
  char *p;
  char hostname[128];
  stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  kprintf("%serror%s%s:%s:%ld%s: %s() in %s(%s) on %s pid %d"
#ifdef YALIBCT_LIBC_HAS_GETTID
          "tid %d"
#endif
          "\n"
          "\t%s\n"
          "\t\tneed %s %s\n"
          "\t\t got %s\n"
          "\t%s%s\n"
          "\t%s%s\n",
          RED2, UNBOLD, BLUE1, file, (long)line, RESET, method, func,
          g_fixturename, hostname, getpid(),
#ifdef YALIBCT_LIBC_HAS_GETTID
          gettid(),
#endif
          code, v1, symbol, v2,
          SUBTLE, strerror(errno), GetProgramExecutableName(), RESET);
  free(v1);
  free(v2);
}


static inline testonly bool testlib_strnequals(size_t cw, const void *s1, const void *s2,
                                 size_t n) {
  if (s1 == s2) return true;
  if (!s1 || !s2) return false;
  return (cw == sizeof(wchar_t)
              ? wcsncmp(s1, s2, n)
              //: cw == sizeof(char16_t) ? strncmp16(s1, s2, n)
                                       : strncmp(s1, s2, n)) == 0;
}

static inline testonly bool testlib_strequals(size_t cw, const void *s1, const void *s2) {
  return testlib_strnequals(cw, s1, s2, SIZE_MAX);
}

static inline void xdie(void) {
  //if (weaken(__die)) __die();
  abort();
}

static inline char *(xvasprintf)(const char *fmt, va_list va) {
  char *buf;
  if ((vasprintf)(&buf, fmt, va) == -1) xdie();
  return buf;
}

static inline char *(xasprintf)(const char *fmt, ...) {
  char *res;
  va_list va;
  va_start(va, fmt);
  res = (xvasprintf)(fmt, va);
  va_end(va);
  return res;
}

dontdiscard testonly static inline char *testlib_formatstr(size_t cw, const void *s, int n) {
  if (s) {
    switch (cw) {
      case 1:
        if (n == -1) n = s ? strlen(s) : 0;
        return xasprintf("%`'.*s", n, s);
      case 2:
          abort();
          /*if (n == -1) n = s ? strlen16(s) : 0;
            return xasprintf("%`'.*hs", n, s);*/
      case 4:
        if (n == -1) n = s ? wcslen(s) : 0;
        return xasprintf("%`'.*ls", n, s);
      default:
        abort();
    }
  } else {
    return strdup("NULL");
  }
}

static inline void testlib_finish(void) {
  if (g_testlib_failed) {
      assert(fprintf(stderr, "%u / %u %s\n", g_testlib_failed, g_testlib_ran,
                     "tests failed") >= 0);
  }
}

static inline wontreturn void testlib_abort(void) {
  testlib_finish();
  _Exit(MIN(255, g_testlib_failed));
}

static inline void testlib_incrementfailed(void) {
  if (++g_testlib_failed > 23) {
      assert(fprintf(stderr, "too many failures, aborting\n") >= 0);
      testlib_abort();
  }
}

forceinline void testlib_onfail2(bool isfatal) {
  testlib_incrementfailed();
  if (isfatal) {
    testlib_abort();
  }
}

static inline void assertStringEquals(FILIFU_ARGS size_t cw, const void *want,
                                    const void *got, const char *gotcode,
                                    bool isfatal) {
  ++g_testlib_ran;
  if (testlib_strequals(cw, want, got)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStringEquals", "≠", gotcode,
                    testlib_formatstr(cw, want, -1),
                    testlib_formatstr(cw, got, -1));
  testlib_onfail2(isfatal);
}
