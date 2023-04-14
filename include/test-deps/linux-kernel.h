// Derived directly from work with this license:
/* SPDX-License-Identifier: GPL-2.0+ */

#pragma once

#include "test-lib/portable-symbols/memchr_inv.h"
#include "test-lib/portable-symbols/vstrdupf.h"
#include "test-lib/portable-snippets-endian.h"
#include "test-lib/hedley.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#define KSTM_MODULE_GLOBALS()
#define __initdata
#define __printf HEDLEY_PRINTF_FORMAT
#define __init
#define pr_warn(fmt, ...) ((void)printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__))
#define pr_err(fmt, ...) ((void)printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__))
#define printk(...) fprintf(stderr, __VA_ARGS__)
#define KERN_WARNING        "warning: "
#define KERN_ERR        "error: "
#define KBUILD_MODNAME "test"

#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)

#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define min(x, y) (((x) < (y)) ? (x) : (y))

#define __force

enum {
    ___GFP_DMA = 0x01u,
    ___GFP_HIGHMEM = 0x02u,
    ___GFP_DMA32 = 0x04u,
    ___GFP_MOVABLE = 0x08u,
    ___GFP_RECLAIMABLE = 0x10u,
    ___GFP_HIGH = 0x20u,
    ___GFP_IO = 0x40u,
    ___GFP_FS = 0x80u,
    ___GFP_ZERO = 0x100u,
    ___GFP_ATOMIC = 0x200u,
    ___GFP_DIRECT_RECLAIM = 0x400u,
    ___GFP_KSWAPD_RECLAIM = 0x800u,
    ___GFP_WRITE = 0x1000u,
    ___GFP_NOWARN = 0x2000u,
    ___GFP_RETRY_MAYFAIL = 0x4000u,
    ___GFP_NOFAIL = 0x8000u,
    ___GFP_NORETRY = 0x10000u,
    ___GFP_MEMALLOC = 0x20000u,
    ___GFP_COMP = 0x40000u,
    ___GFP_NOMEMALLOC = 0x80000u,
    ___GFP_HARDWALL = 0x100000u,
    ___GFP_THISNODE = 0x200000u,
    ___GFP_ACCOUNT = 0x400000u,
    ___GFP_ZEROTAGS = 0x800000u,
};

#define __GFP_DMA       ((__force gfp_t)___GFP_DMA)
#define __GFP_HIGHMEM   ((__force gfp_t)___GFP_HIGHMEM)
#define __GFP_DMA32     ((__force gfp_t)___GFP_DMA32)
#define __GFP_MOVABLE   ((__force gfp_t)___GFP_MOVABLE)  /* ZONE_MOVABLE allowed */
#define GFP_ZONEMASK    (__GFP_DMA|__GFP_HIGHMEM|__GFP_DMA32|__GFP_MOVABLE)

#define __GFP_RECLAIMABLE ((__force gfp_t)___GFP_RECLAIMABLE)
#define __GFP_WRITE     ((__force gfp_t)___GFP_WRITE)
#define __GFP_HARDWALL   ((__force gfp_t)___GFP_HARDWALL)
#define __GFP_THISNODE  ((__force gfp_t)___GFP_THISNODE)
#define __GFP_ACCOUNT   ((__force gfp_t)___GFP_ACCOUNT)

#define __GFP_ATOMIC    ((__force gfp_t)___GFP_ATOMIC)
#define __GFP_HIGH      ((__force gfp_t)___GFP_HIGH)
#define __GFP_MEMALLOC  ((__force gfp_t)___GFP_MEMALLOC)
#define __GFP_NOMEMALLOC ((__force gfp_t)___GFP_NOMEMALLOC)

#define __GFP_IO        ((__force gfp_t)___GFP_IO)
#define __GFP_FS        ((__force gfp_t)___GFP_FS)
#define __GFP_DIRECT_RECLAIM    ((__force gfp_t)___GFP_DIRECT_RECLAIM) /* Caller can reclaim */
#define __GFP_KSWAPD_RECLAIM    ((__force gfp_t)___GFP_KSWAPD_RECLAIM) /* kswapd can wake */
#define __GFP_RECLAIM ((__force gfp_t)(___GFP_DIRECT_RECLAIM|___GFP_KSWAPD_RECLAIM))
#define __GFP_RETRY_MAYFAIL     ((__force gfp_t)___GFP_RETRY_MAYFAIL)
#define __GFP_NOFAIL    ((__force gfp_t)___GFP_NOFAIL)
#define __GFP_NORETRY   ((__force gfp_t)___GFP_NORETRY)

