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
#include "test-lib/portable-symbols/xmalloc.h"
#include "test-lib/compiler-features.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#define TEST(SUITE, NAME) void SUITE ## _ ## NAME (void)

#define ASSERT_STREQ(WANT, GOT) \
  assertStringEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, true)

#define EXPECT_STREQ ASSERT_STREQ

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
#define kvprintf vprintf

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

#define EXPECT_EQ ASSERT_EQ

#define __TEST_EQ(KIND, FILE, LINE, FUNC, WANTCODE, GOTCODE, WANT, GOT, ...) \
  do {                                                                       \
    intptr_t Got, Want;                                                      \
    ++g_testlib_ran;                                                         \
    Got = (intptr_t)(GOT);                                                   \
    Want = (intptr_t)(WANT);                                                 \
    if (Want != Got) {                                                       \
      testlib_error_enter(FILE, FUNC);                                       \
      testlib_showerror_##KIND##_eq(LINE, WANTCODE, GOTCODE,                 \
                                    testlib_formatint(Want),                 \
                                    testlib_formatint(Got), "" __VA_ARGS__); \
      testlib_error_leave();                                                 \
    }                                                                        \
    (void)0;                                                                 \
  } while (0)

#define _spinlock(lock) _spinlock_cooperative(lock)

#define _spinlock_cooperative(lock)                  \
  ({                                                 \
    char __x;                                        \
    volatile int __i;                                \
    unsigned __tries = 0;                            \
    char *__lock = (lock);                           \
    for (;;) {                                       \
      __atomic_load(__lock, &__x, __ATOMIC_RELAXED); \
      if (!__x && !_trylock(__lock)) {               \
        break;                                       \
      } else if (__tries < 7) {                      \
        for (__i = 0; __i != 1 << __tries; __i++) {  \
        }                                            \
        __tries++;                                   \
      } else {                                       \
        _spinlock_yield();                           \
      }                                              \
    }                                                \
    0;                                               \
  })

#define _trylock(lock) __atomic_test_and_set(lock, __ATOMIC_SEQ_CST)

#define _spinlock_yield sched_yield

#if defined _WIN32 && !defined __CYGWIN__
#define IsWindows() 1
#else
#define IsWindows() 0
#endif

#define sys_getpid getpid

#define cosmopolitan_testlib_showerror_internal_pre_jump(a, b, c)    \
    do {                                                \
        testlib_showerror_symbol = (a);                 \
        testlib_showerror_macro = (b);                  \
        testlib_showerror_isfatal = (c);                \
    } while (false)

#define testlib_showerror_expect_eq(a, b, c, d, e, ...)              \
    do {                                                                \
        cosmopolitan_testlib_showerror_internal_pre_jump("=", "EXPECT_EQ", false);   \
        testlib_showerror_((a), (b), (c), (d), (e), __VA_ARGS__);  \
    } while (false)

#define testlib_showerror_assert_eq(a, b, c, d, e, ...)              \
    do {                                                                \
        cosmopolitan_testlib_showerror_internal_pre_jump("=", "ASSERT_EQ", true);   \
        testlib_showerror_((a), (b), (c), (d), (e), __VA_ARGS__);  \
    } while (false)

#define isempty(s) (!(s) || !(*(s)))

#define firstnonnull(a, b) ((!(a) && !(b) && (abort(), 1)), ((a) ? (a) : (b)))

#define ARRAYLEN(A) \
  ((sizeof(A) / sizeof(*(A))) / ((unsigned)!(sizeof(A) % sizeof(*(A)))))

#define _spunlock(lock) (__atomic_store_n(lock, 0, __ATOMIC_RELAXED), 0)

