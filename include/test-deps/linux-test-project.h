// Derived from code with this license:
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2015-2016 Cyril Hrubis <chrubis@suse.cz>
 * Copyright (c) Linux Test Project, 2016-2019
 */

#pragma once

#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/MIN.h"
#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>

const char *TCID;

/* update also docparse/testinfo.pl */
#define LINUX_GIT_URL "https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id="
#define LINUX_STABLE_GIT_URL "https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/commit/?id="
#define GLIBC_GIT_URL "https://sourceware.org/git/?p=glibc.git;a=commit;h="
#define CVE_DB_URL "https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-"

enum {
    DEFAULT_TIMEOUT = 30
};

#define PATH_HUGEPAGES  "/sys/kernel/mm/hugepages/"
#define PATH_NR_HPAGES  "/proc/sys/vm/nr_hugepages"
#define PATH_OC_HPAGES "/proc/sys/vm/nr_overcommit_hugepages"

#define MEMINFO_HPAGE_TOTAL  "HugePages_Total:"
#define MEMINFO_HPAGE_FREE   "HugePages_Free:"
#define MEMINFO_HPAGE_RSVD   "HugePages_Rsvd:"
#define MEMINFO_HPAGE_SURP   "HugePages_Surp:"
#define MEMINFO_HPAGE_SIZE   "Hugepagesize:"

extern char *nr_opt; /* -s num   Set the number of the been allocated hugepages */
extern char *Hopt;   /* -H /..   Location of hugetlbfs, i.e.  -H /var/hugetlbfs */

enum tst_hp_policy {
    TST_REQUEST,
    TST_NEEDS,
};

struct tst_hugepage {
    const unsigned long number;
    enum  tst_hp_policy policy;
};

/*
 * Get the default hugepage size. Returns 0 if hugepages are not supported.
 */
size_t tst_get_hugepage_size(void);

/*
 * Try the best to request a specified number of huge pages from system,
 * it will store the reserved hpage number in tst_hugepages.
 *
 * Note: this depend on the status of system memory fragmentation.
 */
unsigned long tst_reserve_hugepages(struct tst_hugepage *hp);

/*
 * This variable is used for recording the number of hugepages which system can
 * provides. It will be equal to 'hpages' if tst_reserve_hugepages on success,
 * otherwise set it to a number of hugepages that we were able to reserve.
 *
 * If system does not support hugetlb, then it will be set to 0.
 */
extern unsigned long tst_hugepages;

/* CGroups Kernel API version */
enum tst_cg_ver {
    TST_CG_V1 = 1,
    TST_CG_V2 = 2,
};

/* This value is greater than ROOTS_MAX in tst_cgroup.c. */
enum {
    TST_CG_ROOTS_MAX = 32
};

/* Used to specify CGroup hierarchy configuration options, allowing a
 * test to request a particular CGroup structure.
 */
struct tst_cg_opts {
    /* Call tst_brk with TCONF if the controller is not on this
     * version. Defautls to zero to accept any version.
     */
    enum tst_cg_ver needs_ver;
    /* Pass in a specific pid to create and identify the test
     * directory as opposed to the default pid of the calling process.
     */
    int test_pid;
};

/* A Control Group in LTP's aggregated hierarchy */
struct tst_cg_group;

/* Populated with a reference to this tests's CGroup */
extern const struct tst_cg_group *const tst_cg;
extern const struct tst_cg_group *const tst_cg_drain;

struct tst_option {
    char *optstr;
    char **arg;
    char *help;
};

struct tst_tag {
    const char *name;
    const char *value;
};

struct tst_test {
    /* number of tests available in test() function */
    unsigned int tcnt;

    struct tst_option *options;

    const char *min_kver;

    /*
     * The supported_archs is a NULL terminated list of archs the test
     * does support.
     */
    const char *const *supported_archs;

    /* If set the test is compiled out */
    const char *tconf_msg;

    unsigned needs_tmpdir:1;
    unsigned needs_root:1;
    unsigned forks_child:1;
    unsigned needs_device:1;
    unsigned needs_checkpoints:1;
    unsigned needs_overlay:1;
    unsigned format_device:1;
    unsigned mount_device:1;
    unsigned needs_rofs:1;
    unsigned child_needs_reinit:1;
    unsigned needs_devfs:1;
    unsigned restore_wallclock:1;
    /*
     * If set the test function will be executed for all available
     * filesystems and the current filesystem type would be set in the
     * tst_device->fs_type.
     *
     * The test setup and cleanup are executed before/after __EACH__ call
     * to the test function.
     */
    unsigned all_filesystems:1;
    unsigned skip_in_lockdown:1;
    unsigned skip_in_compat:1;
    /*
     * If set, the hugetlbfs will be mounted at .mntpoint.
     */
    unsigned needs_hugetlbfs:1;

    /*
     * The skip_filesystems is a NULL terminated list of filesystems the
     * test does not support. It can also be used to disable whole class of
     * filesystems with a special keywords such as "fuse".
     */
    const char *const *skip_filesystems;

    /* Minimum number of online CPU required by the test */
    unsigned long min_cpus;

    /* Minimum size(MB) of MemAvailable required by the test */
    unsigned long min_mem_avail;

    /*
     * Two policies for reserving hugepage:
     *
     * TST_REQUEST:
     *   It will try the best to reserve available huge pages and return the number
     *   of available hugepages in tst_hugepages, which may be 0 if hugepages are
     *   not supported at all.
     *
     * TST_NEEDS:
     *   This is an enforced requirement, LTP should strictly do hpages applying and
     *   guarantee the 'HugePages_Free' no less than pages which makes that test can
     *   use these specified numbers correctly. Otherwise, test exits with TCONF if
     *   the attempt to reserve hugepages fails or reserves less than requested.
     *
     * With success test stores the reserved hugepage number in 'tst_hugepages. For
     * the system without hugetlb supporting, variable 'tst_hugepages' will be set to 0.
     * If the hugepage number needs to be set to 0 on supported hugetlb system, please
     * use '.hugepages = {TST_NO_HUGEPAGES}'.
     *
     * Also, we do cleanup and restore work for the hpages resetting automatically.
     */
    struct tst_hugepage hugepages;

    /*
     * If set to non-zero, call tst_taint_init(taint_check) during setup
     * and check kernel taint at the end of the test. If all_filesystems
     * is non-zero, taint check will be performed after each FS test and
     * testing will be terminated by TBROK if taint is detected.
     */
    unsigned int taint_check;

    /*
     * If set non-zero denotes number of test variant, the test is executed
     * variants times each time with tst_variant set to different number.
     *
     * This allows us to run the same test for different settings. The
     * intended use is to test different syscall wrappers/variants but the
     * API is generic and does not limit the usage in any way.
     */
    unsigned int test_variants;

    /* Minimal device size in megabytes */
    unsigned int dev_min_size;

    /* Device filesystem type override NULL == default */
    const char *dev_fs_type;

    /* Options passed to SAFE_MKFS() when format_device is set */
    const char *const *dev_fs_opts;
    const char *const *dev_extra_opts;

    /* Device mount options, used if mount_device is set */
    const char *mntpoint;
    unsigned int mnt_flags;
    void *mnt_data;

    /*
     * Maximal test runtime in seconds.
     *
     * Any test that runs for more than a second or two should set this and
     * also use tst_remaining_runtime() to exit when runtime was used up.
     * Tests may finish sooner, for example if requested number of
     * iterations was reached before the runtime runs out.
     *
     * If test runtime cannot be know in advance it should be set to
     * TST_UNLIMITED_RUNTIME.
     */
    int max_runtime;

    void (*setup)(void);
    void (*cleanup)(void);

    void (*test)(unsigned int test_nr);
    void (*test_all)(void);

    /* Syscall name used by the timer measurement library */
    const char *scall;

    /* Sampling function for timer measurement testcases */
    int (*sample)(int clk_id, long long usec);

    /* NULL terminated array of resource file names */
    const char *const *resource_files;

    /* NULL terminated array of needed kernel drivers */
    const char * const *needs_drivers;

    /*
     * {NULL, NULL} terminated array of (/proc, /sys) files to save
     * before setup and restore after cleanup
     */
    const struct tst_path_val *save_restore;

    /*
     * NULL terminated array of kernel config options required for the
     * test.
     */
    const char *const *needs_kconfigs;

    /*
     * {NULL, NULL} terminated array to be allocated buffers.
     */
    struct tst_buffers *bufs;

    /*
     * {NULL, NULL} terminated array of capability settings
     */
    struct tst_cap *caps;

    /*
     * {NULL, NULL} terminated array of tags.
     */
    const struct tst_tag *tags;

    /* NULL terminated array of required commands */
    const char *const *needs_cmds;

    /* Requires a particular CGroup API version. */
    const enum tst_cg_ver needs_cgroup_ver;

    /* {} terminated array of required CGroup controllers */
    const char *const *needs_cgroup_ctrls;
};

struct tst_test *tst_test;

static const char *tid;
static int iterations = 1;
static float duration = -1;
static float timeout_mul = -1;
static pid_t main_pid, lib_pid;
static int mntpoint_mounted;
static int ovl_mounted;

/*
 * Reports result and exits a test.
 */
void tst_brk_(const char *file, int lineno, int ttype,
              const char *fmt, ...)
              __attribute__ ((format (printf, 4, 5)));

#define TST_RETRY_FN_EXP_BACKOFF(FUNC, ECHCK, MAX_DELAY)    \
({  unsigned int tst_delay_, tst_max_delay_;            \
    typeof(FUNC) tst_ret_;                      \
    tst_delay_ = 1;                         \
    tst_max_delay_ = tst_multiply_timeout(MAX_DELAY * 1000000); \
    for (;;) {                          \
        errno = 0;                      \
        tst_ret_ = FUNC;                    \
        if (ECHCK(tst_ret_))                    \
            break;                      \
        if (tst_delay_ < tst_max_delay_) {          \
            usleep(tst_delay_);             \
            tst_delay_ *= 2;                \
        } else {                        \
            break;                      \
        }                           \
    }                               \
    tst_ret_;                           \
})

/*
 * Return value validation macros for TST_RETRY_FUNC():
 * TST_RETVAL_EQ0() - Check that value is equal to zero
 */
#define TST_RETVAL_EQ0(x) (!(x))

/*
 * TST_RETVAL_NOTNULL() - Check that value is not equal to zero/NULL
 */
#define TST_RETVAL_NOTNULL(x) (!!(x))

/*
 * TST_RETVAL_GE0() - Check that value is greater than or equal to zero
 */
#define TST_RETVAL_GE0(x) ((x) >= 0)

#define TST_BUILD_BUG_ON(condition) \
    do { ((void)sizeof(char[1 - 2 * !!(condition)])); } while (0)

#define TST_BRK_SUPPORTS_ONLY_TCONF_TBROK(condition) \
    TST_BUILD_BUG_ON(condition)

#define TST_RES_SUPPORTS_TCONF_TFAIL_TINFO_TPASS_TWARN(condition) \
    TST_BUILD_BUG_ON(condition)

/* stringification */
#define TST_TO_STR_(s) #s
#define TST_TO_STR(s) TST_TO_STR_(s)

#define tst_brk(ttype, arg_fmt, ...)                        \
    ({                                  \
        TST_BRK_SUPPORTS_ONLY_TCONF_TBROK(!((ttype) &           \
            (TBROK | TCONF | TFAIL)));              \
        tst_brk_(__FILE__, __LINE__, (ttype), (arg_fmt), ##__VA_ARGS__);\
    })

/* Use low 6 bits to encode test type */
#define TTYPE_MASK  0x3f
#define TPASS   0   /* Test passed flag */
#define TFAIL   1   /* Test failed flag */
#define TBROK   2   /* Test broken flag */
#define TWARN   4   /* Test warning flag */
#define TINFO   16  /* Test information flag */
#define TCONF   32  /* Test not appropriate for configuration flag */
#define TTYPE_RESULT(ttype) ((ttype) & TTYPE_MASK)

#define TERRNO  0x100   /* Append errno information to output */
#define TTERRNO 0x200   /* Append TEST_ERRNO information to output */
#define TRERRNO 0x400   /* Capture errno information from TEST_RETURN to
               output; useful for pthread-like APIs :). */

struct results {
    int passed;
    int skipped;
    int failed;
    int warnings;
    int broken;
    unsigned int timeout;
    int max_runtime;
};

static struct results *results;

static int ipc_fd;

extern unsigned int tst_max_futexes;

static char ipc_path[1064];
const char *tst_ipc_path = ipc_path;

static char shm_path[1024];

int TST_ERR;
int TST_PASS;
long TST_RET;

static void assert_test_fn(void)
{
    int cnt = 0;

    if (tst_test->test)
        cnt++;

    if (tst_test->test_all)
        cnt++;

    if (tst_test->sample)
        cnt++;

    if (!cnt)
        tst_brk(TBROK, "No test function specified");

    if (cnt != 1)
        tst_brk(TBROK, "You can define only one test function");

    if (tst_test->test && !tst_test->tcnt)
        tst_brk(TBROK, "Number of tests (tcnt) must be > 0");

    if (!tst_test->test && tst_test->tcnt)
        tst_brk(TBROK, "You can define tcnt only for test()");
}

/*
 * Reports testcase result.
 */
void tst_res_(const char *file, int lineno, int ttype,
              const char *fmt, ...)
              __attribute__ ((format (printf, 4, 5)));

