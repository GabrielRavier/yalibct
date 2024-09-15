// Derived from code with this license:
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

#include "test-lib/hedley.h"
#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/printf.h"
#include "test-lib/portable-symbols/wcsncmp.h"
#include "test-lib/portable-symbols/getprogname.h"
#include "test-lib/portable-symbols/gettid.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <wchar.h>
#include <string.h>
#include <sys/param.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

/**
 * Declares test case function.
 *
 * Test cases are guaranteed by the linker to be run in order, sorted by
 * the (SUITE, NAME) tuple passed here.
 */
#define TEST(SUITE, NAME)           \
/*  STATIC_YOINK("__testcase_start");            */ \
  __TEST_PROTOTYPE(SUITE, NAME, __TEST_ARRAY, )

#define __TEST_PROTOTYPE(S, N, A, K)               \
/*  void S##_##N(void);
    testfn_t S##_##N##_ptr[] A(S##_##N) = {S##_##N}; */ \
  K void S##_##N(void)

#define EXPECT_STREQ(WANT, GOT) \
  assertStringEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, false)

#undef forceinline
#define forceinline HEDLEY_ALWAYS_INLINE

#define FILIFU           __FILE__, __LINE__, __FUNCTION__,
#define FILIFU_OBJ(...)  __FILE__, __LINE__, ##__VA_ARGS__
#define FILIFU_ARGS      const char *file, int line, const char *func,
#define FILIFU_FROM(OBJ) (OBJ)->file, (OBJ)->line, __FUNCTION__,
#define FILIFU_FIELDS \
  const char *file;   \
  int line

#define atomic_uint           _Atomic(unsigned int)
atomic_uint g_testlib_ran;

bool testlib_strnequals(size_t cw, const void *s1, const void *s2, size_t n) {
  if (s1 == s2) return true;
  if (!s1 || !s2) return false;
  return (cw == sizeof(wchar_t)    ? wcsncmp(s1, s2, n)
//          : cw == sizeof(char16_t) ? strncmp16(s1, s2, n)
                                   : strncmp(s1, s2, n)) == 0;
}

bool testlib_strequals(size_t cw, const void *s1, const void *s2) {
  return testlib_strnequals(cw, s1, s2, LONG_MAX);
}

#define CLS    (!__nocolor ? "\r\e[J" : "")
#define RED    (!__nocolor ? "\e[30;101m" : "")
#define GREEN  (!__nocolor ? "\e[32m" : "")
#define UNBOLD (!__nocolor ? "\e[22m" : "")
#define RED2   (!__nocolor ? "\e[91;1m" : "")
#define BLUE1  (!__nocolor ? "\e[94;49m" : "")
#define BLUE2  (!__nocolor ? "\e[34m" : "")
#define RESET  (!__nocolor ? "\e[0m" : "")
#define SUBTLE (!__nocolor ? "\e[35m" : "")

/**
 * Indicates if ANSI terminal colors are inappropriate.
 *
 * Normally this variable should be false. We only set it to true if
 * we're running on an old version of Windows or the environment
 * variable `TERM` is set to `dumb`.
 *
 * We think colors should be the norm, since most software is usually
 * too conservative about removing them. Rather than using `isatty`
 * consider using sed for instances where color must be removed:
 *
 *      sed 's/\x1b\[[;[:digit:]]*m//g' <color.txt >uncolor.txt
 *
 * For some reason, important software is configured by default in many
 * operating systems, to not only disable colors, but utf-8 too! Here's
 * an example of how a wrapper script can fix that for `less`.
 *
 *      #!/bin/sh
 *      LESSCHARSET=UTF-8 exec /usr/bin/less -RS "$@"
 *
 * Thank you for using colors!
 */
bool __nocolor;
char g_fixturename[256];

#define GetProgramExecutableName getprogname

void testlib_showerror(const char *file, int line, const char *func,
                       const char *method, const char *symbol, const char *code,
                       char *v1, char *v2) {
  char *p;
  char hostname[128];
  stpcpy(hostname, "unknown");
  gethostname(hostname, sizeof(hostname));
  printf("%serror%s%s:%s:%d%s: %s() in %s(%s) on %s pid %jd tid %jd\n"
          "\t%s\n"
          "\t\tneed %s %s\n"
          "\t\t got %s\n"
          "\t%s%s\n"
          "\t%s%s\n",
          RED2, UNBOLD, BLUE1, file, line, RESET, method, func,
	 g_fixturename, hostname, (intmax_t)getpid(), (intmax_t)gettid(), code, v1, symbol, v2,
          SUBTLE, strerror(errno), GetProgramExecutableName(), RESET);
  //free(v1);
  //free(v2);
}