#define __GFP_NOWARN    ((__force gfp_t)___GFP_NOWARN)
#define __GFP_COMP      ((__force gfp_t)___GFP_COMP)
#define __GFP_ZERO      ((__force gfp_t)___GFP_ZERO)
#define __GFP_ZEROTAGS  ((__force gfp_t)___GFP_ZEROTAGS)
#define __GFP_SKIP_ZERO ((__force gfp_t)___GFP_SKIP_ZERO)
#define __GFP_SKIP_KASAN_UNPOISON ((__force gfp_t)___GFP_SKIP_KASAN_UNPOISON)
#define __GFP_SKIP_KASAN_POISON   ((__force gfp_t)___GFP_SKIP_KASAN_POISON)

/* Disable lockdep for GFP context tracking */
#define __GFP_NOLOCKDEP ((__force gfp_t)___GFP_NOLOCKDEP)

/* Room for N __GFP_FOO bits */
#define __GFP_BITS_SHIFT (27 + IS_ENABLED(CONFIG_LOCKDEP))
#define __GFP_BITS_MASK ((__force gfp_t)((1 << __GFP_BITS_SHIFT) - 1))

#define GFP_ATOMIC      (__GFP_HIGH|__GFP_ATOMIC|__GFP_KSWAPD_RECLAIM)
#define GFP_KERNEL      (__GFP_RECLAIM | __GFP_IO | __GFP_FS)
#define GFP_KERNEL_ACCOUNT (GFP_KERNEL | __GFP_ACCOUNT)
#define GFP_NOWAIT      (__GFP_KSWAPD_RECLAIM)
#define GFP_NOIO        (__GFP_RECLAIM)
#define GFP_NOFS        (__GFP_RECLAIM | __GFP_IO)
#define GFP_USER        (__GFP_RECLAIM | __GFP_IO | __GFP_FS | __GFP_HARDWALL)
#define GFP_DMA         __GFP_DMA
#define GFP_DMA32       __GFP_DMA32
#define GFP_HIGHUSER    (GFP_USER | __GFP_HIGHMEM)
#define GFP_HIGHUSER_MOVABLE    (GFP_HIGHUSER | __GFP_MOVABLE | \
                         __GFP_SKIP_KASAN_POISON | __GFP_SKIP_KASAN_UNPOISON)
#define GFP_TRANSHUGE_LIGHT     ((GFP_HIGHUSER_MOVABLE | __GFP_COMP | \
                         __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM)
#define GFP_TRANSHUGE   (GFP_TRANSHUGE_LIGHT | __GFP_DIRECT_RECLAIM)

#define kfree free

#define __diag_push() HEDLEY_DIAGNOSTIC_PUSH
#define __diag_ignore_all(option, comment) YALIBCT_DIAGNOSTIC_IGNORE(option)
#define __diag_pop() HEDLEY_DIAGNOSTIC_POP

#define __must_check HEDLEY_WARN_UNUSED_RESULT

#define cpu_to_be16 psnip_endian_be16
#define cpu_to_be32 psnip_endian_be32

#ifndef __noreturn
#define __noreturn HEDLEY_NO_RETURN
#endif

/*
 * Optional: only supported since clang >= 14.0
 *
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-error-function-attribute
 */

#define __compiletime_error YALIBCT_ATTRIBUTE_ERROR


#ifdef __OPTIMIZE__
# define __compiletime_assert(condition, msg, prefix, suffix)           \
        do {                                                            \
                /*                                                      \
                 * __noreturn is needed to give the compiler enough     \
                 * information to avoid certain possibly-uninitialized  \
                 * warnings (regardless of the build failing).          \
                 */                                                     \
                __noreturn extern void prefix ## suffix(void)           \
                        __compiletime_error(msg);                       \
                if (!(condition))                                       \
                        prefix ## suffix();                             \
        } while (0)
#else
# define __compiletime_assert(condition, msg, prefix, suffix) do { } while (0)
#endif

#define _compiletime_assert(condition, msg, prefix, suffix) \
        __compiletime_assert(condition, msg, prefix, suffix)

/**
 * compiletime_assert - break build and emit msg if condition is false
 * @condition: a compile-time constant condition to check
 * @msg:       a message to emit if condition is false
 *
 * In tradition of POSIX assert, this macro will break the build if the
 * supplied condition is *false*, emitting the supplied error message if the
 * compiler has support to do so.
 */
#define compiletime_assert(condition, msg) \
        _compiletime_assert(condition, msg, __compiletime_assert_, __COUNTER__)


/**
 * BUILD_BUG_ON_MSG - break compile if a condition is true & emit supplied
 *                    error message.
 * @condition: the condition which the compiler should know is false.
 *
 * See BUILD_BUG_ON for description.
 */
#define BUILD_BUG_ON_MSG(cond, msg) compiletime_assert(!(cond), msg)

/**
 * BUILD_BUG_ON - break compile if a condition is true.
 * @condition: the condition which the compiler should know is false.
 *
 * If you have some code which relies on certain constants being equal, or
 * some other compile-time-evaluated condition, you should use BUILD_BUG_ON to
 * detect if someone changes it.
 */