#define tst_res(ttype, arg_fmt, ...) \
    ({                                  \
        TST_RES_SUPPORTS_TCONF_TFAIL_TINFO_TPASS_TWARN(!((TTYPE_RESULT(ttype) ?: TCONF) & \
            (TCONF | TFAIL | TINFO | TPASS | TWARN)));              \
        tst_res_(__FILE__, __LINE__, (ttype), (arg_fmt), ##__VA_ARGS__);\
    })

static const char *get_tid(char *argv[])
{
    char *p;

    if (!argv || !argv[0] || !argv[0][0]) {
        tst_res(TINFO, "argv[0] is empty!");
        return "ltp_empty_argv";
    }

    p = strrchr(argv[0], '/');
    if (p)
        return p+1;

    return argv[0];
}

static unsigned int count_options(void)
{
    unsigned int i;

    if (!tst_test->options)
        return 0;

    for (i = 0; tst_test->options[i].optstr; i++);

    return i;
}

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

static struct linux_test_project_option {
    const char *optstr;
    const char *help;
} options[] = {
    {"h",  "-h       Prints this help"},
    {"i:", "-i n     Execute test n times"},
    {"I:", "-I x     Execute test for n seconds"},
    {"C:", "-C ARG   Run child process with ARG arguments (used internally)"},
};

static void check_option_collision(void)
{
    unsigned int i, j;
    struct tst_option *toptions = tst_test->options;

    if (!toptions)
        return;

    for (i = 0; toptions[i].optstr; i++) {
        for (j = 0; j < ARRAY_SIZE(options); j++) {
            if (toptions[i].optstr[0] == options[j].optstr[0]) {
                tst_brk(TBROK, "Option collision '%s'",
                        options[j].help);
            }
        }
    }
}

/*
 * This is the default temporary directory used by tst_tmpdir().
 *
 * This is used when TMPDIR env variable is not set.
 */
#define TEMPDIR "/tmp"

/*
 * Default filesystem to be used for tests.
 */
#define DEFAULT_FS_TYPE "ext2"

int tst_parse_float(const char *str, float *val, float min, float max)
{
    double rval;
    char *end;

    if (!str)
        return 0;

    errno = 0;
    rval = strtod(str, &end);

    if (str == end || *end != '\0')
        return EINVAL;

    if (errno)
        return errno;

    if (rval > (double)max || rval < (double)min)
        return ERANGE;

    *val = (float)rval;
    return 0;
}

const char *tst_strerrno(int err)
{
    (void)err;
    return "???";
    /*static const struct pair errno_pairs[] = {
        STRPAIR(0, "SUCCESS")
        // asm-generic/errno-base.h
        PAIR(EPERM)
        PAIR(ENOENT)
        PAIR(ESRCH)
        PAIR(EINTR)
        PAIR(EIO)
        PAIR(ENXIO)
        PAIR(E2BIG)
        PAIR(ENOEXEC)
        PAIR(EBADF)
        PAIR(ECHILD)
        STRPAIR(EAGAIN, "EAGAIN/EWOULDBLOCK")
        PAIR(ENOMEM)
        PAIR(EACCES)
        PAIR(EFAULT)
        PAIR(ENOTBLK)
        PAIR(EBUSY)
        PAIR(EEXIST)
        PAIR(EXDEV)
        PAIR(ENODEV)
        PAIR(ENOTDIR)
        PAIR(EISDIR)
        PAIR(EINVAL)
        PAIR(ENFILE)
        PAIR(EMFILE)
        PAIR(ENOTTY)
        PAIR(ETXTBSY)
        PAIR(EFBIG)
        PAIR(ENOSPC)
        PAIR(ESPIPE)
        PAIR(EROFS)
        PAIR(EMLINK)
        PAIR(EPIPE)
        PAIR(EDOM)
        PAIR(ERANGE)
        // asm-generic/errno.h
        PAIR(EDEADLK)
        PAIR(ENAMETOOLONG)
        PAIR(ENOLCK)
        PAIR(ENOSYS)
        PAIR(ENOTEMPTY)
        PAIR(ELOOP)
        // EWOULDBLOCK == EAGAIN skipped
        PAIR(ENOMSG)
        PAIR(EIDRM)
        PAIR(ECHRNG)
        PAIR(EL2NSYNC)
        PAIR(EL3HLT)
        PAIR(EL3RST)
        PAIR(ELNRNG)
        PAIR(EUNATCH)
        PAIR(ENOCSI)
        PAIR(EL2HLT)
        PAIR(EBADE)
        PAIR(EBADR)
        PAIR(EXFULL)
        PAIR(ENOANO)
        PAIR(EBADRQC)
        PAIR(EBADSLT)
        // EDEADLOCK == EDEADLK skipped
        PAIR(EBFONT)
        PAIR(ENOSTR)
        PAIR(ENODATA)
        PAIR(ETIME)
        PAIR(ENOSR)
        PAIR(ENONET)
        PAIR(ENOPKG)
        PAIR(EREMOTE)
        PAIR(ENOLINK)
        PAIR(EADV)
        PAIR(ESRMNT)
        PAIR(ECOMM)
        PAIR(EPROTO)
        PAIR(EMULTIHOP)
        PAIR(EDOTDOT)
        PAIR(EBADMSG)
        PAIR(EOVERFLOW)
        PAIR(ENOTUNIQ)
        PAIR(EBADFD)
        PAIR(EREMCHG)
        PAIR(ELIBACC)
        PAIR(ELIBBAD)
        PAIR(ELIBSCN)
        PAIR(ELIBMAX)
        PAIR(ELIBEXEC)
        PAIR(EILSEQ)
        PAIR(ERESTART)
        PAIR(ESTRPIPE)
        PAIR(EUSERS)
        PAIR(ENOTSOCK)
        PAIR(EDESTADDRREQ)
        PAIR(EMSGSIZE)
        PAIR(EPROTOTYPE)
        PAIR(ENOPROTOOPT)
        PAIR(EPROTONOSUPPORT)
        PAIR(ESOCKTNOSUPPORT)
        PAIR(EOPNOTSUPP)
        PAIR(EPFNOSUPPORT)
        PAIR(EAFNOSUPPORT)
        PAIR(EADDRINUSE)
        PAIR(EADDRNOTAVAIL)
        PAIR(ENETDOWN)
        PAIR(ENETUNREACH)
        PAIR(ENETRESET)
        PAIR(ECONNABORTED)
        PAIR(ECONNRESET)
        PAIR(ENOBUFS)
        PAIR(EISCONN)
        PAIR(ENOTCONN)
        PAIR(ESHUTDOWN)
        PAIR(ETOOMANYREFS)
        PAIR(ETIMEDOUT)
        PAIR(ECONNREFUSED)
        PAIR(EHOSTDOWN)
        PAIR(EHOSTUNREACH)
        PAIR(EALREADY)
        PAIR(EINPROGRESS)
        PAIR(ESTALE)
        PAIR(EUCLEAN)
        PAIR(ENOTNAM)
        PAIR(ENAVAIL)
        PAIR(EISNAM)
        PAIR(EREMOTEIO)
        PAIR(EDQUOT)
        PAIR(ENOMEDIUM)
        PAIR(EMEDIUMTYPE)
        PAIR(ECANCELED)
#ifdef ENOKEY
        PAIR(ENOKEY)
#endif
#ifdef EKEYEXPIRED
        PAIR(EKEYEXPIRED)
#endif
#ifdef EKEYREVOKED
        PAIR(EKEYREVOKED)
#endif
#ifdef EKEYREJECTED
        PAIR(EKEYREJECTED)
#endif
#ifdef EOWNERDEAD
        PAIR(EOWNERDEAD)
#endif
#ifdef ENOTRECOVERABLE
        PAIR(ENOTRECOVERABLE)
#endif
#ifdef ERFKILL
        PAIR(ERFKILL)
#endif
#ifdef EHWPOISON
        PAIR(EHWPOISON)
#endif
    };

    PAIR_LOOKUP(errno_pairs, err);*/
}

static void parse_mul(float *mul, const char *env_name, float min, float max)
{
    char *str_mul;
    int ret;

    if (*mul > 0)
        return;

    str_mul = getenv(env_name);

    if (!str_mul) {
        *mul = 1;
        return;
    }

    ret = tst_parse_float(str_mul, mul, min, max);
    if (ret) {
        tst_brk(TBROK, "Failed to parse %s: %s",
            env_name, tst_strerrno(ret));
    }
}

unsigned int tst_multiply_timeout(unsigned int timeout)
{
    parse_mul(&timeout_mul, "LTP_TIMEOUT_MUL", 0.099, 10000);

    if (timeout < 1)
        tst_brk(TBROK, "timeout must to be >= 1! (%d)", timeout);

    return timeout * timeout_mul;
}

static void print_help(void)
{
    unsigned int i;
    int timeout, runtime;

    /* see doc/user-guide.txt, which lists also shell API variables */
    assert(fprintf(stderr, "Environment Variables\n") >= 0);
    assert(fprintf(stderr, "---------------------\n") >= 0);
    assert(fprintf(stderr, "KCONFIG_PATH         Specify kernel config file\n") >= 0);
    assert(fprintf(stderr, "KCONFIG_SKIP_CHECK   Skip kernel config check if variable set (not set by default)\n") >= 0);
    assert(fprintf(stderr, "LTPROOT              Prefix for installed LTP (default: /opt/ltp)\n") >= 0);
    assert(fprintf(stderr, "LTP_COLORIZE_OUTPUT  Force colorized output behaviour (y/1 always, n/0: never)\n") >= 0);
    assert(fprintf(stderr, "LTP_DEV              Path to the block device to be used (for .needs_device)\n") >= 0);
    assert(fprintf(stderr, "LTP_DEV_FS_TYPE      Filesystem used for testing (default: %s)\n", DEFAULT_FS_TYPE) >= 0);
    assert(fprintf(stderr, "LTP_SINGLE_FS_TYPE   Testing only - specifies filesystem instead all supported (for .all_filesystems)\n") >= 0);
    assert(fprintf(stderr, "LTP_TIMEOUT_MUL      Timeout multiplier (must be a number >=1)\n") >= 0);
    assert(fprintf(stderr, "LTP_RUNTIME_MUL      Runtime multiplier (must be a number >=1)\n") >= 0);
    assert(fprintf(stderr, "LTP_VIRT_OVERRIDE    Overrides virtual machine detection (values: \"\"|kvm|microsoft|xen|zvm)\n") >= 0);
    assert(fprintf(stderr, "TMPDIR               Base directory for template directory (for .needs_tmpdir, default: %s)\n", TEMPDIR) >= 0);
    assert(fprintf(stderr, "\n") == 1);

    assert(fprintf(stderr, "Timeout and runtime\n") >= 0);
    assert(fprintf(stderr, "-------------------\n") >= 0);

    assert(!tst_test->max_runtime);/*if (tst_test->max_runtime) {
        runtime = multiply_runtime(tst_test->max_runtime);

        if (runtime == TST_UNLIMITED_RUNTIME) {
            fprintf(stderr, "Test iteration runtime is not limited\n");
        } else {
            fprintf(stderr, "Test iteration runtime cap %ih %im %is\n",
                runtime/3600, (runtime%3600)/60, runtime % 60);
        }
    }*/

    timeout = tst_multiply_timeout(DEFAULT_TIMEOUT);

    assert(fprintf(stderr, "Test timeout (not including runtime) %ih %im %is\n",
                   timeout/3600, (timeout%3600)/60, timeout % 60) >= 0);

    assert(fprintf(stderr, "\n") == 1);

    assert(fprintf(stderr, "Options\n") == 8);
    assert(fprintf(stderr, "-------\n") == 8);

    for (i = 0; i < ARRAY_SIZE(options); i++)
        assert(fprintf(stderr, "%s\n", options[i].help) == strlen(options[i].help) + 1);

    if (!tst_test->options)
        return;

    for (i = 0; tst_test->options[i].optstr; i++) {
        assert(fprintf(stderr, "-%c\t %s\n",
                       tst_test->options[i].optstr[0],
                       tst_test->options[i].help) == 5 + strlen(tst_test->options[i].help));
    }
}

static void print_test_tags(void)
{
    unsigned int i;
    const struct tst_tag *tags = tst_test->tags;

    if (!tags)
        return;

    assert(fprintf(stderr, "\nTags\n") == 6);
    assert(fprintf(stderr, "----\n") == 5);

    for (i = 0; tags[i].name; i++) {
        if (!strcmp(tags[i].name, "CVE"))
            assert(fprintf(stderr, CVE_DB_URL "%s\n", tags[i].value) > strlen(tags[i].value) + 1);
        else if (!strcmp(tags[i].name, "linux-git"))
            assert(fprintf(stderr, LINUX_GIT_URL "%s\n", tags[i].value) > strlen(tags[i].value) + 1);
        else if (!strcmp(tags[i].name, "linux-stable-git"))
            assert(fprintf(stderr, LINUX_STABLE_GIT_URL "%s\n", tags[i].value) > strlen(tags[i].value) + 1);
        else if (!strcmp(tags[i].name, "glibc-git"))
            assert(fprintf(stderr, GLIBC_GIT_URL "%s\n", tags[i].value) > strlen(tags[i].value) + 1);
        else
            assert(fprintf(stderr, "%s: %s\n", tags[i].name, tags[i].value) == 3 + strlen(tags[i].name) + strlen(tags[i].value));
    }

    assert(fprintf(stderr, "\n") == 1);
}

#define SAFE_SYMLINK(oldpath, newpath) \
    safe_symlink(__FILE__, __LINE__, NULL, (oldpath), (newpath))

#define SAFE_WRITE(len_strict, fildes, buf, nbyte) \
    safe_write(__FILE__, __LINE__, NULL, (len_strict), (fildes), (buf), (nbyte))

#define SAFE_STRTOL(str, min, max) \
    safe_strtol(__FILE__, __LINE__, NULL, (str), (min), (max))

#define SAFE_STRTOUL(str, min, max) \
    safe_strtoul(__FILE__, __LINE__, NULL, (str), (min), (max))

#define SAFE_STRTOF(str, min, max) \
    safe_strtof(__FILE__, __LINE__, NULL, (str), (min), (max))

#define SAFE_SYSCONF(name) \
    safe_sysconf(__FILE__, __LINE__, NULL, name)

#define SAFE_CHMOD(path, mode) \
    safe_chmod(__FILE__, __LINE__, NULL, (path), (mode))

#define SAFE_FCHMOD(fd, mode) \
    safe_fchmod(__FILE__, __LINE__, NULL, (fd), (mode))

#define SAFE_CHOWN(path, owner, group) \
    safe_chown(__FILE__, __LINE__, NULL, (path), (owner), (group))

#define SAFE_FCHOWN(fd, owner, group) \
    safe_fchown(__FILE__, __LINE__, NULL, (fd), (owner), (group))

#define SAFE_WAIT(status) \
    safe_wait(__FILE__, __LINE__, NULL, (status))

#define SAFE_WAITPID(pid, status, opts) \
    safe_waitpid(__FILE__, __LINE__, NULL, (pid), (status), (opts))

#define SAFE_KILL(pid, sig) \
    safe_kill(__FILE__, __LINE__, NULL, (pid), (sig))

#define SAFE_MEMALIGN(alignment, size) \
    safe_memalign(__FILE__, __LINE__, NULL, (alignment), (size))

#define SAFE_MKFIFO(pathname, mode) \
    safe_mkfifo(__FILE__, __LINE__, NULL, (pathname), (mode))

#define SAFE_RENAME(oldpath, newpath) \
    safe_rename(__FILE__, __LINE__, NULL, (oldpath), (newpath))

# define tst_brkm(flags, cleanup, fmt, ...) do { \
    if (tst_test) \
        tst_brk_(__FILE__, __LINE__, flags, fmt, ##__VA_ARGS__); \
    else \
        tst_brkm__(__FILE__, __LINE__, flags, cleanup, fmt, ##__VA_ARGS__); \
    } while (0)

#define tst_brkm_(file, lineno, flags, cleanup, fmt, ...) do { \
    if (tst_test) \
        tst_brk_(file, lineno, flags, fmt, ##__VA_ARGS__); \
    else \
        tst_brkm__(file, lineno, flags, cleanup, fmt, ##__VA_ARGS__); \
    } while (0)

long TEST_RETURN;
int TEST_ERRNO;
void *TST_RET_PTR;

enum {
    VERBOSE = 1,
    NOPASS = 3,
    DISCARD = 4,

    MAXMESG = 80,     /* max length of internal messages */
    USERMESG = 2048,   /* max length of user message */
    TRUE = 1,
    FALSE = 0,
};

/*
 * EXPAND_VAR_ARGS - Expand the variable portion (arg_fmt) of a result
 *                   message into the specified string.
 *
 * NOTE (garrcoop):  arg_fmt _must_ be the last element in each function
 *           argument list that employs this.
 */
#define EXPAND_VAR_ARGS(buf, arg_fmt, buf_len) do {\
    va_list ap;             \
    assert(arg_fmt != NULL);        \
    va_start(ap, arg_fmt);          \
    assert(vsnprintf(buf, buf_len, arg_fmt, ap) == strlen(buf));        \
    va_end(ap);             \
    assert(strlen(buf) > 0);        \
} while (0)

#if !defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP) && defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#endif

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
static pthread_mutex_t tmutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
static pthread_mutex_t tmutex;

__attribute__((constructor))
static void init_tmutex(void)
{
    pthread_mutexattr_t mutattr = {0};

    pthread_mutexattr_init(&mutattr);
    pthread_mutexattr_settype(&mutattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&tmutex, &mutattr);
    pthread_mutexattr_destroy(&mutattr);
}
#endif

static int T_exitval = 0;   /* exit value used by tst_exit() */
static int passed_cnt;
static int T_mode = VERBOSE;    /* flag indicating print mode: VERBOSE, */
                /* NOPASS, DISCARD */

static char Warn_mesg[MAXMESG]; /* holds warning messages */

/*
 * These are used for condensing output when NOT in verbose mode.
 */
static int Buffered = FALSE;    /* TRUE if condensed output is currently */
                /* buffered (i.e. not yet printed) */
static char *Last_tcid;     /* previous test case id */
static int Last_num;        /* previous test case number */
static int Last_type;       /* previous test result type */
static char *Last_mesg;     /* previous test result message */

int tst_count = 0;

#define PAIR(def) [def] = {.name = #def, .val = def},
#define STRPAIR(key, value) [key] = {.name = value, .val = key},

#define PAIR_LOOKUP(pair_arr, idx) do {                       \
    if ((size_t)idx >= ARRAY_SIZE(pair_arr) || \
        pair_arr[idx].name == NULL)                       \
        return "???";                                 \
    return pair_arr[idx].name;                            \
} while (0)

struct pair {
    const char *name;
    int val;
};

const char *strttype(int ttype)
{
    static const struct pair ttype_pairs[] = {
        PAIR(TPASS)
        PAIR(TFAIL)
        PAIR(TBROK)
        PAIR(TCONF)
        PAIR(TWARN)
        PAIR(TINFO)
    };

    PAIR_LOOKUP(ttype_pairs, TTYPE_RESULT(ttype));
}

int tst_color_enabled(int fd)
{
    static int color;

    if (color)
        return color - 1;

    char *env = getenv("LTP_COLORIZE_OUTPUT");

    if (env) {
        if (!strcmp(env, "n") || !strcmp(env, "0"))
            color = 1;

        if (!strcmp(env, "y") || !strcmp(env, "1"))
            color = 2;

        return color - 1;
    }

    if (isatty(fd) == 0)
        color = 1;
    else
        color = 2;

    return color - 1;
}

/*
 * NOTE: these colors should match colors defined in tst_flag2color() in
 * testcases/lib/tst_ansi_color.sh
 */
#define ANSI_COLOR_BLUE     "\033[1;34m"
#define ANSI_COLOR_GREEN    "\033[1;32m"
#define ANSI_COLOR_MAGENTA  "\033[1;35m"
#define ANSI_COLOR_RED      "\033[1;31m"
#define ANSI_COLOR_YELLOW   "\033[1;33m"

#define ANSI_COLOR_RESET    "\033[0m"

char* tst_ttype2color(int ttype)
{
    switch (TTYPE_RESULT(ttype)) {
    case TPASS:
        return ANSI_COLOR_GREEN;
    break;
    case TFAIL:
        return ANSI_COLOR_RED;
    break;
    case TBROK:
        return ANSI_COLOR_RED;
    break;
    case TCONF:
        return ANSI_COLOR_YELLOW;
    break;
    case TWARN:
        return ANSI_COLOR_MAGENTA;
    break;
    case TINFO:
        return ANSI_COLOR_BLUE;
    break;
    default:
        return "";
    }
}

static void tst_print(const char *tcid, int tnum, int ttype, const char *tmesg)
{
    int err = errno;
    const char *type;
    int ttype_result = TTYPE_RESULT(ttype);
    char message[USERMESG];
    size_t size = 0;

    /*
     * Save the test result type by ORing ttype into the current exit value
     * (used by tst_exit()).  This is already done in tst_res(), but is
     * also done here to catch internal warnings.  For internal warnings,
     * tst_print() is called directly with a case of TWARN.
     */
    T_exitval |= ttype_result;

    /*
     * If output mode is DISCARD, or if the output mode is NOPASS and this
     * result is not one of FAIL, BROK, or WARN, just return.  This check
     * is necessary even though we check for DISCARD mode inside of
     * tst_res(), since occasionally we get to this point without going
     * through tst_res() (e.g. internal TWARN messages).
     */
    if (T_mode == DISCARD || (T_mode == NOPASS && ttype_result != TFAIL &&
                  ttype_result != TBROK
                  && ttype_result != TWARN))
        return;

    /*
     * Build the result line and print it.
     */
    type = strttype(ttype);

    if (T_mode == VERBOSE) {
        size += snprintf(message + size, sizeof(message) - size,
                "%-8s %4d  ", tcid, tnum);
    } else {
        size += snprintf(message + size, sizeof(message) - size,
                "%-8s %4d       ", tcid, tnum);
    }

    if (size >= sizeof(message)) {
        printf("%s: %i: line too long\n", __func__, __LINE__);
        abort();
    }

    if (tst_color_enabled(STDOUT_FILENO))
        size += snprintf(message + size, sizeof(message) - size,
        "%s%s%s  :  %s", tst_ttype2color(ttype), type, ANSI_COLOR_RESET, tmesg);
    else
        size += snprintf(message + size, sizeof(message) - size,
        "%s  :  %s", type, tmesg);

    if (size >= sizeof(message)) {
        printf("%s: %i: line too long\n", __func__, __LINE__);
        abort();
    }

    if (ttype & TERRNO) {
        size += snprintf(message + size, sizeof(message) - size,
                 ": errno=%s(%i): %s", tst_strerrno(err),
                 err, strerror(err));
    }

    if (size >= sizeof(message)) {
        printf("%s: %i: line too long\n", __func__, __LINE__);
        abort();
    }

    if (ttype & TTERRNO) {
        size += snprintf(message + size, sizeof(message) - size,
                 ": TEST_ERRNO=%s(%i): %s",
                 tst_strerrno(TEST_ERRNO), (int)TEST_ERRNO,
                 strerror(TEST_ERRNO));
    }

    if (size >= sizeof(message)) {
        printf("%s: %i: line too long\n", __func__, __LINE__);
        abort();
    }

    if (ttype & TRERRNO) {
        err = TEST_RETURN < 0 ? -(int)TEST_RETURN : (int)TEST_RETURN;
        size += snprintf(message + size, sizeof(message) - size,
                 ": TEST_RETURN=%s(%i): %s",
                 tst_strerrno(err), err, strerror(err));
    }

    if (size + 1 >= sizeof(message)) {
        printf("%s: %i: line too long\n", __func__, __LINE__);
        abort();
    }

    message[size] = '\n';
    message[size + 1] = '\0';

    assert(fputs(message, stdout) >= 0);
}

/* defines for unexpected signal setup routine (set_usig.c) */
#define FORK    1       /* SIGCHLD is to be ignored */
#define NOFORK  0       /* SIGCHLD is to be caught */
#define DEF_HANDLER SIG_ERR /* tells set_usig() to use default signal handler */

/*
 * The following defines are used to control tst_res and t_result reporting.
 */

#define TOUTPUT    "TOUTPUT"        /* The name of the environment variable */
                    /* that can be set to one of the following */
                    /* strings to control tst_res output */
                    /* If not set, TOUT_VERBOSE_S is assumed */

/* environment variables for controlling  tst_res verbosity */
#define TOUT_VERBOSE_S  "VERBOSE"   /* All test cases reported */
#define TOUT_NOPASS_S   "NOPASS"    /* No pass test cases are reported */
#define TOUT_DISCARD_S  "DISCARD"   /* No output is reported */

#define USC_ITERATION_ENV       "USC_ITERATIONS"
#define USC_LOOP_WALLTIME   "USC_LOOP_WALLTIME"
#define USC_NO_FUNC_CHECK   "USC_NO_FUNC_CHECK"
#define USC_LOOP_DELAY      "USC_LOOP_DELAY"

static void check_env(void)
{
    static int first_time = 1;
    char *value;

    if (!first_time)
        return;

    first_time = 0;

    /* BTOUTPUT not defined, use default */
    value = getenv(TOUTPUT);
    if (value == NULL) {
        T_mode = VERBOSE;
        return;
    }

    if (strcmp(value, TOUT_NOPASS_S) == 0) {
        T_mode = NOPASS;
        return;
    }

    if (strcmp(value, TOUT_DISCARD_S) == 0) {
        T_mode = DISCARD;
        return;
    }

    T_mode = VERBOSE;
}

static void tst_condense(int tnum, int ttype, const char *tmesg)
{
    int ttype_result = TTYPE_RESULT(ttype);

    /*
     * If this result is the same as the previous result, return.
     */
    if (Buffered == TRUE) {
        if (strcmp(Last_tcid, TCID) == 0 && Last_type == ttype_result &&
            strcmp(Last_mesg, tmesg) == 0)
            return;

        /*
         * This result is different from the previous result.  First,
         * print the previous result.
         */
        tst_print(Last_tcid, Last_num, Last_type, Last_mesg);
        free(Last_tcid);
        free(Last_mesg);
    }

    /*
     * If a file was specified, print the current result since we have no
     * way of retaining the file contents for comparing with future
     * results.  Otherwise, buffer the current result info for next time.
     */
    Last_tcid = malloc(strlen(TCID) + 1);
    strcpy(Last_tcid, TCID);
    Last_num = tnum;
    Last_type = ttype_result;
    Last_mesg = malloc(strlen(tmesg) + 1);
    strcpy(Last_mesg, tmesg);
    Buffered = TRUE;
}

static void tst_res__(const char *file, const int lineno, int ttype,
                      const char *arg_fmt, ...)
{
    pthread_mutex_lock(&tmutex);

    char tmesg[USERMESG];
    int len = 0;
    int ttype_result = TTYPE_RESULT(ttype);

    if (file && (ttype_result != TPASS && ttype_result != TINFO))
        len = sprintf(tmesg, "%s:%d: ", file, lineno);
    EXPAND_VAR_ARGS(tmesg + len, arg_fmt, USERMESG - len);

    /*
     * Save the test result type by ORing ttype into the current exit
     * value (used by tst_exit()).
     */
    T_exitval |= ttype_result;

    if (ttype_result == TPASS)
        passed_cnt++;

    check_env();

    /*
     * Set the test case number and print the results, depending on the
     * display type.
     */
    if (ttype_result == TWARN || ttype_result == TINFO) {
        tst_print(TCID, 0, ttype, tmesg);
    } else {
        if (tst_count < 0)
            tst_print(TCID, 0, TWARN,
                  "tst_res(): tst_count < 0 is not valid");

        /*
         * Process each display type.
         */
        switch (T_mode) {
        case DISCARD:
            break;
        case NOPASS:    /* filtered by tst_print() */
            tst_condense(tst_count + 1, ttype, tmesg);
            break;
        default:    /* VERBOSE */
            tst_print(TCID, tst_count + 1, ttype, tmesg);
            break;
        }

        tst_count++;
    }

    pthread_mutex_unlock(&tmutex);
}

/*
 * Make tst_brk reentrant so that one can call the SAFE_* macros from within
 * user-defined cleanup functions.
 */
static int tst_brk_entered = 0;

#define NO_NEWLIB_ASSERT(file, lineno)                                \
    if (tst_test) {                                               \
        tst_brk_(file, lineno, TBROK,                         \
             "%s() executed from newlib!", __FUNCTION__); \
    }

void tst_old_flush(void)
{
    NO_NEWLIB_ASSERT("Unknown", 0);

    pthread_mutex_lock(&tmutex);

    /*
     * Print out last line if in NOPASS mode.
     */
    if (Buffered == TRUE && T_mode == NOPASS) {
        tst_print(Last_tcid, Last_num, Last_type, Last_mesg);
        Buffered = FALSE;
    }

    assert(fflush(stdout) == 0);

    pthread_mutex_unlock(&tmutex);
}

void tst_exit(void)
{
    NO_NEWLIB_ASSERT("Unknown", 0);

    pthread_mutex_lock(&tmutex);

    tst_old_flush();

    T_exitval &= ~TINFO;

    if (T_exitval == TCONF && passed_cnt)
        T_exitval &= ~TCONF;

    exit(T_exitval);
}

static void tst_brk__(const char *file, const int lineno, int ttype,
                      void (*func)(void), const char *arg_fmt, ...)
{
    pthread_mutex_lock(&tmutex);

    char tmesg[USERMESG];
    int ttype_result = TTYPE_RESULT(ttype);

    EXPAND_VAR_ARGS(tmesg, arg_fmt, USERMESG);

    /*
     * Only FAIL, BROK, CONF, and RETR are supported by tst_brk().
     */
    if (ttype_result != TFAIL && ttype_result != TBROK &&
        ttype_result != TCONF) {
        assert(sprintf(Warn_mesg, "%s: Invalid Type: %d. Using TBROK",
                       __func__, ttype_result) == strlen(Warn_mesg));
        tst_print(TCID, 0, TWARN, Warn_mesg);
        /* Keep TERRNO, TTERRNO, etc. */
        ttype = (ttype & ~ttype_result) | TBROK;
    }

    tst_res__(file, lineno, ttype, "%s", tmesg);
    if (tst_brk_entered == 0) {
        if (ttype_result == TCONF) {
            tst_res__(file, lineno, ttype,
                "Remaining cases not appropriate for "
                "configuration");
        } else if (ttype_result == TBROK) {
            tst_res__(file, lineno, TBROK,
                 "Remaining cases broken");
        }
    }

    /*
     * If no cleanup function was specified, just return to the caller.
     * Otherwise call the specified function.
     */
    if (func != NULL) {
        tst_brk_entered++;
        (*func) ();
        tst_brk_entered--;
    }
    if (tst_brk_entered == 0)
        tst_exit();

    pthread_mutex_unlock(&tmutex);
}

void tst_brkm__(const char *file, const int lineno, int ttype,
    void (*func)(void), const char *arg_fmt, ...)
{
    char tmesg[USERMESG];

    EXPAND_VAR_ARGS(tmesg, arg_fmt, USERMESG);

    if (tst_test) {
        if (func) {
            tst_brk_(file, lineno, TBROK,
                     "Non-NULL cleanup in newlib!");
        }

        tst_brk_(file, lineno, ttype, "%s", tmesg);
    }

    tst_brk__(file, lineno, ttype, func, "%s", tmesg);

    /* Shouldn't be reached, but fixes build time warnings about noreturn. */
    abort();
}

long safe_strtol(const char *file, const int lineno,
         void (cleanup_fn) (void), char *str, long min, long max)
{
    long rval;
    char *endptr;

    errno = 0;
    rval = strtol(str, &endptr, 10);

    if ((errno == ERANGE && (rval == LONG_MAX || rval == LONG_MIN))
        || (errno != 0 && rval == 0)) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "strtol(%s) failed", str);
        return rval;
    }

    if (endptr == str || (*endptr != '\0' && *endptr != '\n')) {
        tst_brkm_(file, lineno, TBROK, cleanup_fn,
            "strtol(%s): Invalid value", str);
        return 0;
    }

    if (rval > max || rval < min) {
        tst_brkm_(file, lineno, TBROK, cleanup_fn,
            "strtol(%s): %ld is out of range %ld - %ld",
            str, rval, min, max);
        return 0;
    }

    return rval;
}