#undef dontdiscard
#define dontdiscard HEDLEY_WARN_UNUSED_RESULT

#define _weaken(symbol)      symbol

#undef relegated
#define relegated YALIBCT_ATTRIBUTE_COLD
#undef wontreturn
#define wontreturn HEDLEY_NO_RETURN

#define _Exitr _Exit

/**
 * Aborts process after printing a backtrace.
 *
 * If a debugger is present then this will trigger a breakpoint.
 */
relegated wontreturn void __die(void) {
    abort();
  /* asan runtime depends on this function */
  /*int me, owner;
  static atomic_int once;
  owner = 0;
  me = sys_gettid();
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
  if (__vforked ||
      atomic_compare_exchange_strong_explicit(
          &once, &owner, me, memory_order_relaxed, memory_order_relaxed)) {
    __restore_tty();
    if (IsDebuggerPresent(false)) {
      DebugBreak();
    }
    ShowBacktrace(2, __builtin_frame_address(0));
    _Exitr(77);
  } else if (owner == me) {
    kprintf("die failed while dying\n");
    _Exitr(78);
  } else {
    _Exit1(79);
  }*/
}

void xdie(void) {
  /* if (_weaken(__die)) */ __die();
  abort();
}

/**
 * Returns dynamically formatted string.
 *
 * @return fully formatted string, which must be free()'d
 * @see xasprintf()
 */
char *(xvasprintf)(const char *fmt, va_list va) {
  char *buf;
  if ((vasprintf)(&buf, fmt, va) == -1) xdie();
  return buf;
}


/**
 * Returns dynamically formatted string.
 *
 * @return must be free()'d or gc()'d
 * @note greatest of all C functions
 */
char *(xasprintf)(const char *fmt, ...) {
  char *res;
  va_list va;
  va_start(va, fmt);
  res = (xvasprintf)(fmt, va);
  va_end(va);
  return res;
}


/**
 * Turns string into code.
 */
dontdiscard char *testlib_formatstr(size_t cw, const void *s, int n) {
  if (s) {
    switch (cw) {
      case 1:
        if (n == -1) n = s ? strlen(s) : 0;
        return xasprintf("%.*s", n, s);
      case 4:
        if (n == -1) n = s ? wcslen(s) : 0;
        return xasprintf("%.*ls", n, s);
      default:
        abort();
    }
  } else {
    return strdup("NULL");
  }
}

atomic_uint g_testlib_failed;

void testlib_finish(void) {
  if (g_testlib_failed) {
      assert(fprintf(stderr, "%u / %u %s\n", g_testlib_failed, g_testlib_ran,
                     "tests failed") >= 0);
  }
}

wontreturn void testlib_abort(void) {
  testlib_finish();
  abort(); //_Exitr(MAX(1, MIN(255, g_testlib_failed)));
}

void testlib_incrementfailed(void) {
  if (++g_testlib_failed > 23) {
      assert(fprintf(stderr, "too many failures, aborting\n") == 28);
    testlib_abort();
  }
}

forceinline void testlib_onfail2(bool isfatal) {
    (void)isfatal;
  testlib_incrementfailed();
  if (true) { //if (isfatal) {
    testlib_abort();
  }
}

forceinline void assertStringEquals(FILIFU_ARGS size_t cw, const void *want,
                                    const void *got, const char *gotcode,
                                    bool isfatal) {
  ++g_testlib_ran;
  if (testlib_strequals(cw, want, got)) return;
//  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStringEquals", "≠", gotcode,
                    testlib_formatstr(cw, want, -1),
                    testlib_formatstr(cw, got, -1));
  testlib_onfail2(isfatal);
}

uint64_t xorshift64(void)
{
    static uint64_t state_a = 1;
	uint64_t x = state_a;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return state_a = x;
}


/**
 * Returns linear congruential deterministic pseudorandom data, e.g.
 *
 *     uint64_t x = lemur64();
 *
 * You can generate different types of numbers as follows:
 *
 *     int64_t x = lemur64() >> 1;    // make positive signed integer
 *     double x = _real1(lemur64());  // make float on [0,1]-interval
 *
 * If you want a fast pseudorandom number generator that seeds itself
 * automatically on startup and fork(), then consider _rand64. If you
 * want true random data then consider rdseed, rdrand, and getrandom.
 *
 * @return 64 bits of pseudorandom data
 * @note this is Lemire's Lehmer generator
 * @note this function takes at minimum 1 cycle
 * @note this function passes bigcrush and practrand
 * @note this function is not intended for cryptography
 * @see _rand64(), rngset(), _real1(), _real2(), _real3()
 */
