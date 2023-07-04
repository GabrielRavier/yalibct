// Derived from code with this license:
/* Copyright (C) 2016-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#pragma once

#include "test-lib/hedley.h"
#include "test-lib/compiler-features.h"
#include "test-lib/portable-symbols/static_assert.h"
#include "test-lib/portable-symbols/MAX.h"
#include "test-lib/portable-symbols/xmalloc.h"
#include "test-lib/portable-symbols/TEMP_FAILURE_RETRY.h"
#include "test-lib/portable-symbols/getopt_long.h"
#include "test-lib/portable-symbols/no_argument.h"
#include "test-lib/portable-symbols/required_argument.h"
#include "test-lib/portable-symbols/getprogname.h"
#include "test-lib/portable-symbols/internal/gnulib/sb_init.h"
#include "test-lib/portable-symbols/internal/gnulib/sb_dupfree.h"
#include "test-lib/portable-symbols/internal/gnulib/sb_appendf.h"
#include "test-lib/portable-symbols/internal/gnulib/sb_appendvf.h"
#include <sys/stat.h>
#include <poll.h>
#include <sys/wait.h>
#include <signal.h>
#ifdef YALIBCT_LIBC_HAS_MALLOPT
#include <malloc.h>
#endif
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include <assert.h>

#define DIAG_PUSH_NEEDS_COMMENT HEDLEY_DIAGNOSTIC_PUSH

/* Compare the strings LEFT and RIGHT and report a test failure if
   they are different.  Also report failure if one of the arguments is
   a null pointer and the other is not.  The strings should be
   reasonably short because on mismatch, both are printed.  */