float safe_strtof(const char *file, const int lineno,
          void (cleanup_fn) (void), char *str,
          float min, float max)
{
    float rval;
    char *endptr;

    errno = 0;
    rval = strtof(str, &endptr);

    if (errno) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "strtof(%s) failed", str);
        return rval;
    }

    if (endptr == str || (*endptr != '\0' && *endptr != '\n')) {
        tst_brkm_(file, lineno, TBROK, cleanup_fn,
            "Invalid value: '%s'", str);
        return 0;
    }

    if (rval > max || rval < min) {
        tst_brkm_(file, lineno, TBROK, cleanup_fn,
            "strtof(%s): %f is out of range %f - %f",
            str, rval, min, max);
        return 0;
    }

    return rval;
}

static void parse_topt(unsigned int topts_len, int opt, char *optarg)
{
    unsigned int i;
    struct tst_option *toptions = tst_test->options;

    for (i = 0; i < topts_len; i++) {
        if (toptions[i].optstr[0] == opt)
            break;
    }

    if (i >= topts_len)
        tst_brk(TBROK, "Invalid option '%c' (should not happen)", opt);

    if (*toptions[i].arg)
        tst_res(TWARN, "Option -%c passed multiple times", opt);

    *(toptions[i].arg) = optarg ? optarg : "";
}