uint64_t lemur64(void) {
    /*static uint128_t s = 2131259787901769494;
      return (s *= 15750249268501108917ull) >> 64;*/
    return xorshift64();
}

#define READ64LE(S)                                                    \
  ((uint64_t)(255 & (S)[7]) << 070 | (uint64_t)(255 & (S)[6]) << 060 | \
   (uint64_t)(255 & (S)[5]) << 050 | (uint64_t)(255 & (S)[4]) << 040 | \
   (uint64_t)(255 & (S)[3]) << 030 | (uint64_t)(255 & (S)[2]) << 020 | \
   (uint64_t)(255 & (S)[1]) << 010 | (uint64_t)(255 & (S)[0]) << 000)

#undef noasan
#define noasan

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
noasan void *rngset(void *b, size_t n, uint64_t seed(void), size_t reseed) {
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

#define ARRAYLEN(A) \
  ((sizeof(A) / sizeof(*(A))) / ((unsigned)!(sizeof(A) % sizeof(*(A)))))

#define EXPECT_EQ(WANT, GOT, ...)                                             \
  __TEST_EQ(expect, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

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

#define sys_getpid getpid

const char *testlib_showerror_errno;
const char *testlib_showerror_file;
const char *testlib_showerror_func;
bool testlib_showerror_isfatal;
const char *testlib_showerror_macro;
const char *testlib_showerror_symbol;

void testlib_error_enter(const char *file, const char *func) {
  /*ftrace_enabled(-1);
  strace_enabled(-1);
  pthread_mutex_lock(&testlib_error_lock);*/
  /* if (!IsWindows()) */ { pid_t deliberately_ignore = sys_getpid(); (void)deliberately_ignore; } /* make strace easier to read */
  /* if (!IsWindows()) */ { pid_t deliberately_ignore = sys_getpid(); (void)deliberately_ignore; }
  /*if (g_testlib_shoulddebugbreak) {
    DebugBreak();
  }*/
  testlib_showerror_file = file;
  testlib_showerror_func = func;
}

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

#if __cplusplus + 0 >= 201103L
#define autotype(x) auto
#elif ((__has_builtin(auto_type) || defined(__llvm__) ||       \
        (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 409) && \
       !defined(__chibicc__))
#define autotype(x) __auto_type
#else
#define autotype(x) typeof(x)
#endif

#define isempty(s)              \
  ({                            \
    autotype(s) IsEmptyS = (s); \
    !IsEmptyS || !(*IsEmptyS);  \
  })

#define firstnonnull(a, b)                         \
  ({                                               \
    autotype(a) FirstNonNullA = (a);               \
    autotype(a) FirstNonNullB = (b);               \
    if (!FirstNonNullA && !FirstNonNullB) abort(); \
    FirstNonNullA ? FirstNonNullA : FirstNonNullB; \
  })


/* TODO(jart): Pay off tech debt re duplication */
void testlib_showerror_(int line, const char *wantcode, const char *gotcode,
                        char *FREED_want, char *FREED_got, const char *fmt,
                        ...) {
  int e;
  va_list va;
  char hostname[128];
  e = errno;
  if (gethostname(hostname, sizeof(hostname))) {
    stpcpy(hostname, "unknown");
  }
  printf("%serror%s:%s%s:%d%s: %s(%s) on %s pid %jd tid %jd\n"
          "\t%s(%s, %s)\n",
          RED2, UNBOLD, BLUE1, testlib_showerror_file, line, RESET,
	 testlib_showerror_func, g_fixturename, hostname, (intmax_t)getpid(), (intmax_t)gettid(),
          testlib_showerror_macro, wantcode != NULL ? wantcode : "(null)", gotcode);
  if (wantcode) {
    printf("\t\tneed %s %s\n"
            "\t\t got %s\n",
            FREED_want, testlib_showerror_symbol, FREED_got);
  } else {
    printf("\t\t→ %s%s\n", testlib_showerror_symbol, FREED_want);
  }
  if (!isempty(fmt)) {
    printf("\t");
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
    printf("\n");
  }
  printf("\t%s%s%s\n"
          "\t%s%s @ %s%s\n",
          SUBTLE, strerror(e), RESET, SUBTLE,
          firstnonnull(getprogname(), "unknown"), hostname, RESET);
  //free(FREED_want);
  //free(FREED_got);
  ++g_testlib_failed;
  if (true) { //if (testlib_showerror_isfatal) {
    testlib_abort();
  }
}

static size_t sbufi_;
static char sbufs_[2][256];

dontdiscard char *testlib_formatint(intptr_t x) {
  char *str = sbufi_ < ARRAYLEN(sbufs_) ? sbufs_[sbufi_++] : malloc(256);
  char *p = str;
  p += sprintf(p, "%ld\t(or %#lx", x, x);
  if (0 <= x && x < 256) {
      //p += sprintf(p, " or %#`c", (unsigned char)x);
  }
  *p++ = ')';
  *p++ = '\0';
  return str;
}

void testlib_error_leave(void) {
}

#define ASSERT_EQ(WANT, GOT, ...)                                             \
  __TEST_EQ(assert, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

relegated void __oom_hook(size_t request) {
    (void)request;
  int e;
  uint64_t toto, newlim;
  //__restore_tty();
  e = errno;
  //toto = CountMappedBytes();
  printf("\n\nWE REQUIRE MORE VESPENE GAS");
  if (e != ENOMEM) printf(" (%s)", strerror(e));
  /*if (IsRunningUnderMake()) {
    newlim = toto + request;
    newlim += newlim >> 1;
    newlim = _roundup2pow(newlim);
    kprintf("FIX CODE OR TUNE QUOTA += -M%dm\n", newlim / (1024 * 1024));
  }*/
  printf("\n");
  // __print_maps();
  printf("\nTHE STRAW THAT BROKE THE CAMEL'S BACK\n");
  //PrintBacktraceUsingSymbols(2, 0, GetSymbolTable());
  //PrintSystemMappings(2);
  abort(); //_Exitr(42);
}

/**
 * Helper function for allocating anonymous mapping.
 *
 * This function is equivalent to:
 *
 *     mmap(NULL, mapsize, PROT_READ | PROT_WRITE,
 *          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
 *
 * If mmap() fails, possibly because the parent process did this:
 *
 *     if (!vfork()) {
 *       setrlimit(RLIMIT_AS, &(struct rlimit){maxbytes, maxbytes});
 *       execv(prog, (char *const[]){prog, 0});
 *     }
 *     wait(0);
 *
 * Then this function will call:
 *
 *     __oom_hook(size);
 *
 * If it's linked. The LIBC_TESTLIB library provides an implementation,
 * which can be installed as follows:
 *
 *     int main() {
 *         InstallQuotaHandlers();
 *         // ...
 *     }
 *
 * That is performed automatically for unit test executables.
 *
 * @return memory map address on success, or null w/ errno
 */
void *_mapanon(size_t size) {
  void *m;
  m = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (m != MAP_FAILED) {
    return m;
  }
  if (errno == ENOMEM /* && _weaken(__oom_hook) */) {
    _weaken(__oom_hook)(size);
  }
  return 0;
}

enum {
    FRAMESIZE = 0x10000, /* 8086 */
};

#define EXPECT_SYS(ERRNO, WANT, GOT, ...)                                  \
  do {                                                                     \
    testlib_seterrno(0);                                                   \
    __TEST_EQ(expect, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, \
              GOT, __VA_ARGS__);                                           \
    __TEST_EQ(expect, __FILE__, __LINE__, __FUNCTION__, #ERRNO,            \
              testlib_strerror(), ERRNO, testlib_geterrno(), __VA_ARGS__); \
  } while (0)

void testlib_seterrno(int x) {
  errno = x;
}

int testlib_geterrno(void) {
  return errno;
}

const char *testlib_strerror(void) {
  return strerror(errno);
}

#define ASSERT_STREQ(WANT, GOT) \
  assertStringEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, true)

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
uint64_t _rand64(void) {
    return xorshift64();
  /*void *p;
  uint128_t s;
  if (__threaded) pthread_spin_lock(&g_rand64.lock);
  if (__pid == g_rand64.thepid) {
    s = g_rand64.thepool;  // normal path
  } else {
    if (!g_rand64.thepid) {
      if (AT_RANDOM && (p = (void *)_getauxval(AT_RANDOM).value)) {
        // linux / freebsd kernel supplied entropy
        memcpy(&s, p, 16);
      } else {
        // otherwise initialize w/ cheap timestamp
        s = kStartTsc;
      }
    } else {
      // blend another timestamp on fork contention
      s = g_rand64.thepool ^ rdtsc();
    }
    // blend the pid on startup and fork contention
    s ^= __pid;
    g_rand64.thepid = __pid;
  }
  g_rand64.thepool = (s *= 15750249268501108917ull);  // lemur64
  pthread_spin_unlock(&g_rand64.lock);
  return s >> 64;*/
}

#define EXPECT_NE(WANT, GOT, ...)                                             \
  __TEST_NE(expect, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

#define __TEST_NE(KIND, FILE, LINE, FUNC, WANTCODE, GOTCODE, WANT, GOT, ...) \
  do {                                                                       \
    intptr_t Got, Want;                                                      \
    ++g_testlib_ran;                                                         \
    Got = (intptr_t)(GOT);                                                   \
    Want = (intptr_t)(WANT);                                                 \
    if (Want == Got) {                                                       \
      testlib_error_enter(FILE, FUNC);                                       \
      testlib_showerror_##KIND##_ne(LINE, WANTCODE, GOTCODE,                 \
                                    testlib_formatint(Want),                 \
                                    testlib_formatint(Got), "" __VA_ARGS__); \
      testlib_error_leave();                                                 \
    }                                                                        \
  } while (0)

#define testlib_showerror_expect_ne(a, b, c, d, e, ...)              \
    do {                                                                \
        cosmopolitan_testlib_showerror_internal_pre_jump("≠", "EXPECT_NE", false);   \
        testlib_showerror_((a), (b), (c), (d), (e), __VA_ARGS__);  \
    } while (false)

#define ASSERT_NE(WANT, GOT, ...)                                             \
  __TEST_NE(assert, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

#define testlib_showerror_assert_ne(a, b, c, d, e, ...)              \
    do {                                                                \
        cosmopolitan_testlib_showerror_internal_pre_jump("≠", "ASSERT_NE", false);   \
        testlib_showerror_((a), (b), (c), (d), (e), __VA_ARGS__);  \
    } while (false)

#define ASSERT_GE(C, X) _TEST2("ASSERT_GE", C, >=, (X), #C, " ≥ ", #X, 1)
#define ASSERT_GT(C, X) _TEST2("ASSERT_GT", C, >, (X), #C, " > ", #X, 1)
#define ASSERT_LE(C, X) _TEST2("ASSERT_LE", C, <=, (X), #C, " ≤ ", #X, 1)
#define ASSERT_LT(C, X) _TEST2("ASSERT_LT", C, <, (X), #C, " < ", #X, 1)
#define EXPECT_GE(C, X) _TEST2("EXPECT_GE", C, >=, (X), #C, " ≥ ", #X, 0)
#define EXPECT_GT(C, X) _TEST2("EXPECT_GT", C, >, (X), #C, " > ", #X, 0)
#define EXPECT_LE(C, X) _TEST2("EXPECT_LE", C, <=, (X), #C, " ≤ ", #X, 0)
#define EXPECT_LT(C, X) _TEST2("EXPECT_LT", C, <, (X), #C, " < ", #X, 0)

#undef PAGESIZE
#define PAGESIZE    0x1000  /* i386+ */

#define _TEST2(NAME, WANT, OP, GOT, WANTCODE, OPSTR, GOTCODE, ISFATAL)    \
  do {                                                                    \
    intptr_t Want = (intptr_t)(WANT);                                     \
    intptr_t Got = (intptr_t)(GOT);                                       \
    if (!(Want OP Got)) {                                                 \
      testlib_showerror(FILIFU NAME, OPSTR, WANTCODE OPSTR GOTCODE,       \
                        testlib_formatint(Want), testlib_formatint(Got)); \
      testlib_onfail2(ISFATAL);                                           \
    }                                                                     \
  } while (0)

#define ASSERT_SYS(ERRNO, WANT, GOT, ...)                                  \
  do {                                                                     \
    int e = testlib_geterrno();                                            \
    __TEST_EQ(assert, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, \
              GOT, __VA_ARGS__);                                           \
    __TEST_EQ(assert, __FILE__, __LINE__, __FUNCTION__, #ERRNO,            \
              testlib_strerror(), ERRNO, testlib_geterrno(), __VA_ARGS__); \
    testlib_seterrno(e);                                                   \
  } while (0)