#define BUILD_BUG_ON(condition) \
        BUILD_BUG_ON_MSG(condition, "BUILD_BUG_ON failed: " #condition)

#define BUILD_BUG_ON_ZERO(e) ((int)(sizeof(struct { int : (-!!(e)); })))
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __must_be_array(a)     BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#define MODULE_AUTHOR(author)
#define MODULE_LICENSE(license)
#define KSTM_MODULE_LOADERS(module)
#define GIT_VERSION "unknown (this isn't the linux kernel itself lol)"

enum {
    MAGIC_SKIP_RETURN_VALUE = 99,
};

static int total_tests = 0;
static int skipped_tests = 0;
static int failed_tests = 0;

typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned int gfp_t;
typedef uint8_t u8;

static inline u32 get_random_u32(void)
{
    return mrand48();
}

static inline u32 prandom_u32(void)
{
        return get_random_u32();
}

static inline u32 prandom_u32_max(u32 ep_ro)
{
        return (u32)(((u64) prandom_u32() * ep_ro) >> 32);
}

static inline char *kvasprintf(gfp_t gfp, const char *fmt, va_list ap)
{
    (void)gfp;
    return vstrdupf(fmt, ap);
}

static inline void *__must_check ERR_PTR(long error)
{
        return (void *) error;
}

static inline void *kmalloc(size_t size, gfp_t flags)
{
    (void)flags;
    return malloc(size);
}

static inline void test_start(char *name)
{
    //printf("test: %s\n", name);
}

static inline void test_set_git_version(char *value)
{
    //printf("tags: git_version:%s\n", value);
}

static inline void test_harness_internal_sig_handler(int signum)
{
        /* Just wake us up from waitpid */
}

static inline void test_error(char *name)
{
        printf("error: %s\n", name);
}

static struct sigaction test_harness_internal_sig_action = {
        .sa_handler = test_harness_internal_sig_handler,
};

/* Setting timeout to -1 disables the alarm */
static uint64_t test_harness_internal_timeout = 120;

enum {
    TEST_HARNESS_INTERNAL_KILL_TIMEOUT = 5
};

static inline int test_harness_internal_run_test(int (test_function)(void), char *name)
{
        bool terminated;
        int rc, status;
        pid_t pid;

        /* Make sure output is flushed before forking */
        assert(fflush(stdout) == 0);

        pid = fork();
        if (pid == 0) {
                setpgid(0, 0);
                exit(test_function());
        } else if (pid == -1) {
                perror("fork");
                return 1;
        }

        setpgid(pid, pid);

        if (test_harness_internal_timeout != -1)
                /* Wake us up in test_harness_internal_timeout seconds */
                alarm(test_harness_internal_timeout);
        terminated = false;

wait:
        rc = waitpid(pid, &status, 0);
        if (rc == -1) {
                if (errno != EINTR) {
                        printf("unknown error from waitpid\n");
                        return 1;
                }

                if (terminated) {
                        printf("!! force killing %s\n", name);
                        kill(-pid, SIGKILL);
                        return 1;
                } else {
                        printf("!! killing %s\n", name);
                        kill(-pid, SIGTERM);
                        terminated = true;
                        alarm(TEST_HARNESS_INTERNAL_KILL_TIMEOUT);
                        goto wait;
                }
        }

        /* Kill anything else in the process group that is still running */
        kill(-pid, SIGTERM);

        if (WIFEXITED(status))
                status = WEXITSTATUS(status);
        else {
                if (WIFSIGNALED(status))
                        printf("!! child died by signal %d\n", WTERMSIG(status));
                else
                        printf("!! child died by unknown cause\n");

                status = 1; /* Signal or other */
        }

        return status;
}

static inline void test_skip(char *name)
{
        printf("skip: %s\n", name);
}

static inline void test_failure(char *name)
{
        printf("failure: %s\n", name);
}

static inline void test_success(char *name)
{
    //printf("success: %s\n", name);
}

static inline void test_finish(char *name, int status)
{
        if (status)
                test_failure(name);
        else
                test_success(name);
}

static inline int test_harness(int (test_function)(void), char *name)
{
        int rc;

        test_start(name);
        test_set_git_version(GIT_VERSION);

        if (sigaction(SIGINT, &test_harness_internal_sig_action, NULL)) {
                perror("sigaction (sigint)");
                test_error(name);
                return 1;
        }

        if (sigaction(SIGALRM, &test_harness_internal_sig_action, NULL)) {
                perror("sigaction (sigalrm)");
                test_error(name);
                return 1;
        }

        rc = test_harness_internal_run_test(test_function, name);

        if (rc == MAGIC_SKIP_RETURN_VALUE) {
                test_skip(name);
                /* so that skipped test is not marked as failed */
                rc = 0;
        } else
                test_finish(name, rc);

        return rc;
}