static void parse_opts(int argc, char *argv[])
{
    unsigned int i, topts_len = count_options();
    char optstr[2 * ARRAY_SIZE(options) + 2 * topts_len];
    int opt;

    check_option_collision();

    optstr[0] = 0;

    for (i = 0; i < ARRAY_SIZE(options); i++)
        strcat(optstr, options[i].optstr);

    for (i = 0; i < topts_len; i++)
        strcat(optstr, tst_test->options[i].optstr);

    while ((opt = getopt(argc, argv, optstr)) > 0) {
        switch (opt) {
        case '?':
            print_help();
            tst_brk(TBROK, "Invalid option");
        break;
        case 'h':
            print_help();
            print_test_tags();
            exit(0);
        case 'i':
            iterations = SAFE_STRTOL(optarg, 1, INT_MAX);
        break;
        case 'I':
            if (tst_test->max_runtime > 0)
                tst_test->max_runtime = SAFE_STRTOL(optarg, 1, INT_MAX);
            else
                duration = SAFE_STRTOF(optarg, 0.1, HUGE_VALF);
        break;
        case 'C':
#ifdef UCLINUX
            child_args = optarg;
#endif
        break;
        default:
            parse_topt(topts_len, opt, optarg);
        }
    }

    if (optind < argc)
        tst_brk(TBROK, "Unexpected argument(s) '%s'...", argv[optind]);
}

static char *TESTDIR = NULL;    /* the directory created */

static char test_start_work_dir[PATH_MAX];

int tst_tmpdir_created(void)
{
    return TESTDIR != NULL;
}

const char *tst_get_tmpdir_root(void)
{
    const char *env_tmpdir = getenv("TMPDIR");

    if (!env_tmpdir)
        env_tmpdir = TEMPDIR;

    if (env_tmpdir[0] != '/') {
        tst_brkm(TBROK, NULL, "You must specify an absolute "
                "pathname for environment variable TMPDIR");
        return NULL;
    }
    return env_tmpdir;
}

#define OLD_SAFE_MACROS_SAFE_STRTOL(cleanup_fn, str, min, max) \
    safe_strtol(__FILE__, __LINE__, cleanup_fn, (str), (min), (max))

#define OLD_SAFE_MACROS_SAFE_STRTOUL(cleanup_fn, str, min, max) \
    safe_strtoul(__FILE__, __LINE__, cleanup_fn, (str), (min), (max))

#define OLD_SAFE_MACROS_SAFE_SYSCONF(cleanup_fn, name) \
    safe_sysconf(__FILE__, __LINE__, cleanup_fn, name)

#define OLD_SAFE_MACROS_SAFE_CHMOD(cleanup_fn, path, mode) \
    safe_chmod(__FILE__, __LINE__, (cleanup_fn), (path), (mode))

#define OLD_SAFE_MACROS_SAFE_FCHMOD(cleanup_fn, fd, mode) \
    safe_fchmod(__FILE__, __LINE__, (cleanup_fn), (fd), (mode))

#define OLD_SAFE_MACROS_SAFE_CHOWN(cleanup_fn, path, owner, group) \
    safe_chown(__FILE__, __LINE__, (cleanup_fn), (path), (owner), (group))

#define OLD_SAFE_MACROS_SAFE_FCHOWN(cleanup_fn, fd, owner, group) \
    safe_fchown(__FILE__, __LINE__, (cleanup_fn), (fd), (owner), (group))

#define OLD_SAFE_MACROS_SAFE_WAIT(cleanup_fn, status) \
        safe_wait(__FILE__, __LINE__, (cleanup_fn), (status))

#define OLD_SAFE_MACROS_SAFE_WAITPID(cleanup_fn, pid, status, opts) \
        safe_waitpid(__FILE__, __LINE__, (cleanup_fn), (pid), (status), (opts))

#define OLD_SAFE_MACROS_SAFE_KILL(cleanup_fn, pid, sig) \
    safe_kill(__FILE__, __LINE__, (cleanup_fn), (pid), (sig))

#define OLD_SAFE_MACROS_SAFE_MEMALIGN(cleanup_fn, alignment, size) \
    safe_memalign(__FILE__, __LINE__, (cleanup_fn), (alignment), (size))

#define OLD_SAFE_MACROS_SAFE_MKFIFO(cleanup_fn, pathname, mode) \
    safe_mkfifo(__FILE__, __LINE__, (cleanup_fn), (pathname), (mode))

#define OLD_SAFE_MACROS_SAFE_RENAME(cleanup_fn, oldpath, newpath) \
    safe_rename(__FILE__, __LINE__, (cleanup_fn), (oldpath), (newpath))

#define SAFE_MOUNT(cleanup_fn, source, target, filesystemtype, \
           mountflags, data) \
    safe_mount(__FILE__, __LINE__, (cleanup_fn), (source), (target), \
           (filesystemtype), (mountflags), (data))

#define SAFE_UMOUNT(cleanup_fn, target) \
    safe_umount(__FILE__, __LINE__, (cleanup_fn), (target))

int safe_chown(const char *file, const int lineno, void (cleanup_fn)(void),
            const char *path, uid_t owner, gid_t group)
{
    int rval;

    rval = chown(path, owner, group);

    if (rval == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "chown(%s,%d,%d) failed", path, owner, group);
    } else if (rval) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid chown(%s,%d,%d) return value %d", path,
            owner, group, rval);
    }

    return rval;
}

int safe_chmod(const char *file, const int lineno,
               void (cleanup_fn)(void), const char *path, mode_t mode)
{
    int rval;

    rval = chmod(path, mode);

    if (rval == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "chmod(%s,%04o) failed", path, mode);
    } else if (rval) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid chmod(%s,%04o) return value %d", path, mode,
            rval);
    }

    return rval;
}

/*
 * Define some useful macros.
 */
#define DIR_MODE    (S_IRWXU|S_IRWXG|S_IRWXO)

void tst_resm_(const char *file, int lineno, int ttype,
    const char *arg_fmt, ...)
    __attribute__ ((format (printf, 4, 5)));
#define tst_resm(ttype, arg_fmt, ...) \
    tst_resm_(__FILE__, __LINE__, (ttype), \
          (arg_fmt), ##__VA_ARGS__)

void tst_resm_hexd_(const char *file, int lineno, int ttype,
    const void *buf, size_t size, const char *arg_fmt, ...)
    __attribute__ ((format (printf, 6, 7)));
#define tst_resm_hexd(ttype, buf, size, arg_fmt, ...) \
    tst_resm_hexd_(__FILE__, __LINE__, (ttype), (buf), (size), \
               (arg_fmt), ##__VA_ARGS__)

static int rmobj(const char *obj, char **errmsg);

static int purge_dir(const char *path, char **errptr)
{
    int ret_val = 0;
    DIR *dir;
    struct dirent *dir_ent;
    char dirobj[PATH_MAX];
    static char err_msg[PATH_MAX + 1280];

    /* Do NOT perform the request if the directory is "/" */
    if (!strcmp(path, "/")) {
        if (errptr) {
            strcpy(err_msg, "Cannot purge system root directory");
            *errptr = err_msg;
        }

        return -1;
    }

    errno = 0;

    /* Open the directory to get access to what is in it */
    dir = opendir(path);
    if (!dir) {
        if (errptr) {
            assert(sprintf(err_msg,
                           "Cannot open directory %s; errno=%d: %s",
                           path, errno, tst_strerrno(errno)) == strlen(err_msg));
            *errptr = err_msg;
        }
        return -1;
    }

    /* Loop through the entries in the directory, removing each one */
    for (dir_ent = readdir(dir); dir_ent; dir_ent = readdir(dir)) {
        /* Don't remove "." or ".." */
        if (!strcmp(dir_ent->d_name, ".")
            || !strcmp(dir_ent->d_name, ".."))
            continue;

        /* Recursively remove the current entry */
        assert(sprintf(dirobj, "%s/%s", path, dir_ent->d_name) == strlen(path) + 1 + strlen(dir_ent->d_name));
        if (rmobj(dirobj, errptr) != 0)
            ret_val = -1;
    }

    closedir(dir);
    return ret_val;
}

static int rmobj(const char *obj, char **errmsg)
{
    int ret_val = 0;
    struct stat statbuf;
    static char err_msg[PATH_MAX + 1280];
    int fd;

    fd = open(obj, O_DIRECTORY | O_NOFOLLOW);
    if (fd >= 0) {
        close(fd);
        ret_val = purge_dir(obj, errmsg);

        /* If there were problems removing an entry, don't attempt to
           remove the directory itself */
        if (ret_val == -1)
            return -1;

        /* Get the link count, now that all the entries have been removed */
        if (lstat(obj, &statbuf) < 0) {
            if (errmsg != NULL) {
                assert(sprintf(err_msg,
                               "lstat(%s) failed; errno=%d: %s", obj,
                               errno, tst_strerrno(errno)) == strlen(err_msg));
                *errmsg = err_msg;
            }
            return -1;
        }

        /* Remove the directory itself */
        if (statbuf.st_nlink >= 3) {
            /* The directory is linked; unlink() must be used */
            if (unlink(obj) < 0) {
                if (errmsg != NULL) {
                    assert(sprintf(err_msg,
                                   "unlink(%s) failed; errno=%d: %s",
                                   obj, errno, tst_strerrno(errno)) == strlen(err_msg));
                    *errmsg = err_msg;
                }
                return -1;
            }
        } else {
            /* The directory is not linked; remove() can be used */
            if (remove(obj) < 0) {
                if (errmsg != NULL) {
                    assert(sprintf(err_msg,
                                   "remove(%s) failed; errno=%d: %s",
                                   obj, errno, tst_strerrno(errno)) == strlen(err_msg));
                    *errmsg = err_msg;
                }
                return -1;
            }
        }
    } else {
        if (unlink(obj) < 0) {
            if (errmsg != NULL) {
                assert(sprintf(err_msg,
                               "unlink(%s) failed; errno=%d: %s", obj,
                               errno, tst_strerrno(errno)) == strlen(err_msg));
                *errmsg = err_msg;
            }
            return -1;
        }
    }

    return 0;
}

void tst_tmpdir(void)
{
    char template[PATH_MAX];
    const char *env_tmpdir;
    char *errmsg;

    /*
     * Create a template for the temporary directory.  Use the
     * environment variable TMPDIR if it is available, otherwise
     * use our default TEMPDIR.
     */
    env_tmpdir = tst_get_tmpdir_root();
    assert(snprintf(template, PATH_MAX, "%s/%.3sXXXXXX", env_tmpdir, TCID) == strlen(template));

    /* Make the temporary directory in one shot using mkdtemp. */
    if (mkdtemp(template) == NULL) {
        tst_brkm(TBROK | TERRNO, NULL,
             "%s: mkdtemp(%s) failed", __func__, template);
        return;
    }

    TESTDIR = strdup(template);
    if (TESTDIR == NULL) {
        tst_brkm(TBROK | TERRNO, NULL,
             "%s: strdup(%s) failed", __func__, template);
        return;
    }

    OLD_SAFE_MACROS_SAFE_CHOWN(NULL, TESTDIR, -1, getgid());

    OLD_SAFE_MACROS_SAFE_CHMOD(NULL, TESTDIR, DIR_MODE);

    if (getcwd(test_start_work_dir, sizeof(test_start_work_dir)) == NULL) {
        tst_resm(TINFO, "Failed to record test working dir");
        test_start_work_dir[0] = '\0';
    }

    /*
     * Change to the temporary directory.  If the chdir() fails, issue
     * TBROK messages for all test cases, attempt to remove the
     * directory (if it was created), and exit.  If the removal also
     * fails, also issue a TWARN message.
     */
    if (chdir(TESTDIR) == -1) {
        tst_resm(TERRNO, "%s: chdir(%s) failed", __func__, TESTDIR);

        /* Try to remove the directory */
        if (rmobj(TESTDIR, &errmsg) == -1) {
            tst_resm(TWARN, "%s: rmobj(%s) failed: %s",
                 __func__, TESTDIR, errmsg);
        }

        tst_exit();
    }
}

char *tst_get_tmpdir(void)
{
    char *ret = NULL;

    if (TESTDIR == NULL) {
        tst_brkm(TBROK, NULL, "you must call tst_tmpdir() first");
        return NULL;
    }

    ret = strdup(TESTDIR);
    if (!ret)
        tst_brkm(TBROK, NULL, "strdup() failed");

    return ret;
}

static inline int safe_ftruncate(const char *file, const int lineno,
                                 int fd, off_t length)
{
    int rval;

    rval = ftruncate(fd, length);

    if (rval == -1) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "ftruncate(%d,%ld) failed", fd, (long)length);
    } else if (rval) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "Invalid ftruncate(%d,%ld) return value %d", fd,
            (long)length, rval);
    }

    return rval;
}
#define SAFE_FTRUNCATE(fd, length) \
    safe_ftruncate(__FILE__, __LINE__, (fd), (length))

/*
 * following functions are inline because the behaviour may depend on
 * -D_FILE_OFFSET_BITS=64 compile flag
 */