#define ASSERT_BINEQ(WANT, GOT)                                         \
    _Generic((WANT)[0], char                                            \
             : assertBinaryEquals_hex , default                         \
             : do_not_try_to_define_this_function_because_its_specifically_for_16bit_only_lmao) (FILIFU ((const char *)WANT), GOT, -1, #GOT, true)

#define EXPECT_BINEQ ASSERT_BINEQ

#ifndef nosideeffect
#if !defined(__STRICT_ANSI__) &&  \
    (__has_attribute(__pure__) || \
     (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 296)
#define nosideeffect __attribute__((__pure__))
#else
#define nosideeffect
#endif
#endif

#define ASSERT_EQ(WANT, GOT, ...)                                             \
  __TEST_EQ(assert, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 408 || \
    __has_attribute(__no_sanitize_address__)
#define noasan __attribute__((__no_sanitize_address__))
#else
#define noasan
#endif

static _Atomic(unsigned) g_testlib_ran;
static bool g_testlib_shoulddebugbreak;
static bool __nocolor;
static char g_fixturename[256];
static _Atomic(unsigned) g_testlib_failed;
static const bool __threaded = true;

/**
 * Function tracing enabled state.
 *
 * After ftrace_install() has been called, the logging of C function
 * calls may be controlled by changing this variable. If `__ftrace` is
 * greater than zero, functions are logged. Otherwise, they aren't.
 *
 * By convention, functions wishing to disable function tracing for a
 * short time period should say:
 *
 *     void foo() {
 *       --__ftrace;
 *       bar();
 *       ++__ftrace;
 *     }
 *
 * This way you still have some flexibility to force function tracing,
 * by setting `__ftrace` to a higher number like `2` or `200`. Even
 * though under normal circumstances, `__ftrace` should only be either
 * zero or one.
 */
static _Atomic(int) __ftrace;

/**
 * System call logging enabled state.
 *
 * If Cosmopolitan was compiled with the `SYSDEBUG` macro (this is the
 * default behavior, except in tiny and release modes) then `__strace`
 * shall control whether or not system calls are logged to fd 2. If it's
 * greater than zero, syscalls are logged. Otherwise, they're aren't.
 *
 * By convention, functions wishing to disable syscall tracing for a
 * short time period should say:
 *
 *     void foo() {
 *       --__strace;
 *       bar();
 *       ++__strace;
 *     }
 *
 * This way you still have some flexibility to force syscall tracing, by
 * setting `__strace` to a higher number like `2` or `200`. Even though
 * under normal circumstances, `__strace` should only be either zero or
 * one.
 */
static _Atomic(int) __strace;
static int __vforked;
static char cosmopolitan_testrunner_internal_testlib_error_lock;
static const char *testlib_showerror_file;  /* set by macros */
static const char *testlib_showerror_func;  /* set by macros */
static const char *testlib_showerror_symbol;
static const char *testlib_showerror_macro;
static bool testlib_showerror_isfatal;

static inline bool testlib_hexequals(const char *, const void *, size_t) nosideeffect;

static inline testonly void testlib_showerror(const char *file, int line, const char *func,
                                const char *method, const char *symbol,
                                const char *code, char *v1, char *v2) {
  char *p;
  char hostname[128];
  stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  kprintf("%serror%s%s:%s:%ld%s: %s() in %s(%s) on %s pid %d "
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

static inline void testlib_error_enter(const char *file, const char *func) {
  atomic_fetch_sub_explicit(&__ftrace, 1, memory_order_relaxed);
  atomic_fetch_sub_explicit(&__strace, 1, memory_order_relaxed);
  if (!__vforked) _spinlock(&cosmopolitan_testrunner_internal_testlib_error_lock);
  if (!IsWindows()) { pid_t deliberately_ignore = sys_getpid(); (void)deliberately_ignore; } /* make strace easier to read */
  if (!IsWindows()) { pid_t deliberately_ignore = sys_getpid(); (void)deliberately_ignore; }
  if (g_testlib_shoulddebugbreak) {
    DebugBreak();
  }
  testlib_showerror_file = file;
  testlib_showerror_func = func;
}

/* TODO(jart): Pay off tech debt re duplication */
static inline testonly void testlib_showerror_(int line, const char *wantcode,
                                 const char *gotcode, char *FREED_want,
                                 char *FREED_got, const char *fmt, ...) {
  int e;
  va_list va;
  char hostname[128];
  e = errno;
  if (gethostname(hostname, sizeof(hostname))) {
    stpcpy(hostname, "unknown");
  }
  kprintf("%serror%s:%s%s:%d%s: %s(%s) on %s pid %d "
#ifdef YALIBCT_LIBC_HAS_GETTID
          "tid %d\n"
#endif
          "\t%s(%s, %s)\n",
          RED2, UNBOLD, BLUE1, testlib_showerror_file, line, RESET,
          testlib_showerror_func, g_fixturename, hostname, getpid(),
#ifdef YALIBCT_LIBC_HAS_GETTID
          gettid(),
#endif
          testlib_showerror_macro, wantcode ? wantcode : "(null)", gotcode);
  if (wantcode) {
    kprintf("\t\tneed %s %s\n"
            "\t\t got %s\n",
            FREED_want, testlib_showerror_symbol, FREED_got);
  } else {
    kprintf("\t\t→ %s%s\n", testlib_showerror_symbol, FREED_want);
  }
  if (!isempty(fmt)) {
    kprintf("\t");
    va_start(va, fmt);
    kvprintf(fmt, va);
    va_end(va);
    kprintf("\n");
  }
  kprintf("\t%s%s%s\n"
          "\t%s%s @ %s%s\n",
          SUBTLE, strerror(e), RESET, SUBTLE,
          firstnonnull(program_invocation_name, "unknown"), hostname, RESET);
  //free_s(&FREED_want);
  //free_s(&FREED_got);
  ++g_testlib_failed;
  if (testlib_showerror_isfatal) {
    testlib_abort();
  }
}

static size_t cosmopolitan_libc_testlib_formatint_internal_sbufi_;
static char cosmopolitan_libc_testlib_formatint_internal_sbufs_[2][256];

static inline dontdiscard testonly char *testlib_formatint(intptr_t x) {
  char *str = cosmopolitan_libc_testlib_formatint_internal_sbufi_ < ARRAYLEN(cosmopolitan_libc_testlib_formatint_internal_sbufs_) ? cosmopolitan_libc_testlib_formatint_internal_sbufs_[cosmopolitan_libc_testlib_formatint_internal_sbufi_++] : malloc(256);
  char *p = str;
  p += sprintf(p, "%ld\t(or %#lx", x, x);
  if (0 <= x && x < 256) {
    p += sprintf(p, " or %c", (unsigned char)x);
  }
  *p++ = ')';
  *p++ = '\0';
  return str;
}

static inline void testlib_error_leave(void) {
  atomic_fetch_add_explicit(&__ftrace, 1, memory_order_relaxed);
  atomic_fetch_add_explicit(&__strace, 1, memory_order_relaxed);
  _spunlock(&cosmopolitan_testrunner_internal_testlib_error_lock);
}

static inline testonly void testlib_formatbinaryashex(const char *want, const void *got,
                                        size_t n, char **out_v1,
                                        char **out_v2) {
  size_t i;
  uint8_t b;
  char *gothex;
  if (n == -1ul) n = strlen(want) / 2;
  gothex = xmalloc(n * 2 + 1);
  gothex[n * 2] = '\0';
  for (i = 0; i < n; ++i) {
    b = ((uint8_t *)got)[i];
    gothex[i * 2 + 0] = "0123456789abcdef"[(b >> 4) & 0xf];
    gothex[i * 2 + 1] = "0123456789abcdef"[(b >> 0) & 0xf];
  }
  *out_v1 = strdup(want);
  *out_v2 = gothex;
}

/**
 * Converts ASCII hexadecimal character to integer case-insensitively.
 * @return integer or 0 if c ∉ [0-9A-Fa-f]
 */
static inline int hextoint(int c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  } else if ('a' <= c && c <= 'f') {
    return c - 'a' + 10;
  } else if ('A' <= c && c <= 'F') {
    return c - 'A' + 10;
  } else {
    return 0;
  }
}

/**
 * Tests that raw memory is equal to numeric representation, e.g.
 *
 *   testlib_hexequals("00010203", "\0\1\2\3", -1ul);
 *
 * @see unhexstr()
 */
static inline testonly bool testlib_hexequals(const char *want, const void *got, size_t n) {
  size_t i;
  const unsigned char *p = (const unsigned char *)got;
  if (!got) return false;
  for (i = 0; i < n; ++i) {
    if (!want[i * 2]) break;
    //if (i == n) break; // Note: cppcheck warns me about this and the warning looks good to me
    if (p[i] != (unsigned char)(hextoint(want[i * 2 + 0]) * 16 +
                                hextoint(want[i * 2 + 1]))) {
      return false;
    }
  }
  return true;
}

forceinline void assertBinaryEquals_hex(FILIFU_ARGS const char *want,
                                        const void *got, size_t n,
                                        const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  char *v1, *v2;
  if (testlib_hexequals(want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_formatbinaryashex(want, got, n, &v1, &v2);
  testlib_showerror(file, line, func, "assertBinaryEquals", "≠", gotcode, v1,
                    v2);
  testlib_onfail2(isfatal);
}

/**
 * Fills memory with random bytes, e.g.
 *
 *     char buf[512];
 *     rngset(buf, sizeof(buf), 0, 0);
 *
 * If reseed is zero then the internal PRNG is disabled and bytes are
 * simply copied in little-endian order from the seed function. If seed
 * is NULL then the reseed parameter is used as the seed value for the
 * internal PRNG. If seed!=NULL and reseed>8 then reseed is the number
 * of bytes after which the seed() function should be called again, to
 * freshen up the PRNG.
 *
 * The main advantage of this generator is that it produces data at 13
 * gigabytes per second since Vigna's Algorithm vectorizes better than
 * alternatives, going even faster than xorshift.
 *
 * @return original buf
 */
static inline void *rngset(void *b, size_t n, uint64_t seed(void), size_t reseed) {
  size_t m;
  uint64_t i, x, t = 0;
  unsigned char *p = b;
  /*if (IsAsan()) {
    __asan_verify(b, n);
  }*/
  if (!seed) {
    t = reseed;
    reseed = -1;
  } else if (reseed < 8) {
    reseed = 8;
  }
  while (n) {
    if (seed) t = seed();
    if (!seed || reseed > 8) {
      n -= (m = reseed < n ? reseed : n);
      while (m >= 8) {
        x = (t += 0x9e3779b97f4a7c15);
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        x = (x ^ (x >> 31));
        __builtin_memcpy(p, &x, 8);
        p += 8;
        m -= 8;
      }
      while (m--) {
        *p++ = t;
        t >>= 8;
      }
    } else if (n >= 8) {
      p[0] = (0x00000000000000FF & t) >> 000;
      p[1] = (0x000000000000FF00 & t) >> 010;
      p[2] = (0x0000000000FF0000 & t) >> 020;
      p[3] = (0x00000000FF000000 & t) >> 030;
      p[4] = (0x000000FF00000000 & t) >> 040;
      p[5] = (0x0000FF0000000000 & t) >> 050;
      p[6] = (0x00FF000000000000 & t) >> 060;
      p[7] = (0xFF00000000000000 & t) >> 070;
      p += 8;
      n -= 8;
    } else {
      while (n) {
        *p++ = t;
        t >>= 8;
        --n;
      }
    }
  }
  return b;
}

static struct {
  int thepid;
  uint64_t thepool;
  pthread_mutex_t lock;
} cosmopolitan_internal_rand64_g_rand64;

/**
 * Returns nondeterministic random data.
 *
 * This function is similar to lemur64() except that it doesn't produce
 * the same sequences of numbers each time your program is run. This is
 * the case even across forks and threads, whose sequences will differ.
 *
 * @see rdseed(), rdrand(), rand(), random(), rngset()
 * @note this function takes 5 cycles (30 if `__threaded`)
 * @note this function is not intended for cryptography
 * @note this function passes bigcrush and practrand
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
uint64_t rand64(void) {
  void *p;
  uint64_t s = 0;
  if (__threaded) pthread_mutex_lock(&cosmopolitan_internal_rand64_g_rand64.lock);
  if (getpid() == cosmopolitan_internal_rand64_g_rand64.thepid) {
    s = cosmopolitan_internal_rand64_g_rand64.thepool;  // normal path
  } else {
    /*if (!cosmopolitan_internal_rand64_g_rand64.thepid) {
      if (AT_RANDOM && (p = (void *)_getauxval(AT_RANDOM).value)) {
        // linux / freebsd kernel supplied entropy
        memcpy(&s, p, 16);
      } else {
        // otherwise initialize w/ cheap timestamp
        s = kStartTsc;
      }
    } else {
      // blend another timestamp on fork contention
      s = cosmopolitan_internal_rand64_g_rand64.thepool ^ rdtsc();
    }*/
    // blend the pid on startup and fork contention
      s ^= getpid();
      cosmopolitan_internal_rand64_g_rand64.thepid = getpid();
  }
  cosmopolitan_internal_rand64_g_rand64.thepool = (s *= 15750249268501108917ull);  // lemur64
  if (__threaded) pthread_mutex_unlock(&cosmopolitan_internal_rand64_g_rand64.lock);
  return s;
}