#define TEST_COMPARE_STRING(left, right)                         \
  (support_test_compare_string (left, right, __FILE__, __LINE__, \
                                #left, #right))

/* Wrappers for other libc functions.  */
struct xmemstream
{
  FILE *out;
  char *buffer;
  size_t length;
};

/* Record a test failure, print the failure message and terminate with
   exit status 1.  */
#define FAIL_EXIT1(...) \
  support_exit_failure_impl (1, __FILE__, __LINE__, __VA_ARGS__)

enum
  {
    /* Test exit status which indicates that the feature is
       unsupported. */
    EXIT_UNSUPPORTED = 77,

    /* Default timeout is twenty seconds.  Tests should normally
       complete faster than this, but if they don't, that's abnormal
       (a bug) anyways.  */
    DEFAULT_TIMEOUT = 20,

    /* Used for command line argument parsing.  */
    OPT_DIRECT = 1000,
    OPT_TESTDIR,
  };

/* This structure keeps track of test failures.  The counter is
   incremented on each failure.  The failed member is set to true if a
   failure is detected, so that even if the counter wraps around to
   zero, the failure of a test can be detected.

   The init constructor function below puts *state on a shared
   annonymous mapping, so that failure reports from subprocesses
   propagate to the parent process.  */
struct test_failures
{
  unsigned int counter;
  unsigned int failed;
};
static struct test_failures *state;

void
support_record_failure_init (void)
{
  void *ptr = mmap (NULL, sizeof (*state), PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  if (ptr == MAP_FAILED)
    {
      printf ("error: could not map %zu bytes: %m\n", sizeof (*state));
      exit (1);
    }
  /* Zero-initialization of the struct is sufficient.  */
  state = ptr;
}

void
write_message (const char *message)
{
  int saved_errno = errno;
  ssize_t unused __attribute__ ((unused));
  unused = write (STDOUT_FILENO, message, strlen (message));
  errno = saved_errno;
}

void
support_record_failure (void)
{
  if (state == NULL)
    {
      write_message
        ("error: support_record_failure called without initialization\n");
      _exit (1);
    }
  /* Relaxed MO is sufficient because we are only interested in the
     values themselves, in isolation.  */
  __atomic_store_n (&state->failed, 1, __ATOMIC_RELEASE);
  __atomic_add_fetch (&state->counter, 1, __ATOMIC_RELEASE);
}

static void
print_failure (const char *file, int line, const char *format, va_list ap)
{
  int saved_errno = errno;
  printf ("error: %s:%d: ", file, line);
  vprintf (format, ap);
  puts ("");
  errno = saved_errno;
}

void
support_exit_failure_impl (int status, const char *file, int line,
                           const char *format, ...)
{
  if (status != EXIT_SUCCESS && status != EXIT_UNSUPPORTED)
    support_record_failure ();
  va_list ap;
  va_start (ap, format);
  print_failure (file, line, format, ap);
  va_end (ap);
  exit (status);
}

#ifdef YALIBCT_LIBC_HAS_OPEN_MEMSTREAM
void
xopen_memstream (struct xmemstream *stream)
{
  int old_errno = errno;
  *stream = (struct xmemstream) {};
  stream->out = open_memstream (&stream->buffer, &stream->length);
  if (stream->out == NULL)
    FAIL_EXIT1 ("open_memstream: %m");
  errno = old_errno;
}
#endif

void
xfclose (FILE *fp)
{
  if (ferror (fp))
    FAIL_EXIT1 ("stdio stream closed with pending errors");
  if (fflush (fp) != 0)
    FAIL_EXIT1 ("fflush: %m");
  if (fclose (fp) != 0)
    FAIL_EXIT1 ("fclose: %m");
}

void
xfclose_memstream (struct xmemstream *stream)
{
  xfclose (stream->out);
  stream->out = NULL;
}

#ifdef YALIBCT_LIBC_HAS_OPEN_MEMSTREAM
#define xmemstream_or_string_buffer xmemstream
#define xmemstream_or_string_buffer_init xopen_memstream
static inline void xmemstream_or_string_buffer_putc_unlocked(int ch, struct xmemstream_or_string_buffer *self)
{
    putc_unlocked(ch, self->out);
}
static inline void xmemstream_or_string_buffer_fprintf(struct xmemstream_or_string_buffer *self, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    assert(vfprintf(self->out, format, args) >= 0);

    va_end(args);
}
static inline size_t xmemstream_or_string_buffer_fwrite(const void *written_array, size_t member_size, size_t num_members, struct xmemstream_or_string_buffer *self)
{
    return fwrite(written_array, member_size, num_members, self->out);
}
#define xmemstream_or_string_buffer_xfclose xfclose_memstream
#else
struct xmemstream_or_string_buffer {
    struct string_buffer string_buf;
    char *buffer;
    size_t length;
};
static inline void xmemstream_or_string_buffer_init(struct xmemstream_or_string_buffer *self)
{
    sb_init(&self->string_buf);
    self->buffer = 0;
    self->length = 0;
}
static inline void xmemstream_or_string_buffer_putc_unlocked(int c, struct xmemstream_or_string_buffer *self)
{
    sb_appendf(&self->string_buf, "%c", c);
}
static inline void xmemstream_or_string_buffer_fprintf(struct xmemstream_or_string_buffer *self, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    sb_appendvf(&self->string_buf, format, args);

    va_end(args);
}
static inline size_t xmemstream_or_string_buffer_fwrite(const void *written_array, size_t member_size, size_t num_members, struct xmemstream_or_string_buffer *self)
{
    const unsigned char *written_array_cptr = (const unsigned char *)written_array;
    if (!member_size || !num_members)
        return 0;

    for (size_t i = 0; i < num_members; ++i)
        for (size_t j = 0; j < member_size; ++j) {
            xmemstream_or_string_buffer_putc_unlocked(written_array_cptr[i * member_size + j], self);
            if (self->string_buf.error)
                return i;
        }
    return num_members;
}
static inline void xmemstream_or_string_buffer_xfclose(struct xmemstream_or_string_buffer *self)
{
    self->length = self->string_buf.length;
    self->buffer = sb_dupfree(&self->string_buf);
}
#endif

#define YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_BLOB_CHAR unsigned char
#define YALIBCT_INTERNAL_GLIBC_SUPPORT_QUOTE_L_(C) C
#define SUPPORT_QUOTE_BLOB support_quote_blob
#define WIDE 0

#include "test-deps/glibc/support_quote_blob_main.h"

#define YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_CHAR char
#define YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_UCHAR unsigned char
#define LPREFIX ""
#define YALIBCT_INTERNAL_GLIBC_SUPPORT_TEST_COMPARE_STRING_STRLEN strlen
#define MEMCMP memcmp
#define SUPPORT_QUOTE_BLOB support_quote_blob
#define SUPPORT_TEST_COMPARE_STRING support_test_compare_string
#define WIDE 0

#include "test-deps/glibc/support_test_compare_string_main.h"

/* Compare the two integers LEFT and RIGHT and report failure if they
   are different.  */
#define TEST_COMPARE(left, right)                                       \
  ({                                                                    \
    /* + applies the integer promotions, for bitfield support.   */     \
    typedef __typeof__ (+ (left)) __left_type;                          \
    typedef __typeof__ (+ (right)) __right_type;                        \
    __left_type __left_value = (left);                                  \
    __right_type __right_value = (right);                               \
    int __left_is_positive = __left_value > 0;                          \
    int __right_is_positive = __right_value > 0;                        \
    /* Prevent use with floating-point types.  */                       \
    support_static_assert ((__left_type) 1.0 == (__left_type) 1.5,      \
                           "left value has floating-point type");       \
    support_static_assert ((__right_type) 1.0 == (__right_type) 1.5,    \
                           "right value has floating-point type");      \
    /* Prevent accidental use with larger-than-long long types.  */     \
    support_static_assert (sizeof (__left_value) <= sizeof (long long), \
                           "left value fits into long long");           \
    support_static_assert (sizeof (__right_value) <= sizeof (long long), \
                    "right value fits into long long");                 \
    /* Compare the value.  */                                           \
    if (__left_value != __right_value                                   \
        || __left_is_positive != __right_is_positive)                   \
      /* Pass the sign for printing the correct value.  */              \
      support_test_compare_failure                                      \
        (__FILE__, __LINE__,                                            \
         #left, __left_value, __left_is_positive, sizeof (__left_type), \
         #right, __right_value, __right_is_positive, sizeof (__right_type)); \
  })

/* Record a test failure and exit if EXPR evaluates to false.  */
#define TEST_VERIFY_EXIT(expr)                                  \
  ({                                                            \
    if (expr)                                                   \
      ;                                                         \
    else                                                        \
      support_test_verify_exit_impl                             \
        (1, __FILE__, __LINE__, #expr);                         \
  })

struct test_config
{
  void (*prepare_function) (int argc, char **argv);
  int (*test_function) (void);
  int (*test_function_argv) (int argc, char **argv);
  void (*cleanup_function) (void);
  void (*cmdline_function) (int);
  const void *options;   /* Custom options if not NULL.  */
  int timeout;           /* Test timeout in seconds.  */
  int expected_status;   /* Expected exit status.  */
  int expected_signal;   /* If non-zero, expect termination by signal.  */
  char no_mallopt;       /* Boolean flag to disable mallopt.  */
  char no_setvbuf;       /* Boolean flag to disable setvbuf.  */
  char run_command_mode; /* Boolean flag to indicate run-command-mode.  */
  const char *optstring; /* Short command line options.  */
};

#define support_static_assert static_assert

static void
report (const char *which, const char *expr, long long value, int positive,
        int size)
{
  printf ("  %s: ", which);
  if (positive)
    printf ("%llu", (unsigned long long) value);
  else
    printf ("%lld", value);
  unsigned long long mask
    = (~0ULL) >> (8 * (sizeof (unsigned long long) - size));
  printf (" (0x%llx); from: %s\n", (unsigned long long) value & mask, expr);
}

void
support_test_compare_failure (const char *file, int line,
                              const char *left_expr,
                              long long left_value,
                              int left_positive,
                              int left_size,
                              const char *right_expr,
                              long long right_value,
                              int right_positive,
                              int right_size)
{
  int saved_errno = errno;
  support_record_failure ();
  if (left_size != right_size)
    printf ("%s:%d: numeric comparison failure (widths %d and %d)\n",
            file, line, left_size * 8, right_size * 8);
  else
    printf ("%s:%d: numeric comparison failure\n", file, line);
  report (" left", left_expr, left_value, left_positive, left_size);
  report ("right", right_expr, right_value, right_positive, right_size);
  errno = saved_errno;
}

static bool test_main_called;

/* Options provided by the test driver.  */
#define TEST_DEFAULT_OPTIONS                            \
  { "verbose", no_argument, NULL, 'v' },                \
  { "direct", no_argument, NULL, OPT_DIRECT },          \
  { "test-dir", required_argument, NULL, OPT_TESTDIR }, \

static const struct option default_options[] =
{
  TEST_DEFAULT_OPTIONS
  { NULL, 0, NULL, 0 }
};

/* The cleanup handler passed to test_main.  */
static void (*cleanup_function) (void);

enum {
    TIMEOUTFACTOR = 1
};

/* Show people how to run the program.  */
static void
usage (const struct option *options)
{
  size_t i;

  printf ("Usage: %s [options]\n"
          "\n"
          "Environment Variables:\n"
          "  TIMEOUTFACTOR          An integer used to scale the timeout\n"
          "  TMPDIR                 Where to place temporary files\n"
          "  TEST_COREDUMPS         Do not disable coredumps if set\n"
          "\n",
          getprogname());
  printf ("Options:\n");
  for (i = 0; options[i].name; ++i)
    {
      int indent;

      indent = printf ("  --%s", options[i].name);
      if (options[i].has_arg == required_argument)
        indent += printf (" <arg>");
      printf ("%*s", 25 - indent, "");
      switch (options[i].val)
        {
        case 'v':
          printf ("Increase the output verbosity");
          break;
        case OPT_DIRECT:
          printf ("Run the test directly (instead of forking & monitoring)");
          break;
        case OPT_TESTDIR:
          printf ("Override the TMPDIR env var");
          break;
        }
      printf ("\n");
    }
}

const char *test_dir = NULL;
unsigned int test_verbose = 0;

void support_set_test_dir(const char *path) { test_dir = path; }

/* List of temporary files.  */
static struct temp_name_list
{
  struct temp_name_list *next;
  char *name;
  pid_t owner;
  bool toolong;
} *temp_name_list;

/* Name of subdirectories in a too long temporary directory tree.  */
static char toolong_subdir[NAME_MAX + 1];
static bool toolong_initialized;
static size_t toolong_path_max;

static void
ensure_toolong_initialized (void)
{
  if (!toolong_initialized)
    FAIL_EXIT1 ("uninitialized toolong directory tree\n");
}

/* Helper functions called by the test skeleton follow.  */

static void
remove_toolong_subdirs (const char *base)
{
  ensure_toolong_initialized ();

  if (chdir (base) != 0)
    {
      printf ("warning: toolong cleanup base failed: chdir (\"%s\"): %m\n",
          base);
      return;
    }

  /* Descend.  */
  int levels = 0;
  size_t sz = strlen (toolong_subdir);
  for (levels = 0; levels <= toolong_path_max / sz; levels++)
    if (chdir (toolong_subdir) != 0)
      {
    printf ("warning: toolong cleanup failed: chdir (\"%s\"): %m\n",
        toolong_subdir);
    break;
      }

  /* Ascend and remove.  */
  while (--levels >= 0)
    {
      if (chdir ("..") != 0)
    {
      printf ("warning: toolong cleanup failed: chdir (\"..\"): %m\n");
      return;
    }
      if (remove (toolong_subdir) != 0)
    {
      printf ("warning: could not remove subdirectory: %s: %m\n",
          toolong_subdir);
      return;
    }
    }
}

void
support_delete_temp_files (void)
{
  pid_t pid = getpid ();
  while (temp_name_list != NULL)
    {
      /* Only perform the removal if the path was registed in the same
     process, as identified by the PID.  (This assumes that the
     parent process which registered the temporary file sticks
     around, to prevent PID reuse.)  */
      if (temp_name_list->owner == pid)
    {
      if (temp_name_list->toolong)
        remove_toolong_subdirs (temp_name_list->name);

      if (remove (temp_name_list->name) != 0)
        printf ("warning: could not remove temporary file: %s: %m\n",
            temp_name_list->name);
    }
      free (temp_name_list->name);

      struct temp_name_list *next = temp_name_list->next;
      free (temp_name_list);
      temp_name_list = next;
    }
}

void
support_print_temp_files (FILE *f)
{
  if (temp_name_list != NULL)
    {
      struct temp_name_list *n;
      assert(fprintf (f, "temp_files=(\n") >= 0);
      for (n = temp_name_list; n != NULL; n = n->next)
        assert(fprintf (f, "  '%s'\n", n->name) >= 0);
      assert(fprintf (f, ")\n") == 2);;
    }
}

int
support_report_failure (int status)
{
  if (state == NULL)
    {
      write_message
        ("error: support_report_failure called without initialization\n");
      return 1;
    }

  /* Relaxed MO is sufficient because acquire test result reporting
     assumes that exiting from the main thread happens before the
     error reporting via support_record_failure, which requires some
     form of external synchronization.  */
  bool failed = __atomic_load_n (&state->failed, __ATOMIC_RELAXED);
  if (failed)
    printf ("error: %u test failures\n",
            __atomic_load_n (&state->counter, __ATOMIC_RELAXED));

  if ((status == 0 || status == EXIT_UNSUPPORTED) && failed)
    /* If we have a recorded failure, it overrides a non-failure
       report from the test function.  */
    status = 1;
  return status;
}

/* If test failure reporting has been linked in, it may contribute
   additional test failures.  */
static int
adjust_exit_status (int status)
{
  //if (support_report_failure != NULL)
    return support_report_failure (status);
  //return status;
}

/* This must be volatile as it will be modified by the debugger.  */
static volatile int wait_for_debugger = 0;

FILE *
xfopen (const char *path, const char *mode)
{
  FILE *fp = fopen (path, mode);
  if (fp == NULL)
    FAIL_EXIT1 ("could not open %s (mode \"%s\"): %m", path, mode);
  return fp;
}

const char support_objdir_root[] = "placeholder for OBJDIR_PATH (not present for us)";

/* Run test_function or test_function_argv.  */
static int
run_test_function (int argc, char **argv, const struct test_config *config)
{
  const char *wfd = getenv("WAIT_FOR_DEBUGGER");
  if (wfd != NULL)
    wait_for_debugger = atoi (wfd);
  if (wait_for_debugger)
    {
      pid_t mypid;
      FILE *gdb_script;
      char *gdb_script_name;
      int inside_container = 0;

      mypid = getpid();
      if (mypid < 3)
    {
      const char *outside_pid = getenv("PID_OUTSIDE_CONTAINER");
      if (outside_pid)
        {
          mypid = atoi (outside_pid);
          inside_container = 1;
        }
    }

      gdb_script_name = (char *) xmalloc (strlen (argv[0]) + strlen (".gdb") + 1);
      assert(sprintf (gdb_script_name, "%s.gdb", argv[0]) == strlen(argv[0]) + strlen(".gdb"));
      gdb_script = xfopen (gdb_script_name, "w");

      assert(fprintf (stderr, "Waiting for debugger, test process is pid %d\n", mypid) >= 0);
      assert(fprintf (stderr, "gdb -x %s\n", gdb_script_name) == 8 + strlen(gdb_script_name));
      if (inside_container)
          assert(fprintf (gdb_script, "set sysroot %s/testroot.root\n", support_objdir_root) >= 0);
      assert(fprintf (gdb_script, "file\n") == 5);
      assert(fprintf (gdb_script, "file %s\n", argv[0]) == 6 + strlen(argv[0]));
      assert(fprintf (gdb_script, "symbol-file %s\n", argv[0]) >= 13 + strlen(argv[0]));
      assert(fprintf (gdb_script, "exec-file %s\n", argv[0]) == 11 + strlen(argv[0]));
      assert(fprintf (gdb_script, "attach %ld\n", (long int) mypid) >= 8);
      assert(fprintf (gdb_script, "set wait_for_debugger = 0\n") >= 0);
      assert(fclose (gdb_script) == 0);
      free (gdb_script_name);
    }

  /* Wait for the debugger to set wait_for_debugger to zero.  */
  while (wait_for_debugger)
    usleep (1000);

  if (config->run_command_mode)
    {
      /* In run-command-mode, the child process executes the command line
     arguments as a new program.  */
      char **argv_ = xmalloc (sizeof (char *) * argc);
      memcpy (argv_, &argv[1], sizeof (char *) * (argc - 1));
      argv_[argc - 1] = NULL;
      execv (argv_[0], argv_);
      printf ("error: should not return here\n");
      exit (1);
    }

  if (config->test_function != NULL)
    return config->test_function ();
  else if (config->test_function_argv != NULL)
    return config->test_function_argv (argc, argv);
  else
    {
      printf ("error: no test function defined\n");
      exit (1);
    }
}

/* The PID of the test process.  */
static pid_t test_pid;

static void
print_timestamp (const char *what, struct timespec tv)
{
  struct tm tm;
  /* Casts of tv.tv_nsec below are necessary because the type of
     tv_nsec is not literally long int on all supported platforms.  */
  if (gmtime_r (&tv.tv_sec, &tm) == NULL) // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
    printf ("%s: %lld.%09ld\n",
            what, (long long int) tv.tv_sec, (long int) tv.tv_nsec);
  else
    printf ("%s: %04d-%02d-%02dT%02d:%02d:%02d.%09ld\n",
            what, 1900 + tm.tm_year, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, (long int) tv.tv_nsec);
}

/* Timeout handler.  We kill the child and exit with an error.  */
static void
__attribute__ ((noreturn))
signal_handler (int sig)
{
  int killed;
  int status;

  /* Do this first to avoid further interference from the
     subprocess.  */
  struct timespec now;
  clock_gettime (CLOCK_REALTIME, &now);
#ifdef YALIBCT_LIBC_HAS_FSTAT64
  struct stat64 st;
  bool st_available = fstat64 (STDOUT_FILENO, &st) == 0 && st.st_mtime != 0; // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
#else
  struct stat st;
  bool st_available = fstat (STDOUT_FILENO, &st) == 0 && st.st_mtime != 0;
#endif

  assert (test_pid > 1); // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
  /* Kill the whole process group.  */
  kill (-test_pid, SIGKILL);
  /* In case setpgid failed in the child, kill it individually too.  */
  kill (test_pid, SIGKILL);

  /* Wait for it to terminate.  */
  int i;
  for (i = 0; i < 5; ++i)
    {
      killed = waitpid (test_pid, &status, WNOHANG|WUNTRACED);
      if (killed != 0)
        break;

      /* Delay, give the system time to process the kill.  If the
         nanosleep() call return prematurely, all the better.  We
         won't restart it since this probably means the child process
         finally died.  */
      struct timespec ts;
      ts.tv_sec = 0;
      ts.tv_nsec = 100000000;
      nanosleep (&ts, NULL); // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
    }
  if (killed != 0 && killed != test_pid)
    {
      printf ("Failed to kill test process: %m\n"); // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
      exit (1); // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
    }

  if (cleanup_function != NULL)
    cleanup_function ();

  if (sig == SIGINT)
    {
      assert(signal (sig, SIG_DFL) != SIG_ERR);
      assert(raise (sig) == 0);
    }

  if (killed == 0 || (WIFSIGNALED (status) && WTERMSIG (status) == SIGKILL))
      assert(puts ("Timed out: killed the child process") >= 0); // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
  else if (WIFSTOPPED (status))
    printf ("Timed out: the child process was %s\n",
            strsignal (WSTOPSIG (status))); // NOLINT(bugprone-signal-handler,cert-sig30-c,cert-msc54-cpp)
  else if (WIFSIGNALED (status))
    printf ("Timed out: the child process got signal %s\n",
            strsignal (WTERMSIG (status)));
  else
    printf ("Timed out: killed the child process but it exited %d\n",
            WEXITSTATUS (status));

  print_timestamp ("Termination time", now);
  if (st_available) {
#ifdef YALIBCT_LIBC_HAS_STRUCT_STAT_ST_MTIM
    print_timestamp ("Last write to standard output", st.st_mtim);
#else
    struct timespec mtime_timespec;
    mtime_timespec.tv_sec = st.st_mtime;
    mtime_timespec.tv_nsec = 0;
    print_timestamp ("Last write to standard output", mtime_timespec);
#endif
  }

  /* Exit with an error.  */
  exit (1);
}

int
support_test_main (int argc, char **argv, const struct test_config *config)
{
  if (test_main_called)
    {
      printf ("error: test_main called for a second time\n");
      exit (1);
    }
  test_main_called = true;
  const struct option *options;
  if (config->options != NULL)
    options = config->options;
  else
    options = default_options;

  cleanup_function = config->cleanup_function;

  int direct = 0;       /* Directly call the test function?  */
  int status;
  int opt;
  unsigned int timeoutfactor = TIMEOUTFACTOR;
  pid_t termpid;

  /* If we're debugging the test, we need to disable timeouts and use
     the initial pid (esp if we're running inside a container).  */
  if (getenv("WAIT_FOR_DEBUGGER") != NULL)
    direct = 1;

  if (!config->no_mallopt)
    {
      /* Make uses of freed and uninitialized memory known.  Do not
         pull in a definition for mallopt if it has not been defined
         already.  */
      //extern __typeof__ (mallopt) mallopt __attribute__ ((weak));
      //if (mallopt != NULL)
#if defined(YALIBCT_LIBC_HAS_MALLOPT) && defined(YALIBCT_LIBC_HAS_M_PERTURB)
        mallopt (M_PERTURB, 42);
#endif
    }

  while ((opt = getopt_long (argc, argv, config->optstring, options, NULL))
     != -1)
    switch (opt)
      {
      case '?':
        usage (options);
        exit (1);
      case 'v':
        ++test_verbose;
        break;
      case OPT_DIRECT:
        direct = 1;
        break;
      case OPT_TESTDIR:
        test_dir = optarg;
        break;
      default:
        if (config->cmdline_function != NULL)
          config->cmdline_function (opt);
      }

  /* If set, read the test TIMEOUTFACTOR value from the environment.
     This value is used to scale the default test timeout values. */
  char *envstr_timeoutfactor = getenv ("TIMEOUTFACTOR");
  if (envstr_timeoutfactor != NULL)
    {
      char *envstr_conv = envstr_timeoutfactor;
      unsigned long int env_fact;

      env_fact = strtoul (envstr_timeoutfactor, &envstr_conv, 0);
      if (*envstr_conv == '\0' && envstr_conv != envstr_timeoutfactor)
        timeoutfactor = MAX (env_fact, 1);
    }

  /* Set TMPDIR to specified test directory.  */
  if (test_dir != NULL)
    {
      assert(setenv ("TMPDIR", test_dir, 1) == 0);

      if (chdir (test_dir) < 0)
        {
          printf ("chdir: %m\n");
          exit (1);
        }
    }
  else
    {
      test_dir = getenv ("TMPDIR");
      if (test_dir == NULL || test_dir[0] == '\0')
        test_dir = "/tmp";
    }
  //if (support_set_test_dir != NULL)
    support_set_test_dir (test_dir);

  int timeout = config->timeout;
  if (timeout == 0)
    timeout =  DEFAULT_TIMEOUT;

  /* Make sure we see all message, even those on stdout.  */
  if (!config->no_setvbuf)
    assert(setvbuf (stdout, NULL, _IONBF, 0) == 0);

  /* Make sure temporary files are deleted.  */
  //if (support_delete_temp_files != NULL)
  assert(atexit (support_delete_temp_files) == 0);

  /* Correct for the possible parameters.  */
  argv[optind - 1] = argv[0];
  argv += optind - 1;
  argc -= optind - 1;

  /* Call the initializing function, if one is available.  */
  if (config->prepare_function != NULL)
    config->prepare_function (argc, argv);

  const char *envstr_direct = getenv ("TEST_DIRECT");
  if (envstr_direct != NULL)
    {
      FILE *f = fopen (envstr_direct, "w");
      if (f == NULL)
        {
          printf ("cannot open TEST_DIRECT output file '%s': %m\n",
                  envstr_direct);
          exit (1);
        }

      assert(fprintf (f, "timeout=%u\ntimeoutfactor=%u\n",
             config->timeout, timeoutfactor) >= 0);
      if (config->expected_status != 0)
        assert(fprintf (f, "exit=%u\n", config->expected_status) >= 0);
      if (config->expected_signal != 0)
        assert(fprintf (f, "signal=%s\n", strsignal (config->expected_signal)) >= 0);

      //if (support_print_temp_files != NULL)
        support_print_temp_files (f);

      assert(fclose (f) == 0);
      direct = 1;
    }

  bool disable_coredumps;
  {
    const char *coredumps = getenv ("TEST_COREDUMPS");
    disable_coredumps = coredumps == NULL || coredumps[0] == '\0';
  }

  /* If we are not expected to fork run the function immediately.  */
  if (direct)
    return adjust_exit_status (run_test_function (argc, argv, config));

  /* Set up the test environment:
     - prevent core dumps
     - set up the timer
     - fork and execute the function.  */

  test_pid = fork ();
  if (test_pid == 0)
    {
      /* This is the child.  */
      if (disable_coredumps)
        {
          /* Try to avoid dumping core.  This is necessary because we
             run the test from the source tree, and the coredumps
             would end up there (and not in the build tree).  */
          struct rlimit core_limit;
          core_limit.rlim_cur = 0;
          core_limit.rlim_max = 0;
          setrlimit (RLIMIT_CORE, &core_limit);
        }

      /* We put the test process in its own pgrp so that if it bogusly
         generates any job control signals, they won't hit the whole build.  */
      if (setpgid (0, 0) != 0)
        printf ("Failed to set the process group ID: %m\n");

      /* Execute the test function and exit with the return value.   */
      exit (run_test_function (argc, argv, config));
    }
  else if (test_pid < 0)
    {
      printf ("Cannot fork test program: %m\n");
      exit (1);
    }

  /* Set timeout.  */
  assert(signal (SIGALRM, signal_handler) != SIG_ERR);
  alarm (timeout * timeoutfactor);

  /* Make sure we clean up if the wrapper gets interrupted.  */
  assert(signal (SIGINT, signal_handler) != SIG_ERR);

  /* Wait for the regular termination.  */
  termpid = TEMP_FAILURE_RETRY (waitpid (test_pid, &status, 0));
  if (termpid == -1)
    {
      printf ("Waiting for test program failed: %m\n");
      exit (1);
    }
  if (termpid != test_pid)
    {
      printf ("Oops, wrong test program terminated: expected %ld, got %ld\n",
              (long int) test_pid, (long int) termpid);
      exit (1);
    }

  /* Process terminated normaly without timeout etc.  */
  if (WIFEXITED (status))
    {
      if (config->expected_status == 0)
        {
          if (config->expected_signal == 0)
            /* Exit with the return value of the test.  */
            return adjust_exit_status (WEXITSTATUS (status));
          else
            {
              printf ("Expected signal '%s' from child, got none\n",
                      strsignal (config->expected_signal));
              exit (1);
            }
        }
      else
        {
          /* Non-zero exit status is expected */
          if (WEXITSTATUS (status) != config->expected_status)
            {
              printf ("Expected status %d, got %d\n",
                      config->expected_status, WEXITSTATUS (status));
              exit (1);
            }
        }
      return adjust_exit_status (0);
    }
  /* Process was killed by timer or other signal.  */
  else
    {
      if (config->expected_signal == 0)
        {
          printf ("Didn't expect signal from child: got `%s'\n",
                  strsignal (WTERMSIG (status)));
          exit (1);
        }
      else if (WTERMSIG (status) != config->expected_signal)
        {
          printf ("Incorrect signal from child: got `%s', need `%s'\n",
                  strsignal (WTERMSIG (status)),
                  strsignal (config->expected_signal));
          exit (1);
        }

      return adjust_exit_status (0);
    }
}

#define DIAG_IGNORE_NEEDS_COMMENT(version, option) YALIBCT_DIAGNOSTIC_IGNORE(option)

void
support_test_verify_impl (const char *file, int line, const char *expr)
{
  int saved_errno = errno;
  support_record_failure ();
  printf ("error: %s:%d: not true: %s\n", file, line, expr);
  errno = saved_errno;
}

void
support_test_verify_exit_impl (int status, const char *file, int line,
                               const char *expr)
{
  support_test_verify_impl (file, line, expr);
  exit (status);
}

/* Compare [LEFT, LEFT + LEFT_LENGTH) with [RIGHT, RIGHT +
   RIGHT_LENGTH) and report a test failure if the arrays are
   different.  LEFT_LENGTH and RIGHT_LENGTH are measured in bytes.  If
   the length is null, the corresponding pointer is ignored (i.e., it
   can be NULL).  The blobs should be reasonably short because on
   mismatch, both are printed.  */
#define TEST_COMPARE_BLOB(left, left_length, right, right_length)       \
  (support_test_compare_blob (left, left_length, right, right_length,   \
                              __FILE__, __LINE__,                       \
                              #left, #left_length, #right, #right_length))

static void
yalibct_internal_glibc_support_test_compare_blob_report_length (const char *what, unsigned long int length, const char *expr)
{
  printf ("  %s %lu bytes (from %s)\n", what, length, expr);
}

static void
report_blob (const char *what, const unsigned char *blob,
             unsigned long int length, const char *expr)
{
  if (blob == NULL && length > 0)
    printf ("  %s (evaluated from %s): NULL\n", what, expr);
  else if (length > 0)
    {
      printf ("  %s (evaluated from %s):\n", what, expr);
      char *quoted = support_quote_blob (blob, length);
      printf ("      \"%s\"\n", quoted);
      free (quoted);

      assert(fputs ("     ", stdout) >= 0);
      for (unsigned long i = 0; i < length; ++i)
        printf (" %02X", blob[i]);
      assert(putc ('\n', stdout) == '\n');
    }
}

void
support_test_compare_blob (const void *left, unsigned long int left_length,
                           const void *right, unsigned long int right_length,
                           const char *file, int line,
                           const char *left_expr, const char *left_len_expr,
                           const char *right_expr, const char *right_len_expr)
{
  /* No differences are possible if both lengths are null.  */
  if (left_length == 0 && right_length == 0)
    return;

  if (left_length != right_length || left == NULL || right == NULL
      || memcmp (left, right, left_length) != 0)
    {
      support_record_failure ();
      printf ("%s:%d: error: blob comparison failed\n", file, line);
      if (left_length == right_length)
        printf ("  blob length: %lu bytes\n", left_length);
      else
        {
          yalibct_internal_glibc_support_test_compare_blob_report_length ("left length: ", left_length, left_len_expr);
          yalibct_internal_glibc_support_test_compare_blob_report_length ("right length:", right_length, right_len_expr);
        }
      report_blob ("left", left, left_length, left_expr);
      report_blob ("right", right, right_length, right_expr);
    }
}

/* array_length (VAR) is the number of elements in the array VAR.  VAR
   must evaluate to an array, not a pointer.  */
#define array_length(var)                                               \
  (sizeof (var) / sizeof ((var)[0])                                     \
   + 0 * sizeof (struct {                                               \
       _Static_assert (!__builtin_types_compatible_p                    \
                       (__typeof (var), __typeof (&(var)[0])),          \
                       "argument must be an array");                    \
   }))

#define DIAG_POP_NEEDS_COMMENT HEDLEY_DIAGNOSTIC_POP

struct support_capture_subprocess
{
  struct xmemstream_or_string_buffer out;
  struct xmemstream_or_string_buffer err;
  int status;
};

struct support_subprocess
{
  int stdout_pipe[2];
  int stderr_pipe[2];
  pid_t pid;
};

void
xpipe (int fds[2])
{
  if (pipe (fds) < 0)
    FAIL_EXIT1 ("pipe: %m");
}

/* Record a test failure (but continue executing) if EXPR evaluates to
   false.  */
#define TEST_VERIFY(expr)                                       \
  ({                                                            \
    if (expr)                                                   \
      ;                                                         \
    else                                                        \
      support_test_verify_impl (__FILE__, __LINE__, #expr);     \
  })


static struct support_subprocess
support_subprocess_init (void)
{
  struct support_subprocess result = {};

  xpipe (result.stdout_pipe);
  TEST_VERIFY (result.stdout_pipe[0] > STDERR_FILENO);
  TEST_VERIFY (result.stdout_pipe[1] > STDERR_FILENO);

  xpipe (result.stderr_pipe);
  TEST_VERIFY (result.stderr_pipe[0] > STDERR_FILENO);
  TEST_VERIFY (result.stderr_pipe[1] > STDERR_FILENO);

  TEST_VERIFY (fflush (stdout) == 0);
  TEST_VERIFY (fflush (stderr) == 0);

  return result;
}

pid_t
xfork (void)
{
  pid_t result = fork ();
  if (result < 0)
    FAIL_EXIT1 ("fork: %m");
  return result;
}

void
xclose (int fd)
{
  if (close (fd) < 0 && errno != EINTR)
    FAIL_EXIT1 ("close of descriptor %d failed: %m", fd);
}

void
xdup2 (int from, int to)
{
  if (dup2 (from, to) < 0)
    FAIL_EXIT1 ("dup2 (%d, %d): %m", from, to);
}

struct support_subprocess
support_subprocess (void (*callback) (void *), void *closure)
{
  struct support_subprocess result = support_subprocess_init ();

  result.pid = xfork ();
  if (result.pid == 0)
    {
      xclose (result.stdout_pipe[0]);
      xclose (result.stderr_pipe[0]);
      xdup2 (result.stdout_pipe[1], STDOUT_FILENO);
      xdup2 (result.stderr_pipe[1], STDERR_FILENO);
      xclose (result.stdout_pipe[1]);
      xclose (result.stderr_pipe[1]);
      callback (closure);
      _exit (0);
    }
  xclose (result.stdout_pipe[1]);
  xclose (result.stderr_pipe[1]);

  return result;
}

int
xpoll (struct pollfd *fds, nfds_t nfds, int timeout)
{
  int ret = poll (fds, nfds, timeout);
  if (ret < 0)
    FAIL_EXIT1 ("poll: %m");
  return ret;
}

static void
transfer (const char *what, struct pollfd *pfd, struct xmemstream_or_string_buffer *stream)
{
  if (pfd->revents != 0)
    {
      char buf[1024];
      ssize_t ret = TEMP_FAILURE_RETRY (read (pfd->fd, buf, sizeof (buf)));
      if (ret < 0)
        {
          support_record_failure ();
          printf ("error: reading from subprocess %s: %m\n", what);
          pfd->events = 0;
          pfd->revents = 0;
        }
      else if (ret == 0)
        {
          /* EOF reached.  Stop listening.  */
          pfd->events = 0;
          pfd->revents = 0;
        }
      else
        /* Store the data just read.   */
        TEST_VERIFY (xmemstream_or_string_buffer_fwrite (buf, ret, 1, stream) == 1);
    }
}

int
xwaitpid (int pid, int *status, int flags)
{
  pid_t result = waitpid (pid, status, flags);
  if (result < 0)
    FAIL_EXIT1 ("waitpid: %m\n");
  return result;
}

int
support_process_wait (struct support_subprocess *proc)
{
  xclose (proc->stdout_pipe[0]);
  xclose (proc->stderr_pipe[0]);

  int status;
  xwaitpid (proc->pid, &status, 0);
  return status;
}

static void
support_capture_poll (struct support_capture_subprocess *result,
              struct support_subprocess *proc)
{
  struct pollfd fds[2] =
    {
      { .fd = proc->stdout_pipe[0], .events = POLLIN },
      { .fd = proc->stderr_pipe[0], .events = POLLIN },
    };

  do
    {
      xpoll (fds, 2, -1);
      transfer ("stdout", &fds[0], &result->out);
      transfer ("stderr", &fds[1], &result->err);
    }
  while (fds[0].events != 0 || fds[1].events != 0);

  xmemstream_or_string_buffer_xfclose (&result->out);
  xmemstream_or_string_buffer_xfclose (&result->err);

  result->status = support_process_wait (proc);
}

struct support_capture_subprocess
support_capture_subprocess (void (*callback) (void *), void *closure)
{
  struct support_capture_subprocess result;
  xmemstream_or_string_buffer_init(&result.out);
  xmemstream_or_string_buffer_init(&result.err);

  struct support_subprocess proc = support_subprocess (callback, closure);

  support_capture_poll (&result, &proc);
  return result;
}

char *
xasprintf (const char *format, ...)
{
  va_list ap;
  va_start (ap, format);
  char *result;
  if (vasprintf (&result, format, ap) < 0)
    FAIL_EXIT1 ("asprintf: %m");
  va_end (ap);
  return result;
}

void
oom_error (const char *function, size_t size)
{
  printf ("%s: unable to allocate %zu bytes: %m\n", function, size);
  exit (1);
}

void *
xcalloc (size_t n, size_t s)
{
  void *p;

  p = calloc (n, s);
  if (p == NULL)
    oom_error ("calloc", n * s);
  return p;
}

static void
add_temp_file_internal (const char *name, bool toolong)
{
  struct temp_name_list *newp
    = (struct temp_name_list *) xcalloc (sizeof (*newp), 1);
  char *newname = strdup (name);
  if (newname != NULL)
    {
      newp->name = newname;
      newp->next = temp_name_list;
      newp->owner = getpid ();
      newp->toolong = toolong;
      temp_name_list = newp;
    }
  else
    free (newp);
}

void
add_temp_file (const char *name)
{
  add_temp_file_internal (name, false);
}

int
create_temp_file_in_dir (const char *base, const char *dir, char **filename)
{
  char *fname;
  int fd;

  fname = xasprintf ("%s/%sXXXXXX", dir, base);

  fd = mkstemp (fname);
  if (fd == -1)
    {
      printf ("cannot open temporary file '%s': %m\n", fname);
      free (fname);
      return -1;
    }

  add_temp_file (fname);
  if (filename != NULL)
    *filename = fname;
  else
    free (fname);

  return fd;
}

int
create_temp_file (const char *base, char **filename)
{
  return create_temp_file_in_dir (base, test_dir, filename);
}

int
xopen (const char *path, int flags, mode_t mode)
{
#ifdef YALIBCT_LIBC_HAS_OPEN64
  int ret = open64 (path, flags, mode);
  if (ret < 0)
    FAIL_EXIT1 ("open64 (\"%s\", 0x%x, 0%o): %m", path, flags, mode);
#else
  int ret = open (path, flags, mode);
  if (ret < 0)
    FAIL_EXIT1 ("open (\"%s\", 0x%x, 0%o): %m", path, flags, mode);
#endif
  return ret;
}

void
support_write_file_string (const char *path, const char *contents)
{
  int fd = xopen (path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
  const char *end = contents + strlen (contents);
  for (const char *p = contents; p < end; )
    {
      ssize_t ret = write (fd, p, end - p);
      if (ret < 0)
        FAIL_EXIT1 ("cannot write to \"%s\": %m", path);
      if (ret == 0)
        FAIL_EXIT1 ("zero-length write to \"%s\"", path);
      p += ret;
    }
  xclose (fd);
}

bool
support_stat_nanoseconds (const char *path)
{
    (void)path;
  bool support = true;
#if 0//def __linux__
  /* Obtain the original timestamp to restore at the end.  */
  struct stat ost;
  TEST_VERIFY_EXIT (stat (path, &ost) == 0);

  const struct timespec tsp[] = { { 0, TIMESPEC_HZ - 1 },
                  { 0, TIMESPEC_HZ / 2 } };
  TEST_VERIFY_EXIT (utimensat (AT_FDCWD, path, tsp, 0) == 0);

  struct stat st;
  TEST_VERIFY_EXIT (stat (path, &st) == 0);

  support = st.st_atim.tv_nsec == tsp[0].tv_nsec
        && st.st_mtim.tv_nsec == tsp[1].tv_nsec;

  /* Reset to original timestamps.  */
  const struct timespec otsp[] =
  {
    { ost.st_atim.tv_sec, ost.st_atim.tv_nsec },
    { ost.st_mtim.tv_sec, ost.st_mtim.tv_nsec },
  };
  TEST_VERIFY_EXIT (utimensat (AT_FDCWD, path, otsp, 0) == 0);
#endif
  return support;
}