static inline void *old_safe_macros_safe_mmap(const char *file, const int lineno,
    void (*cleanup_fn)(void), void *addr, size_t length,
    int prot, int flags, int fd, off_t offset)
{
    void *rval;

    rval = mmap(addr, length, prot, flags, fd, offset);
    if (rval == MAP_FAILED) {
        tst_brkm(TBROK | TERRNO, cleanup_fn,
             "%s:%d: mmap(%p,%zu,%d,%d,%d,%ld) failed",
             file, lineno, addr, length, prot, flags, fd,
             (long) offset);
    }

    return rval;
}
#define OLD_SAFE_MACROS_SAFE_MMAP(cleanup_fn, addr, length, prot, flags, fd, offset) \
    old_safe_macros_safe_mmap(__FILE__, __LINE__, (cleanup_fn), (addr), (length), (prot), \
    (flags), (fd), (offset))

/*
 * following functions are inline because the behaviour may depend on
 * -D_FILE_OFFSET_BITS=64 compile flag
 */

static inline void *safe_mmap(const char *file, const int lineno,
                              void *addr, size_t length,
                              int prot, int flags, int fd, off_t offset)
{
    void *rval;

    rval = mmap(addr, length, prot, flags, fd, offset);
    if (rval == MAP_FAILED) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "mmap(%p,%zu,%d,%d,%d,%ld) failed",
            addr, length, prot, flags, fd, (long) offset);
    }

    return rval;
}
#define SAFE_MMAP(addr, length, prot, flags, fd, offset) \
    safe_mmap(__FILE__, __LINE__, (addr), (length), (prot), \
    (flags), (fd), (offset))

#define IPC_ENV_VAR "LTP_IPC_PATH"

#define SAFE_GETGROUPS(size, list) \
    safe_getgroups(__FILE__, __LINE__, (size), (list))

#define SAFE_UNLINK(pathname) \
    safe_unlink(__FILE__, __LINE__, NULL, (pathname))

#define SAFE_LINK(oldpath, newpath) \
    safe_link(__FILE__, __LINE__, NULL, (oldpath), (newpath))

#define SAFE_LINKAT(olddirfd, oldpath, newdirfd, newpath, flags) \
    safe_linkat(__FILE__, __LINE__, NULL, (olddirfd), (oldpath), \
            (newdirfd), (newpath), (flags))

#define SAFE_READLINK(path, buf, bufsize) \
    safe_readlink(__FILE__, __LINE__, NULL, (path), (buf), (bufsize))

#define SAFE_SYMLINK(oldpath, newpath) \
    safe_symlink(__FILE__, __LINE__, NULL, (oldpath), (newpath))

#define SAFE_WRITE(len_strict, fildes, buf, nbyte) \
    safe_write(__FILE__, __LINE__, NULL, (len_strict), (fildes), (buf), (nbyte))

#define SAFE_STRTOL(str, min, max) \
    safe_strtol(__FILE__, __LINE__, NULL, (str), (min), (max))

#define SAFE_STRTOUL(str, min, max) \
    safe_strtoul(__FILE__, __LINE__, NULL, (str), (min), (max))

#define SAFE_STRTOF(str, min, max) \
    safe_strtof(__FILE__, __LINE__, NULL, (str), (min), (max))

#define SAFE_SYSCONF(name) \
    safe_sysconf(__FILE__, __LINE__, NULL, name)

#define SAFE_CHMOD(path, mode) \
    safe_chmod(__FILE__, __LINE__, NULL, (path), (mode))

#define SAFE_FCHMOD(fd, mode) \
    safe_fchmod(__FILE__, __LINE__, NULL, (fd), (mode))

#define SAFE_CHOWN(path, owner, group) \
    safe_chown(__FILE__, __LINE__, NULL, (path), (owner), (group))

#define SAFE_FCHOWN(fd, owner, group) \
    safe_fchown(__FILE__, __LINE__, NULL, (fd), (owner), (group))

#define SAFE_WAIT(status) \
    safe_wait(__FILE__, __LINE__, NULL, (status))

int safe_unlink(const char *file, const int lineno, void (*cleanup_fn) (void),
                const char *pathname)
{
    int rval;

    rval = unlink(pathname);

    if (rval == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "unlink(%s) failed", pathname);
    } else if (rval) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid unlink(%s) return value %d", pathname, rval);
    }

    return rval;
}

#define SAFE_ACCESS(path, mode) \
    safe_access(__FILE__, __LINE__, (path), (mode))

#define SAFE_BASENAME(path) \
    safe_basename(__FILE__, __LINE__, NULL, (path))

#define SAFE_CHDIR(path) \
    safe_chdir(__FILE__, __LINE__, NULL, (path))

#define SAFE_CLOSE(fd) do { \
        safe_close(__FILE__, __LINE__, NULL, (fd)); \
        fd = -1; \
    } while (0)

#define SAFE_CREAT(pathname, mode) \
    safe_creat(__FILE__, __LINE__, NULL, (pathname), (mode))

#define SAFE_CHROOT(path) \
    safe_chroot(__FILE__, __LINE__, (path))

#define SAFE_DIRNAME(path) \
    safe_dirname(__FILE__, __LINE__, NULL, (path))

#define SAFE_DUP(oldfd) \
    safe_dup(__FILE__, __LINE__, (oldfd))

#define SAFE_DUP2(oldfd, newfd)         \
    safe_dup2(__FILE__, __LINE__, (oldfd), (newfd))

#define SAFE_GETCWD(buf, size) \
    safe_getcwd(__FILE__, __LINE__, NULL, (buf), (size))

#define SAFE_GETPWNAM(name) \
    safe_getpwnam(__FILE__, __LINE__, NULL, (name))

#define SAFE_GETRUSAGE(who, usage) \
    safe_getrusage(__FILE__, __LINE__, NULL, (who), (usage))

#define SAFE_MALLOC(size) \
    safe_malloc(__FILE__, __LINE__, NULL, (size))

int
safe_close(const char *file, const int lineno, void (*cleanup_fn) (void),
       int fildes)
{
    int rval;

    rval = close(fildes);

    if (rval == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "close(%d) failed", fildes);
    } else if (rval) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid close(%d) return value %d", fildes, rval);
    }

    return rval;
}

typedef volatile uint32_t futex_t;

#define DEFAULT_MSEC_TIMEOUT 10000

futex_t *tst_futexes;
unsigned int tst_max_futexes;

static void setup_ipc(void)
{
    size_t size = getpagesize();

    if (access("/dev/shm", F_OK) == 0) {
        assert(snprintf(shm_path, sizeof(shm_path), "/dev/shm/ltp_%s_%d",
                        tid, getpid()) == strlen(shm_path));
    } else {
        char *tmpdir;

        if (!tst_tmpdir_created())
            tst_tmpdir();

        tmpdir = tst_get_tmpdir();
        assert(snprintf(shm_path, sizeof(shm_path), "%s/ltp_%s_%d",
                        tmpdir, tid, getpid()) == strlen(shm_path));
        free(tmpdir);
    }

    ipc_fd = open(shm_path, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (ipc_fd < 0)
        tst_brk(TBROK | TERRNO, "open(%s)", shm_path);
    SAFE_CHMOD(shm_path, 0666);

    SAFE_FTRUNCATE(ipc_fd, size);

    results = SAFE_MMAP(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, ipc_fd, 0);

    /* Checkpoints needs to be accessible from processes started by exec() */
    if (tst_test->needs_checkpoints || tst_test->child_needs_reinit) {
        assert(sprintf(ipc_path, IPC_ENV_VAR "=%s", shm_path) == strlen(IPC_ENV_VAR) + 1 + strlen(shm_path));
        assert(putenv(ipc_path) == 0);
    } else {
        SAFE_UNLINK(shm_path);
    }

    SAFE_CLOSE(ipc_fd);

    if (tst_test->needs_checkpoints) {
        tst_futexes = (volatile futex_t *)((char*)results + sizeof(struct results));
        tst_max_futexes = (size - sizeof(struct results))/sizeof(futex_t);
    }
}

static int needs_tmpdir(void)
{
    return tst_test->needs_tmpdir ||
           tst_test->needs_device ||
           tst_test->mntpoint ||
           tst_test->resource_files ||
           tst_test->needs_checkpoints;
}

static void do_setup(int argc, char *argv[])
{
    if (!tst_test)
        tst_brk(TBROK, "No tests to run");

    if (tst_test->max_runtime < -1) {
        tst_brk(TBROK, "Invalid runtime value %i",
            results->max_runtime);
    }

    if (tst_test->tconf_msg)
        tst_brk(TCONF, "%s", tst_test->tconf_msg);

    assert_test_fn();

    TCID = tid = get_tid(argv);

    assert(!tst_test->sample); /*if (tst_test->sample)
                                 tst_test = tst_timer_test_setup(tst_test);*/

    parse_opts(argc, argv);

    assert(!tst_test->needs_kconfigs);
    /*if (tst_test->needs_kconfigs && tst_kconfig_check(tst_test->needs_kconfigs))
        tst_brk(TCONF, "Aborting due to unsuitable kernel config, see above!");*/

    if (tst_test->needs_root && geteuid() != 0)
        tst_brk(TCONF, "Test needs to be run as root");

    assert(!tst_test->min_kver);
    /*if (tst_test->min_kver)
        check_kver();*/

    assert(!tst_test->supported_archs);
    /*if (tst_test->supported_archs && !tst_is_on_arch(tst_test->supported_archs))
        tst_brk(TCONF, "This arch '%s' is not supported for test!", tst_arch.name);*/

    assert(!tst_test->skip_in_lockdown);
    /*if (tst_test->skip_in_lockdown && tst_lockdown_enabled())
        tst_brk(TCONF, "Kernel is locked down, skipping test");*/

    assert(!tst_test->skip_in_compat);
    /*if (tst_test->skip_in_compat && TST_ABI != tst_kernel_bits())
        tst_brk(TCONF, "Not supported in 32-bit compat mode");*/

    assert(!tst_test->needs_cmds);
    /*if (tst_test->needs_cmds) {
        const char *cmd;
        int i;

        for (i = 0; (cmd = tst_test->needs_cmds[i]); ++i)
            tst_check_cmd(cmd);
    }*/

    assert(!tst_test->needs_drivers);
    /*if (tst_test->needs_drivers) {
        const char *name;
        int i;

        for (i = 0; (name = tst_test->needs_drivers[i]); ++i)
            if (tst_check_driver(name))
                tst_brk(TCONF, "%s driver not available", name);
    }*/

    if (tst_test->mount_device)
        tst_test->format_device = 1;

    if (tst_test->format_device)
        tst_test->needs_device = 1;

    if (tst_test->all_filesystems)
        tst_test->needs_device = 1;

    assert(!tst_test->min_cpus);
    /*if (tst_test->min_cpus > (unsigned long)tst_ncpus())
        tst_brk(TCONF, "Test needs at least %lu CPUs online", tst_test->min_cpus);*/

    assert(!tst_test->min_mem_avail);
    /*if (tst_test->min_mem_avail > (unsigned long)(tst_available_mem() / 1024))
        tst_brk(TCONF, "Test needs at least %luMB MemAvailable", tst_test->min_mem_avail);*/

    assert(!tst_test->hugepages.number);
    /*if (tst_test->hugepages.number)
        tst_reserve_hugepages(&tst_test->hugepages);*/

    setup_ipc();

    assert(!tst_test->bufs);
    /*if (tst_test->bufs)
        tst_buffers_alloc(tst_test->bufs);*/

    if (needs_tmpdir() && !tst_tmpdir_created())
        tst_tmpdir();

    assert(!tst_test->save_restore);
    /*if (tst_test->save_restore) {
        const struct tst_path_val *pvl = tst_test->save_restore;

        while (pvl->path) {
            tst_sys_conf_save(pvl);
            pvl++;
        }
     }*/

    assert(!tst_test->mntpoint);
    /*if (tst_test->mntpoint)
        SAFE_MKDIR(tst_test->mntpoint, 0777);*/

    if ((tst_test->needs_devfs || tst_test->needs_rofs ||
         tst_test->mount_device || tst_test->all_filesystems ||
         tst_test->needs_hugetlbfs) &&
         !tst_test->mntpoint) {
        tst_brk(TBROK, "tst_test->mntpoint must be set!");
    }

    if (!!tst_test->needs_rofs + !!tst_test->needs_devfs +
        !!tst_test->needs_device + !!tst_test->needs_hugetlbfs > 1) {
        tst_brk(TBROK,
            "Two or more of needs_{rofs, devfs, device, hugetlbfs} are set");
    }

    assert(!tst_test->needs_devfs);
    /*if (tst_test->needs_devfs)
        prepare_and_mount_dev_fs(tst_test->mntpoint);*/

    assert(!tst_test->needs_rofs);
    /*if (tst_test->needs_rofs) {
        // If we failed to mount read-only tmpfs. Fallback to
        // using a device with read-only filesystem.
        if (prepare_and_mount_ro_fs(NULL, tst_test->mntpoint, "tmpfs")) {
            tst_res(TINFO, "Can't mount tmpfs read-only, "
                    "falling back to block device...");
            tst_test->needs_device = 1;
            tst_test->format_device = 1;
        }
    }*/

    assert(!tst_test->needs_hugetlbfs);
    /*if (tst_test->needs_hugetlbfs)
        prepare_and_mount_hugetlb_fs();*/

    assert(!(tst_test->needs_device && !mntpoint_mounted));
    /*if (tst_test->needs_device && !mntpoint_mounted) {
        tdev.dev = tst_acquire_device_(NULL, tst_test->dev_min_size);

        if (!tdev.dev)
            tst_brk(TCONF, "Failed to acquire device");

        tdev.size = tst_get_device_size(tdev.dev);

        tst_device = &tdev;

        if (tst_test->dev_fs_type)
            tdev.fs_type = tst_test->dev_fs_type;
        else
            tdev.fs_type = tst_dev_fs_type();

        if (!tst_test->all_filesystems)
            prepare_device();
    }*/

    if (tst_test->needs_overlay && !tst_test->mount_device) {
        tst_brk(TBROK, "tst_test->mount_device must be set");
    }
    if (tst_test->needs_overlay && !mntpoint_mounted) {
        tst_brk(TBROK, "tst_test->mntpoint must be mounted");
    }
    assert(!(tst_test->needs_overlay && !ovl_mounted));
    /*if (tst_test->needs_overlay && !ovl_mounted) {
        SAFE_MOUNT_OVERLAY();
        ovl_mounted = 1;
    }*/

    assert(!tst_test->resource_files);
    /*if (tst_test->resource_files)
        copy_resources();*/

    assert(!tst_test->restore_wallclock);
    /*if (tst_test->restore_wallclock)
        tst_wallclock_save();*/

    assert(!tst_test->taint_check);
    /*if (tst_test->taint_check)
        tst_taint_init(tst_test->taint_check);*/

    assert(!tst_test->needs_cgroup_ctrls && !tst_test->needs_cgroup_ver);
    /*if (tst_test->needs_cgroup_ctrls)
        do_cgroup_requires();
    else if (tst_test->needs_cgroup_ver)
        tst_brk(TBROK, "tst_test->needs_cgroup_ctrls must be set");*/
}

void tst_enable_oom_protection(pid_t pid)
{
    // Guess I'll see about that if I ever need it
    //set_oom_score_adj(pid, -1000);
}

void tst_disable_oom_protection(pid_t pid)
{
    // Guess I'll see about that if I ever need it
    //set_oom_score_adj(pid, 0);
}


#define SAFE_SIGNAL(signum, handler) \
    safe_signal(__FILE__, __LINE__, (signum), (handler))

sighandler_t safe_signal(const char *file, const int lineno,
    int signum, sighandler_t handler)
{
    sighandler_t rval;

    rval = signal(signum, handler);

    if (rval == SIG_ERR) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "signal(%d,%p) failed",
            signum, handler);
    }

    return rval;
}

#define LTP_ATTRIBUTE_UNUSED YALIBCT_ATTRIBUTE_UNUSED

static pid_t test_pid;


static volatile sig_atomic_t sigkill_retries;

#define WRITE_MSG(msg) do { \
    if (write(2, msg, sizeof(msg) - 1)) { \
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66425 */ \
    } \
} while (0)

static void alarm_handler(int sig LTP_ATTRIBUTE_UNUSED)
{
    WRITE_MSG("Test timeouted, sending SIGKILL!\n");
    kill(-test_pid, SIGKILL);
    alarm(5);

    if (++sigkill_retries > 10) {
        WRITE_MSG("Cannot kill test processes!\n");
        WRITE_MSG("Congratulation, likely test hit a kernel bug.\n");
        WRITE_MSG("Exiting uncleanly...\n");
        _exit(TFAIL);
    }
}

static void heartbeat_handler(int sig LTP_ATTRIBUTE_UNUSED)
{
    alarm(results->timeout);
    sigkill_retries = 0;
}

#define TST_NO_HUGEPAGES ((unsigned long)-1)

#define TST_UNLIMITED_RUNTIME (-1)

static void set_timeout(void)
{
    unsigned int timeout = DEFAULT_TIMEOUT;

    if (results->max_runtime == TST_UNLIMITED_RUNTIME) {
        tst_res(TINFO, "Timeout per run is disabled");
        return;
    }

    if (results->max_runtime < 0) {
        tst_brk(TBROK, "max_runtime must to be >= -1! (%d)",
            results->max_runtime);
    }

    results->timeout = tst_multiply_timeout(timeout) + results->max_runtime;

    //tst_res(TINFO, "Timeout per run is %uh %02um %02us",
    //    results->timeout/3600, (results->timeout%3600)/60,
    //   results->timeout % 60);
}

unsigned int tst_variant;

static void sigint_handler(int sig LTP_ATTRIBUTE_UNUSED)
{
    if (test_pid > 0) {
        WRITE_MSG("Sending SIGKILL to test process...\n");
        kill(-test_pid, SIGKILL);
    }
}

#define SAFE_SETPGID(pid, pgid) \
    safe_setpgid(__FILE__, __LINE__, (pid), (pgid))

int safe_setpgid(const char *file, const int lineno, pid_t pid, pid_t pgid)
{
    int rval;

    rval = setpgid(pid, pgid);

    if (rval == -1) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "setpgid(%i, %i) failed", pid, pgid);
    } else if (rval) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "Invalid setpgid(%i, %i) return value %d", pid, pgid,
            rval);
    }

    return rval;
}

/*int tst_clock_gettime(clockid_t clk_id, struct timespec *ts)
{
    static struct tst_ts tts = { 0, };
    static mysyscall func;
    int ret;

#if (__NR_clock_gettime64 != __LTP__NR_INVALID_SYSCALL)
    if (!func && syscall_supported_by_kernel(__NR_clock_gettime64)) {
        func = sys_clock_gettime64;
        tts.type = TST_KERN_TIMESPEC;
    }
#endif

    if (!func && syscall_supported_by_kernel(__NR_clock_gettime)) {
        func = sys_clock_gettime;
        tts.type = TST_KERN_OLD_TIMESPEC;
    }

    if (!func) {
        tst_res(TCONF, "clock_gettime() not available");
        errno = ENOSYS;
        return -1;
    }

    ret = func(clk_id, tst_ts_get(&tts));
    ts->tv_sec = tst_ts_get_sec(tts);
    ts->tv_nsec = tst_ts_get_nsec(tts);
    return ret;
}*/

#define tst_clock_gettime clock_gettime

static struct timespec tst_start_time; /* valid only for test pid */

static void heartbeat(void)
{
    if (tst_clock_gettime(CLOCK_MONOTONIC, &tst_start_time))
        tst_res(TWARN | TERRNO, "tst_clock_gettime() failed");

    if (getppid() == 1) {
        tst_res(TFAIL, "Main test process might have exit!");
        /*
         * We need kill the task group immediately since the
         * main process has exit.
         */
        kill(0, SIGKILL);
        exit(TBROK);
    }

    kill(getppid(), SIGUSR1);
}

const char *tst_get_startwd(void)
{
    return test_start_work_dir;
}

#define SAFE_FOPEN(path, mode) \
    safe_fopen(__FILE__, __LINE__, NULL, path, mode)

#define SAFE_FCLOSE(f) \
    safe_fclose(__FILE__, __LINE__, NULL, f)

#define SAFE_ASPRINTF(strp, fmt, ...) \
    safe_asprintf(__FILE__, __LINE__, NULL, strp, fmt, __VA_ARGS__)

#define SAFE_POPEN(command, type) \
    safe_popen(__FILE__, __LINE__, NULL, command, type)

int safe_asprintf(const char *file, const int lineno, void (cleanup_fn)(void),
                  char **strp, const char *fmt, ...)
{
    int ret;
    va_list va;

    va_start(va, fmt);
    ret = vasprintf(strp, fmt, va);
    va_end(va);

    if (ret == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "asprintf(%s,...) failed", fmt);
    } else if (ret < 0) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid asprintf(%s,...) return value %d", fmt, ret);
    }

    return ret;
}

#define SAFE_PIDFD_OPEN(pid, flags) \
    safe_pidfd_open(__FILE__, __LINE__, (pid), (flags))

#define SAFE_SETENV(name, value, overwrite) do {        \
    if (setenv(name, value, overwrite)) {           \
        tst_brk_(__FILE__, __LINE__, TBROK | TERRNO,    \
            "setenv(%s, %s, %d) failed",        \
            name, value, overwrite);        \
    }                           \
    } while (0)

static void add_paths(void)
{
    char *old_path = getenv("PATH");
    const char *start_dir;
    char *new_path;

    start_dir = tst_get_startwd();

    if (old_path)
        SAFE_ASPRINTF(&new_path, "%s::%s", old_path, start_dir);
    else
        SAFE_ASPRINTF(&new_path, "::%s", start_dir);

    SAFE_SETENV("PATH", new_path, 1);
    free(new_path);
}

static void do_test_setup(void)
{
    main_pid = getpid();

    assert(!(!tst_test->all_filesystems && tst_test->skip_filesystems));
    /*if (!tst_test->all_filesystems && tst_test->skip_filesystems) {
        long fs_type = tst_fs_type(".");
        const char *fs_name = tst_fs_type_name(fs_type);

        if (tst_fs_in_skiplist(fs_name, tst_test->skip_filesystems)) {
            tst_brk(TCONF, "%s is not supported by the test",
                fs_name);
        }

        tst_res(TINFO, "%s is supported by the test", fs_name);
    }*/

    assert(!tst_test->caps);
    /*if (tst_test->caps)
        tst_cap_setup(tst_test->caps, TST_CAP_REQ);*/

    if (tst_test->setup)
        tst_test->setup();

    if (main_pid != getpid())
        tst_brk(TBROK, "Runaway child in setup()!");

    assert(!tst_test->caps);
    /*if (tst_test->caps)
        tst_cap_setup(tst_test->caps, TST_CAP_DROP);*/
}

enum tst_ts_type {
    TST_LIBC_TIMESPEC,
    TST_KERN_OLD_TIMESPEC,
    TST_KERN_TIMESPEC
};

struct tst_ts {
    enum tst_ts_type type;
    union ts {
        struct timespec libc_ts;
/*        struct __kernel_old_timespec kern_old_ts;
          struct __kernel_timespec kern_ts;*/
    } ts;
};

/*
 * Returns tst_ts seconds.
 */
static inline long long tst_ts_get_sec(struct tst_ts ts)
{
    switch (ts.type) {
    case TST_LIBC_TIMESPEC:
        return ts.ts.libc_ts.tv_sec;
/*    case TST_KERN_OLD_TIMESPEC:
        return ts.ts.kern_old_ts.tv_sec;
      case TST_KERN_TIMESPEC:
        return ts.ts.kern_ts.tv_sec;*/
    default:
        tst_brk(TBROK, "Invalid type: %d", ts.type);
        return -1;
    }
}

/*
 * Returns tst_ts nanoseconds.
 */
static inline long long tst_ts_get_nsec(struct tst_ts ts)
{
    switch (ts.type) {
    case TST_LIBC_TIMESPEC:
        return ts.ts.libc_ts.tv_nsec;
    /*case TST_KERN_OLD_TIMESPEC:
        return ts.ts.kern_old_ts.tv_nsec;
    case TST_KERN_TIMESPEC:
        return ts.ts.kern_ts.tv_nsec;*/
    default:
        tst_brk(TBROK, "Invalid type: %d", ts.type);
        return -1;
    }
}

/*
 * Converts tst_ts to milliseconds and rounds the value.
 */
static inline long long tst_ts_to_ms(struct tst_ts t)
{
    return tst_ts_get_sec(t) * 1000 +
           (tst_ts_get_nsec(t) + 500000) / 1000000;
}

/*
 * Converts timespec to tst_ts.
 */
static inline struct tst_ts tst_ts_from_timespec(struct timespec ts)
{
    struct tst_ts t = {
        .type = TST_LIBC_TIMESPEC,
        .ts.libc_ts.tv_sec = ts.tv_sec,
        .ts.libc_ts.tv_nsec = ts.tv_nsec,
    };

    return t;
}

/*
 * Converts timespec to milliseconds and rounds the value.
 */
static inline long long tst_timespec_to_ms(struct timespec ts)
{
    return tst_ts_to_ms(tst_ts_from_timespec(ts));
}

static unsigned long long get_time_ms(void)
{
    struct timespec ts;

    if (tst_clock_gettime(CLOCK_MONOTONIC, &ts))
        tst_brk(TBROK | TERRNO, "tst_clock_gettime()");

    return tst_timespec_to_ms(ts);
}

#define NON_CONST_PAIR(def) assert(ARRAY_SIZE(signal_pairs) > def);     \
    signal_pairs[def].name = #def;                                      \
    signal_pairs[def].val = def;
#define NON_CONST_STRPAIR(key, value) \
    assert(ARRAY_SIZE(signal_pairs) > key);                             \
    signal_pairs[key].name = value;                                     \
    signal_pairs[key].val = key;

const char *tst_strsig(int sig)
{
    static struct pair signal_pairs[2000];

        NON_CONST_PAIR(SIGHUP)
        NON_CONST_PAIR(SIGINT)
        NON_CONST_PAIR(SIGQUIT)
        NON_CONST_PAIR(SIGILL)
    #ifdef SIGTRAP
        NON_CONST_PAIR(SIGTRAP)
    #endif

    #ifdef SIGIOT
        /* SIGIOT same as SIGABRT */
        NON_CONST_STRPAIR(SIGABRT, "SIGIOT/SIGABRT")
    #else
        NON_CONST_PAIR(SIGABRT)
    #endif

    #ifdef SIGEMT
        NON_CONST_PAIR(SIGEMT)
    #endif
    #ifdef SIGBUS
        NON_CONST_PAIR(SIGBUS)
    #endif
        NON_CONST_PAIR(SIGFPE)
        NON_CONST_PAIR(SIGKILL)
        NON_CONST_PAIR(SIGUSR1)
        NON_CONST_PAIR(SIGSEGV)
        NON_CONST_PAIR(SIGUSR2)
        NON_CONST_PAIR(SIGPIPE)
        NON_CONST_PAIR(SIGALRM)
        NON_CONST_PAIR(SIGTERM)
    #ifdef SIGSTKFLT
        NON_CONST_PAIR(SIGSTKFLT)
    #endif
        NON_CONST_PAIR(SIGCHLD)
        NON_CONST_PAIR(SIGCONT)
        NON_CONST_PAIR(SIGSTOP)
        NON_CONST_PAIR(SIGTSTP)
        NON_CONST_PAIR(SIGTTIN)
        NON_CONST_PAIR(SIGTTOU)
    #ifdef SIGURG
        NON_CONST_PAIR(SIGURG)
    #endif
    #ifdef SIGXCPU
        NON_CONST_PAIR(SIGXCPU)
    #endif
    #ifdef SIGXFSZ
        NON_CONST_PAIR(SIGXFSZ)
    #endif
    #ifdef SIGVTALRM
        NON_CONST_PAIR(SIGVTALRM)
    #endif
    #ifdef SIGPROF
        NON_CONST_PAIR(SIGPROF)
    #endif
    #ifdef SIGWINCH
        NON_CONST_PAIR(SIGWINCH)
    #endif

    #if defined(SIGIO) && defined(SIGPOLL)
        /* SIGPOLL same as SIGIO */
        NON_CONST_STRPAIR(SIGIO, "SIGIO/SIGPOLL")
    #elif defined(SIGIO)
        NON_CONST_PAIR(SIGIO)
    #elif defined(SIGPOLL)
        NON_CONST_PAIR(SIGPOLL)
    #endif

    #ifdef SIGINFO
        NON_CONST_PAIR(SIGINFO)
    #endif
    #ifdef SIGLOST
        NON_CONST_PAIR(SIGLOST)
    #endif
    #ifdef SIGPWR
        NON_CONST_PAIR(SIGPWR)
    #endif
    #if defined(SIGSYS)
        /*
         * According to signal(7)'s manpage, SIGUNUSED is synonymous
         * with SIGSYS on most architectures.
         */
        NON_CONST_STRPAIR(SIGSYS, "SIGSYS/SIGUNUSED")
    #endif

    PAIR_LOOKUP(signal_pairs, sig);
};

static void check_child_status(pid_t pid, int status)
{
    int ret;

    if (WIFSIGNALED(status)) {
        tst_brk(TBROK, "Child (%i) killed by signal %s",
                pid, tst_strsig(WTERMSIG(status)));
    }

    if (!(WIFEXITED(status)))
        tst_brk(TBROK, "Child (%i) exited abnormally", pid);

    ret = WEXITSTATUS(status);
    switch (ret) {
    case TPASS:
    case TBROK:
    case TCONF:
    break;
    default:
        tst_brk(TBROK, "Invalid child (%i) exit value %i", pid, ret);
    }
}

void tst_reap_children(void)
{
    int status;
    pid_t pid;

    for (;;) {
        pid = wait(&status);

        if (pid > 0) {
            check_child_status(pid, status);
            continue;
        }

        if (errno == ECHILD)
            break;

        if (errno == EINTR)
            continue;

        tst_brk(TBROK | TERRNO, "wait() failed");
    }
}

static int results_equal(struct results *a, struct results *b)
{
    if (a->passed != b->passed)
        return 0;

    if (a->failed != b->failed)
        return 0;

    if (a->skipped != b->skipped)
        return 0;

    if (a->broken != b->broken)
        return 0;

    return 1;
}

static void run_tests(void)
{
    unsigned int i;
    struct results saved_results;

    if (!tst_test->test) {
        saved_results = *results;
        heartbeat();
        tst_test->test_all();

        if (getpid() != main_pid) {
            exit(0);
        }

        tst_reap_children();

        if (results_equal(&saved_results, results))
            tst_brk(TBROK, "Test haven't reported results!");
        return;
    }

    for (i = 0; i < tst_test->tcnt; i++) {
        saved_results = *results;
        heartbeat();
        tst_test->test(i);

        if (getpid() != main_pid) {
            exit(0);
        }

        tst_reap_children();

        if (results_equal(&saved_results, results))
            tst_brk(TBROK, "Test %i haven't reported results!", i);
    }
}

void tst_vbrk_(const char *file, int lineno, int ttype,
               const char *fmt, va_list va);

static void (*tst_brk_handler)(const char *file, const int lineno, int ttype,
                   const char *fmt, va_list va) = tst_vbrk_;

static void print_result(const char *file, const int lineno, int ttype,
                         const char *fmt, va_list va)
{
    char buf[1024];
    char *str = buf;
    int ret, size = sizeof(buf), ssize, int_errno, buflen;
    const char *str_errno = NULL;
    const char *res;

    switch (TTYPE_RESULT(ttype)) {
    case TPASS:
        res = "TPASS";
    break;
    case TFAIL:
        res = "TFAIL";
    break;
    case TBROK:
        res = "TBROK";
    break;
    case TCONF:
        res = "TCONF";
    break;
    case TWARN:
        res = "TWARN";
    break;
    case TINFO:
        res = "TINFO";
    break;
    default:
        tst_brk(TBROK, "Invalid ttype value %i", ttype);
        abort();
    }

    if (ttype & TERRNO) {
        str_errno = tst_strerrno(errno);
        int_errno = errno;
    }

    if (ttype & TTERRNO) {
        str_errno = tst_strerrno(TST_ERR);
        int_errno = TST_ERR;
    }

    if (ttype & TRERRNO) {
        int_errno = TST_RET < 0 ? -(int)TST_RET : (int)TST_RET;
        str_errno = tst_strerrno(int_errno);
    }

    ret = snprintf(str, size, "%s:%i: ", file, lineno);
    str += ret;
    size -= ret;

    if (tst_color_enabled(STDERR_FILENO))
        ret = snprintf(str, size, "%s%s: %s", tst_ttype2color(ttype),
                   res, ANSI_COLOR_RESET);
    else
        ret = snprintf(str, size, "%s: ", res);
    str += ret;
    size -= ret;

    ssize = size - 2;
    ret = vsnprintf(str, size, fmt, va);
    str += MIN(ret, ssize);
    size -= MIN(ret, ssize);
    if (ret >= ssize) {
        tst_res_(file, lineno, TWARN,
                "Next message is too long and truncated:");
    } else if (str_errno) {
        ssize = size - 2;
        ret = snprintf(str, size, ": %s (%d)", str_errno, int_errno);
        str += MIN(ret, ssize);
        size -= MIN(ret, ssize);
        if (ret >= ssize)
            tst_res_(file, lineno, TWARN,
                "Next message is too long and truncated:");
    }

    assert(snprintf(str, size, "\n") == 1);

    if (TTYPE_RESULT(ttype) == TPASS)
        return;

    /* we might be called from signal handler, so use write() */
    buflen = str - buf + 1;
    str = buf;
    while (buflen) {
        ret = write(STDERR_FILENO, str, buflen);
        if (ret <= 0)
            break;

        str += ret;
        buflen -= ret;
    }
}

static inline int tst_atomic_add_return(int i, int *v) // NOLINT(readability-non-const-parameter)
{
    return __atomic_add_fetch(v, i, __ATOMIC_SEQ_CST);
}

static inline int tst_atomic_inc(int *v)
{
    return tst_atomic_add_return(1, v);
}

static void update_results(int ttype)
{
    if (!results)
        return;

    switch (ttype) {
    case TCONF:
        tst_atomic_inc(&results->skipped);
    break;
    case TPASS:
        tst_atomic_inc(&results->passed);
    break;
    case TWARN:
        tst_atomic_inc(&results->warnings);
    break;
    case TFAIL:
        tst_atomic_inc(&results->failed);
    break;
    case TBROK:
        tst_atomic_inc(&results->broken);
    break;
    }
}

static void tst_cvres(const char *file, const int lineno, int ttype,
              const char *fmt, va_list va)
{
    if (TTYPE_RESULT(ttype) == TBROK) {
        ttype &= ~TTYPE_MASK;
        ttype |= TWARN;
    }

    print_result(file, lineno, ttype, fmt, va);
    update_results(TTYPE_RESULT(ttype));
}

static struct map *maps;

void tst_free_all(void)
{
    struct map *i = maps;

    assert(!i);
    /*while (i) {
        struct map *j = i;
        check_canary(i);
        SAFE_MUNMAP(i->addr, i->size);
        i = i->next;
        free(j);
    }*/

    maps = NULL;
}

static void do_test_cleanup(void)
{
    tst_brk_handler = tst_cvres;

    if (tst_test->cleanup)
        tst_test->cleanup();

    tst_free_all();

    tst_brk_handler = tst_vbrk_;
}

static void testrun(void)
{
    unsigned int i = 0;
    unsigned long long stop_time = 0;
    int cont = 1;

    heartbeat();
    add_paths();
    do_test_setup();

    if (duration > 0)
        stop_time = get_time_ms() + (unsigned long long)(duration * 1000);

    for (;;) {
        cont = 0;

        if (i < (unsigned int)iterations) {
            i++;
            cont = 1;
        }

        if (stop_time && get_time_ms() < stop_time)
            cont = 1;

        if (!cont)
            break;

        run_tests();
        heartbeat();
    }

    do_test_cleanup();
    exit(0);
}

pid_t safe_waitpid(const char *file, const int lineno, void (cleanup_fn)(void),
                   pid_t pid, int *status, int opts)
{
    pid_t rval;

    do {
        rval = waitpid(pid, status, opts);
    } while (rval == -1 && errno == EINTR);

    if (rval == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "waitpid(%d,%p,%d) failed", pid, status, opts);
    } else if (rval < 0) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid waitpid(%d,%p,%d) return value %d", pid,
            status, opts, rval);
    }

    return rval;
}

static int fork_testrun(void)
{
    int status;

    SAFE_SIGNAL(SIGINT, sigint_handler);
    SAFE_SIGNAL(SIGTERM, sigint_handler);

    alarm(results->timeout);

    test_pid = fork();
    if (test_pid < 0)
        tst_brk(TBROK | TERRNO, "fork()");

    if (!test_pid) {
        tst_disable_oom_protection(0);
        SAFE_SIGNAL(SIGALRM, SIG_DFL);
        SAFE_SIGNAL(SIGUSR1, SIG_DFL);
        SAFE_SIGNAL(SIGTERM, SIG_DFL);
        SAFE_SIGNAL(SIGINT, SIG_DFL);
        SAFE_SETPGID(0, 0);
        testrun();
    }

    SAFE_WAITPID(test_pid, &status, 0);
    alarm(0);
    SAFE_SIGNAL(SIGTERM, SIG_DFL);
    SAFE_SIGNAL(SIGINT, SIG_DFL);

    assert(!tst_test->taint_check);
    /*if (tst_test->taint_check && tst_taint_check()) {
        tst_res(TFAIL, "Kernel is now tainted.");
        return TFAIL;
    }*/

    if (tst_test->forks_child && kill(-test_pid, SIGKILL) == 0)
        tst_res(TINFO, "Killed the leftover descendant processes");

    if (WIFEXITED(status) && WEXITSTATUS(status))
        return WEXITSTATUS(status);

    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
        tst_res(TINFO, "If you are running on slow machine, "
                   "try exporting LTP_TIMEOUT_MUL > 1");
        tst_brk(TBROK, "Test killed! (timeout?)");
    }

    if (WIFSIGNALED(status))
        tst_brk(TBROK, "Test killed by %s!", tst_strsig(WTERMSIG(status)));

    return 0;
}

static void print_colored(const char *str)
{
    if (tst_color_enabled(STDOUT_FILENO))
        assert(fprintf(stderr, "%s%s%s", ANSI_COLOR_YELLOW, str, ANSI_COLOR_RESET) == strlen(ANSI_COLOR_YELLOW) + strlen(str) + strlen(ANSI_COLOR_RESET));
    else
        assert(fprintf(stderr, "%s", str) == strlen(str));
}

static void print_failure_hint(const char *tag, const char *hint,
                   const char *url)
{
    const struct tst_tag *tags = tst_test->tags;

    if (!tags)
        return;

    unsigned int i;
    int hint_printed = 0;

    for (i = 0; tags[i].name; i++) {
        if (!strcmp(tags[i].name, tag)) {
            if (!hint_printed) {
                hint_printed = 1;
                assert(fprintf(stderr, "\n") == 1);
                print_colored("HINT: ");
                assert(fprintf(stderr, "You _MAY_ be %s:\n\n", hint) == 16 + strlen(hint));
            }

            if (url)
                assert(fprintf(stderr, "%s%s\n", url, tags[i].value) == strlen(url) + strlen(tags[i].value) + 1);
            else
                assert(fprintf(stderr, "%s\n", tags[i].value) == strlen(tags[i].value) + 1);
        }
    }
}

/* update also docparse/testinfo.pl */
static void print_failure_hints(void)
{
    print_failure_hint("linux-git", "missing kernel fixes", LINUX_GIT_URL);
    print_failure_hint("linux-stable-git", "missing stable kernel fixes",
                       LINUX_STABLE_GIT_URL);
    print_failure_hint("glibc-git", "missing glibc fixes", GLIBC_GIT_URL);
    print_failure_hint("CVE", "vulnerable to CVE(s)", CVE_DB_URL);
    print_failure_hint("known-fail", "hit by known kernel failures", NULL);
}

void tst_rmdir(void)
{
    char *errmsg;

    /*
     * Check that TESTDIR is not NULL.
     */
    if (TESTDIR == NULL) {
        tst_resm(TWARN,
             "%s: TESTDIR was NULL; no removal attempted",
             __func__);
        return;
    }

    /*
     * Unmap the backend file.
     * This is needed to overcome the NFS "silly rename" feature.
     */
    if (tst_futexes) {
        msync((void *)tst_futexes, getpagesize(), MS_SYNC);
        munmap((void *)tst_futexes, getpagesize());
    }

    /*
     * Attempt to remove the "TESTDIR" directory, using rmobj().
     */
    if (rmobj(TESTDIR, &errmsg) == -1) {
        tst_resm(TWARN, "%s: rmobj(%s) failed: %s",
             __func__, TESTDIR, errmsg);
    }
}

static struct tst_sys_conf *save_restore_data;

void tst_sys_conf_restore(int verbose)
{
    (void)verbose;
    struct tst_sys_conf *i;

    assert(!save_restore_data);
    /*for (i = save_restore_data; i; i = i->next) {
        if (verbose) {
            tst_res(TINFO, "Restoring conf.: %s -> %s\n",
                i->path, i->value);
        }
        FILE_PRINTF(i->path, "%s", i->value);
    }*/
}

static void cleanup_ipc(void)
{
    size_t size = getpagesize();

    if (ipc_fd > 0 && close(ipc_fd))
        tst_res(TWARN | TERRNO, "close(ipc_fd) failed");

    if (shm_path[0] && !access(shm_path, F_OK) && unlink(shm_path))
        tst_res(TWARN | TERRNO, "unlink(%s) failed", shm_path);

    if (results) {
        msync((void*)results, size, MS_SYNC);
        munmap((void*)results, size);
        results = NULL;
    }
}

static void do_cleanup(void)
{
    assert(!tst_test->needs_cgroup_ctrls);
    /*if (tst_test->needs_cgroup_ctrls)
        tst_cg_cleanup();*/

    assert(!ovl_mounted);
    /*if (ovl_mounted)
        SAFE_UMOUNT(OVL_MNT);*/

    assert(!mntpoint_mounted);
    /*if (mntpoint_mounted)
        tst_umount(tst_test->mntpoint);*/

    assert(!(tst_test->needs_device));
    /*if (tst_test->needs_device && tdev.dev)
        tst_release_device(tdev.dev);*/

    if (tst_tmpdir_created()) {
        /* avoid munmap() on wrong pointer in tst_rmdir() */
        tst_futexes = NULL;
        tst_rmdir();
    }

    tst_sys_conf_restore(0);

    assert(!tst_test->restore_wallclock);
    /*if (tst_test->restore_wallclock)
        tst_wallclock_restore();*/

    cleanup_ipc();
}

static void do_exit(int ret)
{
    if (results) {
        if (results->passed && ret == TCONF)
            ret = 0;

        if (results->failed) {
            ret |= TFAIL;
            print_failure_hints();
        }

        if (results->skipped && !results->passed)
            ret |= TCONF;

        if (results->warnings)
            ret |= TWARN;

        if (results->broken)
            ret |= TBROK;

        if (results->failed != 0 || results->broken != 0 || results->skipped != 0 || results->warnings != 0) {
            assert(fprintf(stderr, "\nSummary:\n") == 10);
            assert(fprintf(stderr, "passed   %d\n", results->passed) >= 10);
            assert(fprintf(stderr, "failed   %d\n", results->failed) >= 10);
            assert(fprintf(stderr, "broken   %d\n", results->broken) >= 10);
            assert(fprintf(stderr, "skipped  %d\n", results->skipped) >= 10);
            assert(fprintf(stderr, "warnings %d\n", results->warnings) >= 10);
        }
    }

    do_cleanup();

    exit(ret);
}

void tst_run_tcases(int argc, char *argv[], struct tst_test *self)
{
    int ret = 0;
    unsigned int test_variants = 1;

    lib_pid = getpid();
    tst_test = self;

    do_setup(argc, argv);
    tst_enable_oom_protection(lib_pid);

    SAFE_SIGNAL(SIGALRM, alarm_handler);
    SAFE_SIGNAL(SIGUSR1, heartbeat_handler);

    assert(!tst_test->max_runtime);
    /*if (tst_test->max_runtime)
        results->max_runtime = multiply_runtime(tst_test->max_runtime);*/

    set_timeout();

    if (tst_test->test_variants)
        test_variants = tst_test->test_variants;

    for (tst_variant = 0; tst_variant < test_variants; tst_variant++) {
        assert(!tst_test->all_filesystems);
        /*if (tst_test->all_filesystems)
            ret |= run_tcases_per_fs();
        else*/
            ret |= fork_testrun();

        if (ret & ~(TCONF))
            goto exit;
    }

exit:
    do_exit(ret);
}

static struct tst_test test;

int main(int argc, char *argv[])
{
    tst_run_tcases(argc, argv, &test);
}

#define TST_EXP_VAL_SILENT(SCALL, VAL, ...) TST_EXP_VAL_SILENT_(SCALL, VAL, #SCALL, ##__VA_ARGS__)

#define TESTPTR(SCALL) \
    do { \
        errno = 0; \
        TST_RET_PTR = (void*)SCALL; \
        TST_ERR = errno; \
    } while (0)

#define TST_2_(_1, _2, ...) _2

#define TST_FMT_(FMT, _1, ...) FMT, ##__VA_ARGS__

#define TST_MSG_(RES, FMT, SCALL, ...) \
    tst_res_(__FILE__, __LINE__, RES, \
        TST_FMT_(TST_2_(dummy, ##__VA_ARGS__, SCALL) FMT, __VA_ARGS__))

#define TST_MSGP_(RES, FMT, PAR, SCALL, ...) \
    tst_res_(__FILE__, __LINE__, RES, \
        TST_FMT_(TST_2_(dummy, ##__VA_ARGS__, SCALL) FMT, __VA_ARGS__), PAR)

#define TST_MSGP2_(RES, FMT, PAR, PAR2, SCALL, ...) \
    tst_res_(__FILE__, __LINE__, RES, \
        TST_FMT_(TST_2_(dummy, ##__VA_ARGS__, SCALL) FMT, __VA_ARGS__), PAR, PAR2)

#define TST_EXP_POSITIVE_(SCALL, SSCALL, ...)                                  \
    do {                                                                   \
        TEST(SCALL);                                                   \
                                                                       \
        TST_PASS = 0;                                                  \
                                                                       \
        if (TST_RET == -1) {                                           \
            TST_MSG_(TFAIL | TTERRNO, " failed",                   \
                     SSCALL, ##__VA_ARGS__);                       \
            break;                                                 \
        }                                                              \
                                                                       \
        if (TST_RET < 0) {                                             \
            TST_MSGP_(TFAIL | TTERRNO, " invalid retval %ld",      \
                      TST_RET, SSCALL, ##__VA_ARGS__);             \
            break;                                                 \
        }                                                              \
                                                                               \
        TST_PASS = 1;                                                  \
                                                                               \
    } while (0)

#define TST_EXP_VAL(SCALL, VAL, ...)                                           \
    do {                                                                   \
        TST_EXP_VAL_SILENT_(SCALL, VAL, #SCALL, ##__VA_ARGS__);        \
                                                                       \
        if (TST_PASS)                                                  \
            TST_MSG_(TPASS, " passed", #SCALL, ##__VA_ARGS__);     \
                                                                   \
    } while(0)

#define TEST(SCALL) \
    do { \
        errno = 0; \
        TST_RET = SCALL; \
        TST_ERR = errno; \
    } while (0)

#define TEST_VOID(SCALL) \
    do { \
        errno = 0; \
        SCALL; \
        TST_ERR = errno; \
    } while (0)

#define TST_EXP_PASS_SILENT_(SCALL, SSCALL, ...)                               \
    do {                                                                   \
        TEST(SCALL);                                                   \
                                                                       \
        TST_PASS = 0;                                                  \
                                                                       \
        if (TST_RET == -1) {                                           \
            TST_MSG_(TFAIL | TTERRNO, " failed",                   \
                     SSCALL, ##__VA_ARGS__);                       \
                break;                                                 \
        }                                                              \
                                                                       \
        if (TST_RET != 0) {                                            \
            TST_MSGP_(TFAIL | TTERRNO, " invalid retval %ld",      \
                      TST_RET, SSCALL, ##__VA_ARGS__);             \
            break;                                                 \
        }                                                              \
                                                                               \
        TST_PASS = 1;                                                  \
                                                                               \
    } while (0)

#define TST_EXP_PASS_SILENT(SCALL, ...) TST_EXP_PASS_SILENT_(SCALL, #SCALL, ##__VA_ARGS__)

#define TST_EXP_PASS(SCALL, ...)                                               \
    do {                                                                   \
        TST_EXP_PASS_SILENT_(SCALL, #SCALL, ##__VA_ARGS__);            \
                                                                       \
        if (TST_PASS)                                                  \
            TST_MSG_(TPASS, " passed", #SCALL, ##__VA_ARGS__);     \
    } while (0)

#define TST_EXP_FAIL2(SCALL, ERRNO, ...)                                       \
    do {                                                                   \
        TST_EXP_FAIL_SILENT_(TST_RET >= 0, SCALL, #SCALL,              \
            ERRNO, ##__VA_ARGS__);                                 \
        if (TST_PASS)                                                  \
            TST_MSG_(TPASS | TTERRNO, " ", #SCALL, ##__VA_ARGS__); \
    } while (0)

#define TST_EXP_FAIL_SILENT(SCALL, ERRNO, ...) \
    TST_EXP_FAIL_SILENT_(TST_RET == 0, SCALL, #SCALL, ERRNO, ##__VA_ARGS__)

#define TST_EXP_FAIL2_SILENT(SCALL, ERRNO, ...) \
    TST_EXP_FAIL_SILENT_(TST_RET >= 0, SCALL, #SCALL, ERRNO, ##__VA_ARGS__)

#define TST_EXP_EXPR(EXPR, FMT, ...)                        \
    tst_res_(__FILE__, __LINE__, (EXPR) ? TPASS : TFAIL, "Expect: " FMT, ##__VA_ARGS__);

#define TST_EXP_EQ_(VAL_A, SVAL_A, VAL_B, SVAL_B, TYPE, PFS) do {\
    TYPE tst_tmp_a__ = VAL_A; \
    TYPE tst_tmp_b__ = VAL_B; \
    if (tst_tmp_a__ == tst_tmp_b__) { \
        tst_res_(__FILE__, __LINE__, TPASS, \
            SVAL_A " == " SVAL_B " (" PFS ")", tst_tmp_a__); \
    } else { \
        tst_res_(__FILE__, __LINE__, TFAIL, \
            SVAL_A " (" PFS ") != " SVAL_B " (" PFS ")", \
            tst_tmp_a__, tst_tmp_b__); \
    } \
} while (0)

#define TST_EXP_EQ_LI(VAL_A, VAL_B) \
        TST_EXP_EQ_(VAL_A, #VAL_A, VAL_B, #VAL_B, long long, "%lli")

#define TST_EXP_EQ_LU(VAL_A, VAL_B) \
        TST_EXP_EQ_(VAL_A, #VAL_A, VAL_B, #VAL_B, unsigned long long, "%llu")

#define TST_EXP_EQ_SZ(VAL_A, VAL_B) \
        TST_EXP_EQ_(VAL_A, #VAL_A, VAL_B, #VAL_B, size_t, "%zu")

#define TST_EXP_EQ_SSZ(VAL_A, VAL_B) \
        TST_EXP_EQ_(VAL_A, #VAL_A, VAL_B, #VAL_B, ssize_t, "%zi")

struct passwd *safe_getpwnam(const char *file, const int lineno,
                 void (*cleanup_fn) (void), const char *name)
{
    struct passwd *rval;

    rval = getpwnam(name);

    if (rval == NULL) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "getpwnam(%s) failed", name);
    }

    return rval;
}

#define SAFE_PIPE2(fildes, flags) \
    safe_pipe2(__FILE__, __LINE__, (fildes), (flags))

#define SAFE_READ(len_strict, fildes, buf, nbyte) \
    safe_read(__FILE__, __LINE__, NULL, (len_strict), (fildes), (buf), (nbyte))

#define SAFE_SETEGID(egid) \
    safe_setegid(__FILE__, __LINE__, NULL, (egid))

#define SAFE_SETEUID(euid) \
    safe_seteuid(__FILE__, __LINE__, NULL, (euid))

#define SAFE_SETGID(gid) \
    safe_setgid(__FILE__, __LINE__, NULL, (gid))

#define SAFE_SETUID(uid) \
    safe_setuid(__FILE__, __LINE__, NULL, (uid))

int safe_setuid(const char *file, const int lineno, void (*cleanup_fn) (void),
                uid_t uid)
{
    int rval;

    rval = setuid(uid);

    if (rval == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "setuid(%u) failed", (unsigned int)uid);
    } else if (rval) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid setuid(%u) return value %d",
            (unsigned int)uid, rval);
    }

    return rval;
}

int tst_fill_fd(int fd, char pattern, size_t bs, size_t bcount)
{
    size_t i;
    char *buf;

    /* Filling a memory buffer with provided pattern */
    buf = malloc(bs);
    if (buf == NULL)
        return -1;

    for (i = 0; i < bs; i++)
        buf[i] = pattern;

    /* Filling the file */
    for (i = 0; i < bcount; i++) {
        if (write(fd, buf, bs) != (ssize_t)bs) {
            free(buf);
            return -1;
        }
    }

    free(buf);

    return 0;
}

int tst_fill_file(const char *path, char pattern, size_t bs, size_t bcount)
{
    int fd;

    fd = open(path, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd < 0)
        return -1;

    if (tst_fill_fd(fd, pattern, bs, bcount)) {
        close(fd);
        unlink(path);
        return -1;
    }

    if (close(fd) < 0) {
        unlink(path);

        return -1;
    }

    return 0;
}

void tst_brk_(const char *file, const int lineno, int ttype,
              const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    tst_brk_handler(file, lineno, ttype, fmt, va);
    va_end(va);
}

void tst_vres_(const char *file, const int lineno, int ttype,
               const char *fmt, va_list va)
{
    print_result(file, lineno, ttype, fmt, va);

    update_results(TTYPE_RESULT(ttype));
}

void tst_res_(const char *file, const int lineno, int ttype,
              const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    tst_vres_(file, lineno, ttype, fmt, va);
    va_end(va);
}

void tst_resm_(const char *file, const int lineno, int ttype,
    const char *arg_fmt, ...)
{
    char tmesg[USERMESG];

    EXPAND_VAR_ARGS(tmesg, arg_fmt, USERMESG);

    if (tst_test)
        tst_res_(file, lineno, ttype, "%s", tmesg);
    else
        tst_res__(file, lineno, ttype, "%s", tmesg);
}

void tst_vbrk_(const char *file, const int lineno, int ttype,
               const char *fmt, va_list va)
{
    print_result(file, lineno, ttype, fmt, va);
    update_results(TTYPE_RESULT(ttype));

    /*
     * The getpid implementation in some C library versions may cause cloned
     * test threads to show the same pid as their parent when CLONE_VM is
     * specified but CLONE_THREAD is not. Use direct syscall to avoid
     * cleanup running in the child.
     */
    // yalibct note: if a libc still does this it's a massive bug lmao
    if (getpid() == main_pid)//if (syscall(SYS_getpid) == main_pid)
        do_test_cleanup();

    if (getpid() == lib_pid)
        do_exit(TTYPE_RESULT(ttype));

    exit(TTYPE_RESULT(ttype));
}

#define SAFE_REALLOC(ptr, size) \
    safe_realloc(__FILE__, __LINE__, (ptr), (size))

#define SAFE_MKDIR(pathname, mode) \
    safe_mkdir(__FILE__, __LINE__, NULL, (pathname), (mode))

#define SAFE_RMDIR(pathname) \
    safe_rmdir(__FILE__, __LINE__, NULL, (pathname))

#define SAFE_MUNMAP(addr, length) \
    safe_munmap(__FILE__, __LINE__, NULL, (addr), (length))

#define SAFE_OPEN(pathname, oflags, ...) \
    safe_open(__FILE__, __LINE__, NULL, (pathname), (oflags), \
        ##__VA_ARGS__)

#define SAFE_PIPE(fildes) \
    safe_pipe(__FILE__, __LINE__, NULL, (fildes))

#define TST_OPEN_NEEDS_MODE(oflag) \
    (((oflag) & O_CREAT) != 0 || ((oflag) & O_TMPFILE) == O_TMPFILE)

int safe_open(const char *file, const int lineno, void (*cleanup_fn) (void),
              const char *pathname, int oflags, ...)
{
    int rval;
    mode_t mode = 0;

    if (TST_OPEN_NEEDS_MODE(oflags)) {
        va_list ap;

        va_start(ap, oflags);

        /* Android's NDK's mode_t is smaller than an int, which results in
         * SIGILL here when passing the mode_t type.
         */
        mode = va_arg(ap, int);

        va_end(ap);
    }

    rval = open(pathname, oflags, mode);

    if (rval == -1) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "open(%s,%d,%04o) failed", pathname, oflags, mode);
    } else if (rval < 0) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "Invalid open(%s,%d,%04o) return value %d", pathname,
            oflags, mode, rval);
    }

    return rval;
}

/* supported values for safe_write() len_strict parameter */
enum safe_write_opts {
    /* no length strictness, short writes are ok */
    SAFE_WRITE_ANY = 0,

    /* strict length, short writes raise TBROK */
    SAFE_WRITE_ALL = 1,

    /* retry/resume after short write */
    SAFE_WRITE_RETRY = 2,
};

ssize_t safe_write(const char *file, const int lineno, void (cleanup_fn) (void),
           enum safe_write_opts len_strict, int fildes, const void *buf,
           size_t nbyte)
{
    ssize_t rval;
    const unsigned char *wbuf = (const unsigned char *)buf;
    size_t len = nbyte;
    int iter = 0;

    do {
        iter++;
        rval = write(fildes, wbuf, len);
        if (rval == -1) {
            if (len_strict == SAFE_WRITE_RETRY)
                tst_resm_(file, lineno, TINFO,
                    "write() wrote %zu bytes in %d calls",
                    nbyte-len, iter);
            tst_brkm_(file, lineno, TBROK | TERRNO,
                cleanup_fn, "write(%d,%p,%zu) failed",
                fildes, buf, nbyte);
        }

        if (len_strict == SAFE_WRITE_ANY)
            return rval;

        if (len_strict == SAFE_WRITE_ALL) {
            if ((size_t)rval != nbyte)
                tst_brkm_(file, lineno, TBROK | TERRNO,
                    cleanup_fn, "short write(%d,%p,%zu) "
                    "return value %zd",
                    fildes, buf, nbyte, rval);
            return rval;
        }

        wbuf += rval;
        len -= rval;
    } while (len > 0);

    return rval;
}

static inline int safe_stat(const char *file, const int lineno,
                            const char *path, struct stat *buf)
{
    int rval;

    rval = stat(path, buf);

    if (rval == -1) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "stat(%s,%p) failed", path, buf);
    } else if (rval) {
        tst_brk_(file, lineno, TBROK | TERRNO,
            "Invalid stat(%s,%p) return value %d", path, buf,
            rval);
    }

    return rval;
}
#define SAFE_STAT(path, buf) \
    safe_stat(__FILE__, __LINE__, (path), (buf))

void *safe_malloc(const char *file, const int lineno, void (*cleanup_fn) (void),
          size_t size)
{
    void *rval;

    rval = malloc(size);

    if (rval == NULL) {
        tst_brkm_(file, lineno, TBROK | TERRNO, cleanup_fn,
            "malloc(%zu) failed", size);
    }

    return rval;
}
