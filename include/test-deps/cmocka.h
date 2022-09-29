// Derived from code with this license:
/*
 * Copyright 2008 Google Inc.
 * Copyright 2014-2022 Andreas Schneider <asn@cryptomilk.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#define assert_ptr_equal(a, b) \
    _assert_uint_equal(cast_ptr_to_uintmax_type(a), \
                       cast_ptr_to_uintmax_type(b), \
                       __FILE__, __LINE__)

#define assert_int_equal(a, b) \
    _assert_int_equal(cast_to_intmax_type(a), \
                      cast_to_intmax_type(b), \
                      __FILE__, __LINE__)

/* Perform an unsigned cast to uintmax_t. */
#define cast_to_uintmax_type(value) \
    ((uintmax_t)(value))

/* Perform an unsigned cast to uintptr_t. */
#define cast_to_uintptr_type(value) \
    ((uintptr_t)(value))

/* Perform a cast of a pointer to uintmax_t */
#define cast_ptr_to_uintmax_type(value) \
    cast_to_uintmax_type(cast_to_uintptr_type(value))

/* Perform an signed cast to intmax_t. */
#define cast_to_intmax_type(value) \
    ((intmax_t)(value))

/** Initializes a CMUnitTest structure. */
#define cmocka_unit_test(f) { #f, f, NULL, NULL, NULL }

# define cmocka_run_group_tests(group_tests, group_setup, group_teardown) \
        _cmocka_run_group_tests(#group_tests, group_tests, sizeof(group_tests) / sizeof((group_tests)[0]), group_setup, group_teardown)

#define CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT "%s:%u"

#if 1//defined(HAVE_GCC_THREAD_LOCAL_STORAGE)
# define CMOCKA_INTERNAL_CMOCKA_THREAD __thread
#elif 1//defined(HAVE_MSVC_THREAD_LOCAL_STORAGE)
# define CMOCKA_INTERNAL_CMOCKA_THREAD __declspec(thread)
#else
# define CMOCKA_INTERNAL_CMOCKA_THREAD
#endif

#define assert_true(c) _assert_true(cast_to_uintmax_type(c), #c, \
                                    __FILE__, __LINE__)

#define malloc test_malloc
#define realloc test_realloc
#define calloc test_calloc
#define free test_free

#define CMOCKA_INTERNAL_ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#if defined(__GNUC__)
#define CMOCKA_NORETURN __attribute__ ((noreturn))
#elif defined(_MSC_VER)
#define CMOCKA_NORETURN __declspec(noreturn)
#else
#define CMOCKA_NORETURN
#endif

#define assert_non_null(c) _assert_true(cast_ptr_to_uintmax_type(c), #c, \
                                        __FILE__, __LINE__)

/**
 * POSIX has sigsetjmp/siglongjmp, while Windows only has setjmp/longjmp.
 */
#if 1//def HAVE_SIGLONGJMP
# define CMOCKA_INTERNAL_cm_jmp_buf             sigjmp_buf
# define CMOCKA_INTERNAL_cm_setjmp(env)         sigsetjmp(env, 1)
# define CMOCKA_INTERNAL_cm_longjmp(env, val)   siglongjmp(env, val)
#else
# define CMOCKA_INTERNAL_cm_jmp_buf             jmp_buf
# define CMOCKA_INTERNAL_cm_setjmp(env)         setjmp(env)
# define CMOCKA_INTERNAL_cm_longjmp(env, val)   longjmp(env, val)
#endif

/**
 * This is a hack to fix warnings. The idea is to use this everywhere that we
 * get the "discarding const" warning by the compiler. That doesn't actually
 * fix the real issue, but marks the place and you can search the code for
 * discard_const.
 *
 * Please use this macro only when there is no other way to fix the warning.
 * We should use this function in only in a very few places.
 *
 * Also, please call this via the discard_const_p() macro interface, as that
 * makes the return type safe.
 */
#define CMOCKA_INTERNAL_discard_const(ptr) ((void *)((uintptr_t)(ptr)))

#define test_free(ptr) _test_free(ptr, __FILE__, __LINE__)

/**
 * Type-safe version of discard_const
 */
#define CMOCKA_INTERNAL_discard_const_p(type, ptr) ((type *)CMOCKA_INTERNAL_discard_const(ptr))

/* Size of guard bytes around dynamically allocated blocks. */
#define CMOCKA_INTERNAL_MALLOC_GUARD_SIZE 16
/* Pattern used to initialize guard blocks. */
#define CMOCKA_INTERNAL_MALLOC_GUARD_PATTERN 0xEF
/* Pattern used to initialize memory allocated with test_malloc(). */
#define CMOCKA_INTERNAL_MALLOC_ALLOC_PATTERN 0xBA
#define CMOCKA_INTERNAL_MALLOC_FREE_PATTERN 0xCD
/* Alignment of allocated blocks.  NOTE: This must be base2. */
#ifndef CMOCKA_INTERNAL_MALLOC_ALIGNMENT
#define CMOCKA_INTERNAL_MALLOC_ALIGNMENT sizeof(size_t)
#endif

#if 1//def HAVE_CLOCK_REALTIME
#define CMOCKA_INTERNAL_CMOCKA_CLOCK_GETTIME(clock_id, ts) clock_gettime((clock_id), (ts))
#else
#define CMOCKA_INTERNAL_CMOCKA_CLOCK_GETTIME(clock_id, ts)
#endif

/* Function prototype for test functions. */
typedef void (*CMUnitTestFunction)(void **state);

/* Function prototype for setup and teardown functions. */
typedef int (*CMFixtureFunction)(void **state);

struct CMUnitTest {
    const char *name;
    CMUnitTestFunction test_func;
    CMFixtureFunction setup_func;
    CMFixtureFunction teardown_func;
    void *initial_state;
};

/* Doubly linked list node. */
typedef struct CMOCKA_INTERNAL_ListNode {
    const void *value;
    int refcount;
    struct CMOCKA_INTERNAL_ListNode *next;
    struct CMOCKA_INTERNAL_ListNode *prev;
} CMOCKA_INTERNAL_ListNode;

enum CMOCKA_INTERNAL_CMUnitTestStatus {
    CMOCKA_INTERNAL_CM_TEST_NOT_STARTED,
    CMOCKA_INTERNAL_CM_TEST_PASSED,
    CMOCKA_INTERNAL_CM_TEST_FAILED,
    CMOCKA_INTERNAL_CM_TEST_ERROR,
    CMOCKA_INTERNAL_CM_TEST_SKIPPED,
};

struct CMOCKA_INTERNAL_CMUnitTestState {
    const CMOCKA_INTERNAL_ListNode *check_point; /* Check point of the test if there's a setup function. */
    const struct CMUnitTest *test; /* Point to array element in the tests we get passed */
    void *state; /* State associated with the test */
    const char *error_message; /* The error messages by the test */
    enum CMOCKA_INTERNAL_CMUnitTestStatus status; /* PASSED, FAILED, ABORT ... */
    double runtime; /* Time calculations */
};

enum cm_message_output {
    CM_OUTPUT_STANDARD = 1,
    CM_OUTPUT_STDOUT = 1, /* API compatiblity */
    CM_OUTPUT_SUBUNIT = 2,
    CM_OUTPUT_TAP = 4,
    CM_OUTPUT_XML = 8,
};

/* Location within some source code. */
typedef struct SourceLocation {
    const char* file;
    int line;
} SourceLocation;

/* Debug information for malloc(). */
struct CMOCKA_INTERNAL_MallocBlockInfoData {
    void* block;              /* Address of the block returned by malloc(). */
    size_t allocated_size;    /* Total size of the allocated block. */
    size_t size;              /* Request block size. */
    SourceLocation location;  /* Where the block was allocated. */
    CMOCKA_INTERNAL_ListNode node;            /* Node within list of all allocated blocks. */
};

typedef union {
    struct CMOCKA_INTERNAL_MallocBlockInfoData *data;
    char *ptr;
} CMOCKA_INTERNAL_MallocBlockInfo;

/*
 * Contains a list of values for a symbol.
 * NOTE: Each structure referenced by symbol_values_list_head must have a
 * SourceLocation as its' first member.
 */
typedef struct CMOCKA_INTERNAL_SymbolMapValue {
    const char *symbol_name;
    CMOCKA_INTERNAL_ListNode symbol_values_list_head;
} CMOCKA_INTERNAL_SymbolMapValue;

/* Where a particular ordering was located and its symbol name */
typedef struct CMOCKA_INTERNAL_FuncOrderingValue {
    SourceLocation location;
    const char * function;
} CMOCKA_INTERNAL_FuncOrderingValue;

/* Used by list_free() to deallocate values referenced by list nodes. */
typedef void (*CMOCKA_INTERNAL_CleanupListValue)(const void *value, void *cleanup_value_data);

enum CMOCKA_INTERNAL_cm_printf_type {
    CMOCKA_INTERNAL_PRINTF_TEST_START,
    CMOCKA_INTERNAL_PRINTF_TEST_SUCCESS,
    CMOCKA_INTERNAL_PRINTF_TEST_FAILURE,
    CMOCKA_INTERNAL_PRINTF_TEST_ERROR,
    CMOCKA_INTERNAL_PRINTF_TEST_SKIPPED,
};

static int CMOCKA_INTERNAL_cm_error_message_enabled = 1;
static CMOCKA_INTERNAL_CMOCKA_THREAD char *CMOCKA_INTERNAL_cm_error_message;

/* List of all currently allocated blocks. */
static CMOCKA_INTERNAL_CMOCKA_THREAD CMOCKA_INTERNAL_ListNode CMOCKA_INTERNAL_global_allocated_blocks;

static const char *CMOCKA_INTERNAL_global_test_filter_pattern;

static const char *CMOCKA_INTERNAL_global_skip_filter_pattern;

static uint32_t CMOCKA_INTERNAL_global_msg_output = CM_OUTPUT_STANDARD;

#ifndef _WIN32
/* Signals caught by exception_handler(). */
static const int CMOCKA_INTERNAL_exception_signals[] = {
    SIGFPE,
    SIGILL,
    SIGSEGV,
#ifdef SIGBUS
    SIGBUS,
#endif
#ifdef SIGSYS
    SIGSYS,
#endif
};

/* Default signal functions that should be restored after a test is complete. */
typedef void (*CMOCKA_INTERNAL_SignalFunction)(int signal);
static CMOCKA_INTERNAL_SignalFunction CMOCKA_INTERNAL_default_signal_functions[
    CMOCKA_INTERNAL_ARRAY_SIZE(CMOCKA_INTERNAL_exception_signals)];

#else /* _WIN32 */

/* The default exception filter. */
static LPTOP_LEVEL_EXCEPTION_FILTER CMOCKA_INTERNAL_previous_exception_filter;

/* Fatal exceptions. */
typedef struct CMOCKA_INTERNAL_ExceptionCodeInfo {
    DWORD code;
    const char* description;
} CMOCKA_INTERNAL_ExceptionCodeInfo;

#define CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(CMOCKA_INTERNAL_exception_code) {CMOCKA_INTERNAL_exception_code, #CMOCKA_INTERNAL_exception_code}

static const CMOCKA_INTERNAL_ExceptionCodeInfo CMOCKA_INTERNAL_exception_codes[] = {
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_ACCESS_VIOLATION),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_ARRAY_BOUNDS_EXCEEDED),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_DATATYPE_MISALIGNMENT),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_FLT_DENORMAL_OPERAND),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_FLT_DIVIDE_BY_ZERO),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_FLT_INEXACT_RESULT),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_FLT_INVALID_OPERATION),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_FLT_OVERFLOW),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_FLT_STACK_CHECK),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_FLT_UNDERFLOW),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_GUARD_PAGE),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_ILLEGAL_INSTRUCTION),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_INT_DIVIDE_BY_ZERO),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_INT_OVERFLOW),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_INVALID_DISPOSITION),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_INVALID_HANDLE),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_IN_PAGE_ERROR),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_NONCONTINUABLE_EXCEPTION),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_PRIV_INSTRUCTION),
    CMOCKA_INTERNAL_EXCEPTION_CODE_INFO(EXCEPTION_STACK_OVERFLOW),
};
#endif /* !_WIN32 */

/* Keeps a map of the values that functions will have to return to provide */
/* mocked interfaces. */
static CMOCKA_INTERNAL_CMOCKA_THREAD CMOCKA_INTERNAL_ListNode CMOCKA_INTERNAL_global_function_result_map_head;
/* Location of the last mock value returned was declared. */
static CMOCKA_INTERNAL_CMOCKA_THREAD SourceLocation CMOCKA_INTERNAL_global_last_mock_value_location;

/* Keeps a map of the values that functions expect as parameters to their
 * mocked interfaces. */
static CMOCKA_INTERNAL_CMOCKA_THREAD CMOCKA_INTERNAL_ListNode CMOCKA_INTERNAL_global_function_parameter_map_head;
/* Location of last parameter value checked was declared. */
static CMOCKA_INTERNAL_CMOCKA_THREAD SourceLocation CMOCKA_INTERNAL_global_last_parameter_location;

/* List (acting as FIFO) of call ordering. */
static CMOCKA_INTERNAL_CMOCKA_THREAD CMOCKA_INTERNAL_ListNode CMOCKA_INTERNAL_global_call_ordering_head;
/* Location of last call ordering that was declared. */
static CMOCKA_INTERNAL_CMOCKA_THREAD SourceLocation CMOCKA_INTERNAL_global_last_call_ordering_location;

/*
 * Keeps track of the calling context returned by setenv() so that the fail()
 * method can jump out of a test.
 */
static CMOCKA_INTERNAL_CMOCKA_THREAD CMOCKA_INTERNAL_cm_jmp_buf CMOCKA_INTERNAL_global_run_test_env;
static CMOCKA_INTERNAL_CMOCKA_THREAD int CMOCKA_INTERNAL_global_running_test = 0;

/* Keeps track of the calling context returned by setenv() so that */
/* mock_assert() can optionally jump back to expect_assert_failure(). */
static jmp_buf global_expect_assert_env;
static int global_expecting_assert = 0;
static const char *global_last_failed_assert = NULL;
static int CMOCKA_INTERNAL_global_skip_test;
static int CMOCKA_INTERNAL_global_stop_test;

static inline void vprint_error(const char* const format, va_list args)
{
    char buffer[4096];

    vsnprintf(buffer, sizeof(buffer), format, args);
    fprintf(stderr, "%s", buffer);
    fflush(stderr);
#ifdef _WIN32
    OutputDebugString(buffer);
#endif /* _WIN32 */
}

static inline void cmocka_print_error(const char * const format, ...)
{
    va_list args;
    va_start(args, format);
    /*if (CMOCKA_INTERNAL_cm_error_message_enabled) {
        vcmocka_print_error(format, args);
    } else*/ {
        vprint_error(format, args);
    }
    va_end(args);
}

/* Returns 1 if the specified values are equal.  If the values are not equal
 * an error is displayed and 0 is returned. */
static inline bool CMOCKA_INTERNAL_uint_values_equal_display_error(const uintmax_t left,
                                            const uintmax_t right)
{
    const bool equal = left == right;
    if (!equal) {
        cmocka_print_error("%ju (%#jx) != %ju (%#jx)\n",
                           left,
                           left,
                           right,
                           right);
    }
    return equal;
}

/* Exit the currently executing test. */
static inline void CMOCKA_INTERNAL_exit_test(const bool quit_application)
{
    /*const char *env = getenv("CMOCKA_TEST_ABORT");
    int abort_test = 0;

    if (env != NULL && strlen(env) == 1) {
        abort_test = (env[0] == '1');
    }

    if (CMOCKA_INTERNAL_global_skip_test == 0 && abort_test == 1) {
        if (CMOCKA_INTERNAL_cm_error_message != NULL) {
            print_error("%s", CMOCKA_INTERNAL_cm_error_message);
        }
        abort();
    } else if (CMOCKA_INTERNAL_global_running_test) {
        cm_longjmp(CMOCKA_INTERNAL_global_run_test_env, 1);
    } else if (quit_application)*/ {
        abort();//exit(EXIT_FAILURE);
    }
}

/* New formatter */
static inline uint32_t CMOCKA_INTERNAL_cm_get_output(void)
{
    return CMOCKA_INTERNAL_global_msg_output;
    /*static bool env_checked = false;
    char *env = NULL;
    size_t len = 0;
    uint32_t new_output = 0;
    char *str_output_list = NULL;
    char *str_output = NULL;
    char *saveptr = NULL;

    if (env_checked) {
        return CMOCKA_INTERNAL_global_msg_output;
    }

    env = getenv("CMOCKA_MESSAGE_OUTPUT");
    if (env == NULL) {
        return CMOCKA_INTERNAL_global_msg_output;
    }

    len = strlen(env);
    if (len == 0 || len > 32) {
        return CMOCKA_INTERNAL_global_msg_output;
    }

    str_output_list = strdup(env);
    if (str_output_list == NULL) {
        return CMOCKA_INTERNAL_global_msg_output;
    }

    for (str_output = strtok_r(str_output_list, ",", &saveptr);
         str_output != NULL;
         str_output = strtok_r(NULL, ",", &saveptr)) {
        if (strcasecmp(str_output, "STANDARD") == 0) {
            new_output |= CM_OUTPUT_STANDARD;
        } else if (strcasecmp(str_output, "STDOUT") == 0) {
            new_output |= CM_OUTPUT_STANDARD;
        } else if (strcasecmp(str_output, "SUBUNIT") == 0) {
            new_output |= CM_OUTPUT_SUBUNIT;
        } else if (strcasecmp(str_output, "TAP") == 0) {
            new_output |= CM_OUTPUT_TAP;
        } else if (strcasecmp(str_output, "XML") == 0) {
            new_output |= CM_OUTPUT_XML;
        }
    }

    libc_free(str_output_list);

    if (new_output != 0) {
        CMOCKA_INTERNAL_global_msg_output = new_output;
    }

    env_checked = true;

    return CMOCKA_INTERNAL_global_msg_output;*/
}

static inline void _fail(const char * const file, const int line) {
    uint32_t output = CMOCKA_INTERNAL_cm_get_output();

    if (output & CM_OUTPUT_STANDARD) {
        cmocka_print_error("[   LINE   ] --- " CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT
                       ": error: Failure!",
                       file, line);
    }
    if ((output & CM_OUTPUT_SUBUNIT) || (output & CM_OUTPUT_TAP) ||
        (output & CM_OUTPUT_XML)) {
        assert(false);//cmocka_print_error(CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT ": error: Failure!", file, line);
    }
    CMOCKA_INTERNAL_exit_test(true);

    /* Unreachable */
    abort();//exit(EXIT_FAILURE);
}

static inline void _assert_uint_equal(const uintmax_t a,
                        const uintmax_t b,
                        const char * const file,
                        const int line)
{
    if (!CMOCKA_INTERNAL_uint_values_equal_display_error(a, b)) {
        _fail(file, line);
    }
}

/* Returns 1 if the specified values are equal.  If the values are not equal
 * an error is displayed and 0 is returned. */
static inline bool CMOCKA_INTERNAL_int_values_equal_display_error(const intmax_t left,
                                           const intmax_t right)
{
    const bool equal = left == right;
    if (!equal) {
        cmocka_print_error("%jd != %jd\n", left, right);
    }
    return equal;
}

static inline void _assert_int_equal(const intmax_t a,
                       const intmax_t b,
                       const char * const file,
                       const int line)
{
    if (!CMOCKA_INTERNAL_int_values_equal_display_error(a, b)) {
        _fail(file, line);
    }
}

/* Initialize a list node. */
static inline CMOCKA_INTERNAL_ListNode* CMOCKA_INTERNAL_list_initialize(CMOCKA_INTERNAL_ListNode * const node) {
    node->value = NULL;
    node->next = node;
    node->prev = node;
    node->refcount = 1;
    return node;
}

/* Get the list of allocated blocks. */
static inline CMOCKA_INTERNAL_ListNode* CMOCKA_INTERNAL_get_allocated_blocks_list(void) {
    /* If it initialized, initialize the list of allocated blocks. */
    if (!CMOCKA_INTERNAL_global_allocated_blocks.value) {
        CMOCKA_INTERNAL_list_initialize(&CMOCKA_INTERNAL_global_allocated_blocks);
        CMOCKA_INTERNAL_global_allocated_blocks.value = (void*)1;
    }
    return &CMOCKA_INTERNAL_global_allocated_blocks;
}

/* Crudely checkpoint the current heap state. */
static inline const CMOCKA_INTERNAL_ListNode* CMOCKA_INTERNAL_check_point_allocated_blocks(void) {
    return CMOCKA_INTERNAL_get_allocated_blocks_list()->prev;
}

static inline void _assert_true(const uintmax_t result,
                  const char * const expression,
                  const char * const file, const int line) {
    if (!result) {
        cmocka_print_error("%s\n", expression);
        _fail(file, line);
    }
}

static inline void *CMOCKA_INTERNAL_libc_calloc(size_t nmemb, size_t size)
{
#undef calloc
    return calloc(nmemb, size);
#define calloc test_calloc
}

static inline int CMOCKA_INTERNAL_c_strmatch(const char *str, const char *pattern)
{
    int ok;

    if (str == NULL || pattern == NULL) {
        return 0;
    }

    for (;;) {
        /* Check if pattern is done */
        if (*pattern == '\0') {
            /* If string is at the end, we're good */
            if (*str == '\0') {
                return 1;
            }

            return 0;
        }

        if (*pattern == '*') {
            /* Move on */
            pattern++;

            /* If we are at the end, everything is fine */
            if (*pattern == '\0') {
                return 1;
            }

            /* Try to match each position */
            for (; *str != '\0'; str++) {
                ok = CMOCKA_INTERNAL_c_strmatch(str, pattern);
                if (ok) {
                    return 1;
                }
            }

            /* No match */
            return 0;
        }

        /* If we are at the end, leave */
        if (*str == '\0') {
            return 0;
        }

        /* Check if we have a single wildcard or matching char */
        if (*pattern != '?' && *str != *pattern) {
            return 0;
        }

        /* Move string and pattern */
        str++;
        pattern++;
    }

    return 0;
}

/* Standard output and error print methods. */
static inline void vprint_message(const char* const format, va_list args)
{
    char buffer[4096];

    vsnprintf(buffer, sizeof(buffer), format, args);
    printf("%s", buffer);
    fflush(stdout);
#ifdef _WIN32
    OutputDebugString(buffer);
#endif /* _WIN32 */
}

static inline void print_message(const char* const format, ...) {
    va_list args;
    va_start(args, format);
    vprint_message(format, args);
    va_end(args);
}

static inline void CMOCKA_INTERNAL_cmprintf_group_start_standard(const char *group_name,
                                          const size_t num_tests)
{
    /*print_message("[==========] %s: Running %zu test(s).\n",
                  group_name,
                  num_tests);*/
}

static inline void CMOCKA_INTERNAL_cmprintf_group_start(const char *group_name,
                                 const size_t num_tests)
{
    uint32_t output;

    output = CMOCKA_INTERNAL_cm_get_output();

    if (output & CM_OUTPUT_STANDARD) {
        CMOCKA_INTERNAL_cmprintf_group_start_standard(group_name, num_tests);
    }
    if (output & CM_OUTPUT_TAP) {
        assert(false);//CMOCKA_INTERNAL_cmprintf_group_start_tap(num_tests);
    }
}

#ifndef _WIN32
CMOCKA_NORETURN static inline void CMOCKA_INTERNAL_exception_handler(int sig) {
    const char *sig_strerror = "";

#if 1//def HAVE_STRSIGNAL
    sig_strerror = strsignal(sig);
#endif

    cmocka_print_error("Test failed with exception: %s(%d)",
                   sig_strerror, sig);
    CMOCKA_INTERNAL_exit_test(true);

    /* Unreachable */
    abort();//exit(EXIT_FAILURE);
}

#else /* _WIN32 */

static inline LONG WINAPI CMOCKA_INTERNAL_exception_filter(EXCEPTION_POINTERS *exception_pointers) {
    EXCEPTION_RECORD * const exception_record =
        exception_pointers->ExceptionRecord;
    const DWORD code = exception_record->ExceptionCode;
    unsigned int i;
    for (i = 0; i < CMOCKA_INTERNAL_ARRAY_SIZE(exception_codes); i++) {
        const ExceptionCodeInfo * const code_info = &exception_codes[i];
        if (code == code_info->code) {
            static int shown_debug_message = 0;
            fflush(stdout);
            cmocka_print_error("%s occurred at %p.\n", code_info->description,
                        exception_record->ExceptionAddress);
            if (!shown_debug_message) {
                cmocka_print_error(
                    "\n"
                    "To debug in Visual Studio...\n"
                    "1. Select menu item File->Open Project\n"
                    "2. Change 'Files of type' to 'Executable Files'\n"
                    "3. Open this executable.\n"
                    "4. Select menu item Debug->Start\n"
                    "\n"
                    "Alternatively, set the environment variable \n"
                    "UNIT_TESTING_DEBUG to 1 and rebuild this executable, \n"
                    "then click 'Debug' in the popup dialog box.\n"
                    "\n");
                shown_debug_message = 1;
            }
            CMOCKA_INTERNAL_exit_test(false);
            return EXCEPTION_EXECUTE_HANDLER;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif /* !_WIN32 */

/* Initialize a SourceLocation structure. */
static inline void CMOCKA_INTERNAL_initialize_source_location(SourceLocation * const location) {
    assert_non_null(location);
    location->file = NULL;
    location->line = 0;
}

/* Create function results and expected parameter lists. */
static inline void CMOCKA_INTERNAL_initialize_testing(const char *test_name) {
    (void)test_name;
    CMOCKA_INTERNAL_list_initialize(&CMOCKA_INTERNAL_global_function_result_map_head);
    CMOCKA_INTERNAL_initialize_source_location(&CMOCKA_INTERNAL_global_last_mock_value_location);
    CMOCKA_INTERNAL_list_initialize(&CMOCKA_INTERNAL_global_function_parameter_map_head);
    CMOCKA_INTERNAL_initialize_source_location(&CMOCKA_INTERNAL_global_last_parameter_location);
    CMOCKA_INTERNAL_list_initialize(&CMOCKA_INTERNAL_global_call_ordering_head);
    CMOCKA_INTERNAL_initialize_source_location(&CMOCKA_INTERNAL_global_last_parameter_location);
}

/* Display the blocks allocated after the specified check point.  This
 * function returns the number of blocks displayed. */
static inline size_t CMOCKA_INTERNAL_display_allocated_blocks(const CMOCKA_INTERNAL_ListNode * const check_point) {
    const CMOCKA_INTERNAL_ListNode * const head = CMOCKA_INTERNAL_get_allocated_blocks_list();
    const CMOCKA_INTERNAL_ListNode *node;
    size_t allocated_blocks = 0;
    assert_non_null(check_point);
    assert_non_null(check_point->next);

    for (node = check_point->next; node != head; node = node->next) {
        const CMOCKA_INTERNAL_MallocBlockInfo block_info = {
            .ptr = CMOCKA_INTERNAL_discard_const(node->value),
        };
        assert_non_null(block_info.ptr);

        if (allocated_blocks == 0) {
            cmocka_print_error("Blocks allocated...\n");
        }
        cmocka_print_error(CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT ": note: block %p allocated here\n",
                       block_info.data->location.file,
                       block_info.data->location.line,
                       block_info.data->block);
        allocated_blocks++;
    }
    return allocated_blocks;
}

/* Remove a node from a list. */
static inline CMOCKA_INTERNAL_ListNode* CMOCKA_INTERNAL_list_remove(
        CMOCKA_INTERNAL_ListNode * const node, const CMOCKA_INTERNAL_CleanupListValue cleanup_value,
        void * const cleanup_value_data) {
    assert_non_null(node);
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (cleanup_value) {
        cleanup_value(node->value, cleanup_value_data);
    }
    return node;
}

/* Use the real free in this function. */
#undef free
static inline void _test_free(void* const ptr, const char* file, const int line) {
    unsigned int i;
    char *block = CMOCKA_INTERNAL_discard_const_p(char, ptr);
    CMOCKA_INTERNAL_MallocBlockInfo block_info;

    if (ptr == NULL) {
        return;
    }

    _assert_true(cast_ptr_to_uintmax_type(ptr), "ptr", file, line);
    block_info.ptr = block - (CMOCKA_INTERNAL_MALLOC_GUARD_SIZE +
                              sizeof(struct CMOCKA_INTERNAL_MallocBlockInfoData));
    /* Check the guard blocks. */
    {
        char *guards[2] = {block - CMOCKA_INTERNAL_MALLOC_GUARD_SIZE,
                           block + block_info.data->size};
        for (i = 0; i < CMOCKA_INTERNAL_ARRAY_SIZE(guards); i++) {
            unsigned int j;
            char * const guard = guards[i];
            for (j = 0; j < CMOCKA_INTERNAL_MALLOC_GUARD_SIZE; j++) {
                const char diff = guard[j] - CMOCKA_INTERNAL_MALLOC_GUARD_PATTERN;
                if (diff) {
                    cmocka_print_error(CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT
                                   ": error: Guard block of %p size=%lu is corrupt\n"
                                   CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT ": note: allocated here at %p\n",
                                   file,
                                   line,
                                   ptr,
                                   (unsigned long)block_info.data->size,
                                   block_info.data->location.file,
                                   block_info.data->location.line,
                                   (void *)&guard[j]);
                    _fail(file, line);
                }
            }
        }
    }
    CMOCKA_INTERNAL_list_remove(&block_info.data->node, NULL, NULL);

    block = CMOCKA_INTERNAL_discard_const_p(char, block_info.data->block);
    memset(block, CMOCKA_INTERNAL_MALLOC_FREE_PATTERN, block_info.data->allocated_size);
    free(block);
}
#define free test_free

/* Free all blocks allocated after the specified check point. */
static inline void CMOCKA_INTERNAL_free_allocated_blocks(const CMOCKA_INTERNAL_ListNode * const check_point) {
    const CMOCKA_INTERNAL_ListNode * const head = CMOCKA_INTERNAL_get_allocated_blocks_list();
    const CMOCKA_INTERNAL_ListNode *node;
    assert_non_null(check_point);

    node = check_point->next;
    assert_non_null(node);

    while (node != head) {
        const CMOCKA_INTERNAL_MallocBlockInfo block_info = {
            .ptr = CMOCKA_INTERNAL_discard_const(node->value),
        };
        node = node->next;
        free(CMOCKA_INTERNAL_discard_const_p(char, block_info.data) +
             sizeof(struct CMOCKA_INTERNAL_MallocBlockInfoData) +
             CMOCKA_INTERNAL_MALLOC_GUARD_SIZE);
    }
}

/* Fail if any any blocks are allocated after the specified check point. */
static inline void CMOCKA_INTERNAL_fail_if_blocks_allocated(const CMOCKA_INTERNAL_ListNode * const check_point,
                                     const char * const test_name) {
    const size_t allocated_blocks = CMOCKA_INTERNAL_display_allocated_blocks(check_point);
    if (allocated_blocks > 0) {
        CMOCKA_INTERNAL_free_allocated_blocks(check_point);
        cmocka_print_error("ERROR: %s leaked %zu block(s)\n", test_name,
                       allocated_blocks);
        CMOCKA_INTERNAL_exit_test(true);
    }
}

/* Determine whether a list is empty. */
static inline int CMOCKA_INTERNAL_list_empty(const CMOCKA_INTERNAL_ListNode * const head) {
    assert_non_null(head);
    return head->next == head;
}

/* Remove a list node from a list and free the node. */
static inline void CMOCKA_INTERNAL_list_remove_free(
        CMOCKA_INTERNAL_ListNode * const node, const CMOCKA_INTERNAL_CleanupListValue cleanup_value,
        void * const cleanup_value_data) {
    assert_non_null(node);
    free(CMOCKA_INTERNAL_list_remove(node, cleanup_value, cleanup_value_data));
}

/* Deallocate a value referenced by a list. */
static inline void CMOCKA_INTERNAL_free_value(const void *value, void *cleanup_value_data) {
    (void)cleanup_value_data;
    assert_non_null(value);
    free((void*)value);
}

/*
 * Traverse down a tree of symbol values and remove the first symbol value
 * in each branch that has a refcount < -1 (i.e should always be returned
 * and has been returned at least once).
 */
static inline void CMOCKA_INTERNAL_remove_always_return_values(CMOCKA_INTERNAL_ListNode * const map_head,
                                        const size_t number_of_symbol_names) {
    CMOCKA_INTERNAL_ListNode *current;
    assert_non_null(map_head);
    assert_true(number_of_symbol_names);
    current = map_head->next;
    while (current != map_head) {
        CMOCKA_INTERNAL_SymbolMapValue * const value = (CMOCKA_INTERNAL_SymbolMapValue*)current->value;
        CMOCKA_INTERNAL_ListNode * const next = current->next;
        CMOCKA_INTERNAL_ListNode *child_list;
        assert_non_null(value);
        child_list = &value->symbol_values_list_head;

        if (!CMOCKA_INTERNAL_list_empty(child_list)) {
            if (number_of_symbol_names == 1) {
                CMOCKA_INTERNAL_ListNode * const child_node = child_list->next;
                /* If this item has been returned more than once, free it. */
                if (child_node->refcount < -1) {
                    CMOCKA_INTERNAL_list_remove_free(child_node, CMOCKA_INTERNAL_free_value, NULL);
                }
            } else {
                CMOCKA_INTERNAL_remove_always_return_values(child_list,
                                            number_of_symbol_names - 1);
            }
        }

        if (CMOCKA_INTERNAL_list_empty(child_list)) {
            CMOCKA_INTERNAL_list_remove_free(current, CMOCKA_INTERNAL_free_value, NULL);
        }
        current = next;
    }
}

/*
 * Checks if there are any leftover values set up by the test that were never
 * retrieved through execution, and fail the test if that is the case.
 */
static inline size_t CMOCKA_INTERNAL_check_for_leftover_values(
        const CMOCKA_INTERNAL_ListNode * const map_head, const char * const error_message,
        const size_t number_of_symbol_names) {
    const CMOCKA_INTERNAL_ListNode *current;
    size_t symbols_with_leftover_values = 0;
    assert_non_null(map_head);
    assert_true(number_of_symbol_names);

    for (current = map_head->next; current != map_head;
         current = current->next) {
        const CMOCKA_INTERNAL_SymbolMapValue * const value =
            (CMOCKA_INTERNAL_SymbolMapValue*)current->value;
        const CMOCKA_INTERNAL_ListNode *child_list;
        assert_non_null(value);
        child_list = &value->symbol_values_list_head;

        if (!CMOCKA_INTERNAL_list_empty(child_list)) {
            if (number_of_symbol_names == 1) {
                const CMOCKA_INTERNAL_ListNode *child_node;
                cmocka_print_error("%s: %s\n", error_message, value->symbol_name);

                for (child_node = child_list->next; child_node != child_list;
                     child_node = child_node->next) {
                    const SourceLocation * const location =
                        (const SourceLocation*)child_node->value;
                    cmocka_print_error(CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT
                                   ": note: remaining item was declared here\n",
                                   location->file, location->line);
                }
            } else {
                cmocka_print_error("%s: ", value->symbol_name);
                CMOCKA_INTERNAL_check_for_leftover_values(child_list, error_message,
                                          number_of_symbol_names - 1);
            }
            symbols_with_leftover_values ++;
        }
    }
    return symbols_with_leftover_values;
}

/**
 * Taverse a list of nodes and remove first symbol value in list that has a
 * refcount < -1 (i.e. should always be returned and has been returned at
 * least once).
 */

static inline void CMOCKA_INTERNAL_remove_always_return_values_from_list(CMOCKA_INTERNAL_ListNode * const map_head)
{
    CMOCKA_INTERNAL_ListNode * current = NULL;
    CMOCKA_INTERNAL_ListNode * next = NULL;
    assert_non_null(map_head);

    for (current = map_head->next, next = current->next;
            current != map_head;
            current = next, next = current->next) {
        if (current->refcount < -1) {
            CMOCKA_INTERNAL_list_remove_free(current, CMOCKA_INTERNAL_free_value, NULL);
        }
    }
}

static inline size_t CMOCKA_INTERNAL_check_for_leftover_values_list(const CMOCKA_INTERNAL_ListNode * head,
                                             const char * const error_message)
{
    CMOCKA_INTERNAL_ListNode *child_node;
    size_t leftover_count = 0;
    if (!CMOCKA_INTERNAL_list_empty(head))
    {
        for (child_node = head->next; child_node != head;
                 child_node = child_node->next, ++leftover_count) {
            const CMOCKA_INTERNAL_FuncOrderingValue *const o =
                    (const CMOCKA_INTERNAL_FuncOrderingValue*) child_node->value;
            cmocka_print_error("%s: %s\n", error_message, o->function);
            cmocka_print_error(CMOCKA_INTERNAL_SOURCE_LOCATION_FORMAT
                    ": note: remaining item was declared here\n",
                    o->location.file, o->location.line);
        }
    }
    return leftover_count;
}

static inline int CMOCKA_INTERNAL_has_leftover_values(const char *test_name) {
    int leftover_values = 0;
    (void)test_name;
    CMOCKA_INTERNAL_remove_always_return_values(&CMOCKA_INTERNAL_global_function_result_map_head, 1);
    if (CMOCKA_INTERNAL_check_for_leftover_values(
            &CMOCKA_INTERNAL_global_function_result_map_head,
            "Has remaining non-returned values", 1)) {
        leftover_values = 1;
    }

    CMOCKA_INTERNAL_remove_always_return_values(&CMOCKA_INTERNAL_global_function_parameter_map_head, 2);
    if (CMOCKA_INTERNAL_check_for_leftover_values(
            &CMOCKA_INTERNAL_global_function_parameter_map_head,
            "Parameter still has values that haven't been checked",
            2)) {
        leftover_values = 1;
    }

    CMOCKA_INTERNAL_remove_always_return_values_from_list(&CMOCKA_INTERNAL_global_call_ordering_head);
    if (CMOCKA_INTERNAL_check_for_leftover_values_list(&CMOCKA_INTERNAL_global_call_ordering_head,
        "Function was expected to be called but was not")) {
        leftover_values = 1;
    }
    return (leftover_values);
}

static inline void CMOCKA_INTERNAL_fail_if_leftover_values(const char *test_name) {
    if (CMOCKA_INTERNAL_has_leftover_values(test_name) != 0) {
        CMOCKA_INTERNAL_exit_test(true);
    }
}

/*
 * Frees memory kept by a linked list The cleanup_value function is called for
 * every "value" field of nodes in the list, except for the head.  In addition
 * to each list value, cleanup_value_data is passed to each call to
 * cleanup_value.  The head of the list is not deallocated.
 */
static inline CMOCKA_INTERNAL_ListNode* CMOCKA_INTERNAL_list_free(
        CMOCKA_INTERNAL_ListNode * const head, const CMOCKA_INTERNAL_CleanupListValue cleanup_value,
        void * const cleanup_value_data) {
    assert_non_null(head);
    while (!CMOCKA_INTERNAL_list_empty(head)) {
        CMOCKA_INTERNAL_list_remove_free(head->next, cleanup_value, cleanup_value_data);
    }
    return head;
}

/* Releases memory associated to a symbol_map_value. */
static inline void CMOCKA_INTERNAL_free_symbol_map_value(const void *value,
                                  void *cleanup_value_data) {
    CMOCKA_INTERNAL_SymbolMapValue * const map_value = (CMOCKA_INTERNAL_SymbolMapValue*)value;
    const uintmax_t children = cast_ptr_to_uintmax_type(cleanup_value_data);
    assert_non_null(value);
    if (children == 0) {
        CMOCKA_INTERNAL_list_free(&map_value->symbol_values_list_head,
                  CMOCKA_INTERNAL_free_value,
                  NULL);
    } else {
        CMOCKA_INTERNAL_list_free(&map_value->symbol_values_list_head,
                  CMOCKA_INTERNAL_free_symbol_map_value,
                  (void *)((uintptr_t)children - 1));
    }

    free(map_value);
}

static inline void CMOCKA_INTERNAL_teardown_testing(const char *test_name) {
    (void)test_name;
    CMOCKA_INTERNAL_list_free(&CMOCKA_INTERNAL_global_function_result_map_head, CMOCKA_INTERNAL_free_symbol_map_value,
              (void*)0);
    CMOCKA_INTERNAL_initialize_source_location(&CMOCKA_INTERNAL_global_last_mock_value_location);
    CMOCKA_INTERNAL_list_free(&CMOCKA_INTERNAL_global_function_parameter_map_head, CMOCKA_INTERNAL_free_symbol_map_value,
              (void*)1);
    CMOCKA_INTERNAL_initialize_source_location(&CMOCKA_INTERNAL_global_last_parameter_location);
    CMOCKA_INTERNAL_list_free(&CMOCKA_INTERNAL_global_call_ordering_head, CMOCKA_INTERNAL_free_value,
              (void*)0);
    CMOCKA_INTERNAL_initialize_source_location(&CMOCKA_INTERNAL_global_last_call_ordering_location);
}

/****************************************************************************
 * CMOCKA TEST RUNNER
 ****************************************************************************/
static inline int CMOCKA_INTERNAL_cmocka_run_one_test_or_fixture(const char *function_name,
                                          CMUnitTestFunction test_func,
                                          CMFixtureFunction setup_func,
                                          CMFixtureFunction teardown_func,
                                          void ** const volatile state,
                                          const void *const heap_check_point)
{
    const CMOCKA_INTERNAL_ListNode * const volatile check_point = (const CMOCKA_INTERNAL_ListNode*)
        (heap_check_point != NULL ?
         heap_check_point : CMOCKA_INTERNAL_check_point_allocated_blocks());
    int handle_exceptions = 1;
    void *current_state = NULL;
    int rc = 0;

    /* FIXME check only one test or fixture is set */

    /* Detect if we should handle exceptions */
#ifdef _WIN32
    handle_exceptions = !IsDebuggerPresent();
#endif /* _WIN32 */
#ifdef UNIT_TESTING_DEBUG
    handle_exceptions = 0;
#endif /* UNIT_TESTING_DEBUG */


    if (handle_exceptions) {
#ifndef _WIN32
        unsigned int i;
        for (i = 0; i < CMOCKA_INTERNAL_ARRAY_SIZE(CMOCKA_INTERNAL_exception_signals); i++) {
            CMOCKA_INTERNAL_default_signal_functions[i] = signal(
                    CMOCKA_INTERNAL_exception_signals[i], CMOCKA_INTERNAL_exception_handler);
        }
#else /* _WIN32 */
        previous_exception_filter = SetUnhandledExceptionFilter(
                exception_filter);
#endif /* !_WIN32 */
    }

    /* Init the test structure */
    CMOCKA_INTERNAL_initialize_testing(function_name);

    CMOCKA_INTERNAL_global_running_test = 1;

    if (CMOCKA_INTERNAL_cm_setjmp(CMOCKA_INTERNAL_global_run_test_env) == 0) {
        if (test_func != NULL) {
            test_func(state != NULL ? state : &current_state);

            CMOCKA_INTERNAL_fail_if_blocks_allocated(check_point, function_name);
            rc = 0;
        } else if (setup_func != NULL) {
            rc = setup_func(state != NULL ? state : &current_state);

            /*
             * For setup we can ignore any allocated blocks. We just need to
             * ensure they're deallocated on tear down.
             */
        } else if (teardown_func != NULL) {
            rc = teardown_func(state != NULL ? state : &current_state);

            CMOCKA_INTERNAL_fail_if_blocks_allocated(check_point, function_name);
        } else {
            /* ERROR */
        }
        CMOCKA_INTERNAL_fail_if_leftover_values(function_name);
        CMOCKA_INTERNAL_global_running_test = 0;
    } else {
        /* TEST FAILED */
        CMOCKA_INTERNAL_global_running_test = 0;
        rc = -1;
        if (CMOCKA_INTERNAL_global_stop_test) {
            if (CMOCKA_INTERNAL_has_leftover_values(function_name) == 0) {
                rc = 0;
            }
            CMOCKA_INTERNAL_global_stop_test = 0;
        }
    }
    CMOCKA_INTERNAL_teardown_testing(function_name);

    if (handle_exceptions) {
#ifndef _WIN32
        unsigned int i;
        for (i = 0; i < CMOCKA_INTERNAL_ARRAY_SIZE(CMOCKA_INTERNAL_exception_signals); i++) {
            signal(CMOCKA_INTERNAL_exception_signals[i], CMOCKA_INTERNAL_default_signal_functions[i]);
        }
#else /* _WIN32 */
        if (previous_exception_filter) {
            SetUnhandledExceptionFilter(previous_exception_filter);
            previous_exception_filter = NULL;
        }
#endif /* !_WIN32 */
    }

    return rc;
}

static inline int CMOCKA_INTERNAL_cmocka_run_group_fixture(const char *function_name,
                                    CMFixtureFunction setup_func,
                                    CMFixtureFunction teardown_func,
                                    void **state,
                                    const void *const heap_check_point)
{
    int rc;

    if (setup_func != NULL) {
        rc = CMOCKA_INTERNAL_cmocka_run_one_test_or_fixture(function_name,
                                        NULL,
                                        setup_func,
                                        NULL,
                                        state,
                                        heap_check_point);
    } else {
        rc = CMOCKA_INTERNAL_cmocka_run_one_test_or_fixture(function_name,
                                        NULL,
                                        NULL,
                                        teardown_func,
                                        state,
                                        heap_check_point);
    }

    return rc;
}

static inline void print_error(const char* const format, ...) {
    va_list args;
    va_start(args, format);
    vprint_error(format, args);
    va_end(args);
}

static inline void CMOCKA_INTERNAL_cmprintf_standard(enum CMOCKA_INTERNAL_cm_printf_type type,
                              const char *test_name,
                              const char *error_message)
{
    switch (type) {
    case CMOCKA_INTERNAL_PRINTF_TEST_START:
        //print_message("[ RUN      ] %s\n", test_name);
        break;
    case CMOCKA_INTERNAL_PRINTF_TEST_SUCCESS:
        //print_message("[       OK ] %s\n", test_name);
        break;
    case CMOCKA_INTERNAL_PRINTF_TEST_FAILURE:
        if (error_message != NULL) {
            print_error("[  ERROR   ] --- %s\n", error_message);
        }
        print_message("[  FAILED  ] %s\n", test_name);
        break;
    case CMOCKA_INTERNAL_PRINTF_TEST_SKIPPED:
        print_message("[  SKIPPED ] %s\n", test_name);
        break;
    case CMOCKA_INTERNAL_PRINTF_TEST_ERROR:
        if (error_message != NULL) {
            print_error("%s\n", error_message);
        }
        print_error("[  ERROR   ] %s\n", test_name);
        break;
    }
}

static inline void CMOCKA_INTERNAL_cmprintf(enum CMOCKA_INTERNAL_cm_printf_type type,
                     size_t test_number,
                     const char *test_name,
                     const char *error_message)
{
    uint32_t output;

    output = CMOCKA_INTERNAL_cm_get_output();

    if (output & CM_OUTPUT_STANDARD) {
        CMOCKA_INTERNAL_cmprintf_standard(type, test_name, error_message);
    }
    if (output & CM_OUTPUT_SUBUNIT) {
        assert(false);//CMOCKA_INTERNAL_cmprintf_subunit(type, test_name, error_message);
    }
    if (output & CM_OUTPUT_TAP) {
        assert(false);//CMOCKA_INTERNAL_cmprintf_tap(type, test_number, test_name, error_message);
    }
}

/****************************************************************************
 * TIME CALCULATIONS
 ****************************************************************************/

#if 1//def HAVE_STRUCT_TIMESPEC
static inline struct timespec CMOCKA_INTERNAL_cm_tspecdiff(struct timespec time1,
                                    struct timespec time0)
{
    struct timespec ret;
    int xsec = 0;
    int sign = 1;

    if (time0.tv_nsec > time1.tv_nsec) {
        xsec = (int) ((time0.tv_nsec - time1.tv_nsec) / (1E9 + 1));
        time0.tv_nsec -= (long int) (1E9 * xsec);
        time0.tv_sec += xsec;
    }

    if ((time1.tv_nsec - time0.tv_nsec) > 1E9) {
        xsec = (int) ((time1.tv_nsec - time0.tv_nsec) / 1E9);
        time0.tv_nsec += (long int) (1E9 * xsec);
        time0.tv_sec -= xsec;
    }

    ret.tv_sec = time1.tv_sec - time0.tv_sec;
    ret.tv_nsec = time1.tv_nsec - time0.tv_nsec;

    if (time1.tv_sec < time0.tv_sec) {
        sign = -1;
    }

    ret.tv_sec = ret.tv_sec * sign;

    return ret;
}

static inline double CMOCKA_INTERNAL_cm_secdiff(struct timespec clock1, struct timespec clock0)
{
    double ret;
    struct timespec diff;

    diff = CMOCKA_INTERNAL_cm_tspecdiff(clock1, clock0);

    ret = (double) diff.tv_sec;
    ret += (double) diff.tv_nsec / (double) 1E9;

    return ret;
}
#endif /* HAVE_STRUCT_TIMESPEC */

static inline int CMOCKA_INTERNAL_cmocka_run_one_tests(struct CMOCKA_INTERNAL_CMUnitTestState *test_state)
{
#if 1//def HAVE_STRUCT_TIMESPEC
    struct timespec start = {
        .tv_sec = 0,
        .tv_nsec = 0,
    };
    struct timespec finish = {
        .tv_sec = 0,
        .tv_nsec = 0,
    };
#endif
    int rc = 0;

    /* Run setup */
    if (test_state->test->setup_func != NULL) {
        /* Setup the memory check point, it will be evaluated on teardown */
        test_state->check_point = CMOCKA_INTERNAL_check_point_allocated_blocks();

        rc = CMOCKA_INTERNAL_cmocka_run_one_test_or_fixture(test_state->test->name,
                                            NULL,
                                            test_state->test->setup_func,
                                            NULL,
                                            &test_state->state,
                                            test_state->check_point);
        if (rc != 0) {
            test_state->status = CMOCKA_INTERNAL_CM_TEST_ERROR;
            cmocka_print_error("Test setup failed");
        }
    }

    /* Run test */
#if 1//def HAVE_STRUCT_TIMESPEC
    CMOCKA_INTERNAL_CMOCKA_CLOCK_GETTIME(CLOCK_REALTIME, &start);
#endif

    if (rc == 0) {
        rc = CMOCKA_INTERNAL_cmocka_run_one_test_or_fixture(test_state->test->name,
                                            test_state->test->test_func,
                                            NULL,
                                            NULL,
                                            &test_state->state,
                                            NULL);
        if (rc == 0) {
            test_state->status = CMOCKA_INTERNAL_CM_TEST_PASSED;
        } else {
            if (CMOCKA_INTERNAL_global_skip_test) {
                test_state->status = CMOCKA_INTERNAL_CM_TEST_SKIPPED;
                CMOCKA_INTERNAL_global_skip_test = 0; /* Do not skip the next test */
            } else {
                test_state->status = CMOCKA_INTERNAL_CM_TEST_FAILED;
            }
        }
        rc = 0;
    }

    test_state->runtime = 0.0;

#if 1//def HAVE_STRUCT_TIMESPEC
    CMOCKA_INTERNAL_CMOCKA_CLOCK_GETTIME(CLOCK_REALTIME, &finish);
    test_state->runtime = CMOCKA_INTERNAL_cm_secdiff(finish, start);
#endif

    /* Run teardown */
    if (rc == 0 && test_state->test->teardown_func != NULL) {
        rc = CMOCKA_INTERNAL_cmocka_run_one_test_or_fixture(test_state->test->name,
                                            NULL,
                                            NULL,
                                            test_state->test->teardown_func,
                                            &test_state->state,
                                            test_state->check_point);
        if (rc != 0) {
            test_state->status = CMOCKA_INTERNAL_CM_TEST_ERROR;
            cmocka_print_error("Test teardown failed");
        }
    }

    test_state->error_message = CMOCKA_INTERNAL_cm_error_message;
    CMOCKA_INTERNAL_cm_error_message = NULL;

    return rc;
}

static inline void CMOCKA_INTERNAL_libc_free(void *ptr)
{
#undef free
    free(ptr);
#define free test_free
}

static inline void CMOCKA_INTERNAL_vcm_free_error(char *err_msg)
{
    CMOCKA_INTERNAL_libc_free(err_msg);
}

static inline void CMOCKA_INTERNAL_cmprintf_group_finish_standard(const char *group_name,
                                           size_t total_executed,
                                           size_t total_passed,
                                           size_t total_failed,
                                           size_t total_errors,
                                           size_t total_skipped,
                                           struct CMOCKA_INTERNAL_CMUnitTestState *cm_tests)
{
    size_t i;

    /*print_message("[==========] %s: %zu test(s) run.\n",
                  group_name,
                  total_executed);
    print_error("[  PASSED  ] %u test(s).\n",
                (unsigned)(total_passed));*/

    if (total_skipped) {
        print_error("[  SKIPPED ] %s: %zu test(s), listed below:\n",
                    group_name,
                    total_skipped);
        for (i = 0; i < total_executed; i++) {
            struct CMOCKA_INTERNAL_CMUnitTestState *cmtest = &cm_tests[i];

            if (cmtest->status == CMOCKA_INTERNAL_CM_TEST_SKIPPED) {
                print_error("[  SKIPPED ] %s\n", cmtest->test->name);
            }
        }
        print_error("\n %zu SKIPPED TEST(S)\n", total_skipped);
    }

    if (total_failed) {
        print_error("[  FAILED  ] %s: %zu test(s), listed below:\n",
                    group_name,
                    total_failed);
        for (i = 0; i < total_executed; i++) {
            struct CMOCKA_INTERNAL_CMUnitTestState *cmtest = &cm_tests[i];

            if (cmtest->status == CMOCKA_INTERNAL_CM_TEST_FAILED) {
                print_error("[  FAILED  ] %s\n", cmtest->test->name);
            }
        }
        print_error("\n %zu FAILED TEST(S)\n",
                    (total_failed + total_errors));
    }
}

static inline void CMOCKA_INTERNAL_cmprintf_group_finish(const char *group_name,
                                  size_t total_executed,
                                  size_t total_passed,
                                  size_t total_failed,
                                  size_t total_errors,
                                  size_t total_skipped,
                                  double total_runtime,
                                  struct CMOCKA_INTERNAL_CMUnitTestState *cm_tests)
{
    uint32_t output;

    output = CMOCKA_INTERNAL_cm_get_output();

    if (output & CM_OUTPUT_STANDARD) {
        CMOCKA_INTERNAL_cmprintf_group_finish_standard(group_name,
                                       total_executed,
                                       total_passed,
                                       total_failed,
                                       total_errors,
                                       total_skipped,
                                       cm_tests);
    }
    if (output & CM_OUTPUT_TAP) {
        assert(false);/*CMOCKA_INTERNAL_cmprintf_group_finish_tap(group_name,
                                  total_executed,
                                  total_passed,
                                  total_skipped);*/
    }
    if (output & CM_OUTPUT_XML) {
        assert(false);/*CMOCKA_INTERNAL_cmprintf_group_finish_xml(group_name,
                                  total_executed,
                                  total_failed,
                                  total_errors,
                                  total_skipped,
                                  total_runtime,
                                  cm_tests);*/
    }
}

static inline int _cmocka_run_group_tests(const char *group_name,
                            const struct CMUnitTest * const tests,
                            const size_t num_tests,
                            CMFixtureFunction group_setup,
                            CMFixtureFunction group_teardown)
{
    struct CMOCKA_INTERNAL_CMUnitTestState *cm_tests;
    const CMOCKA_INTERNAL_ListNode *group_check_point = CMOCKA_INTERNAL_check_point_allocated_blocks();
    void *group_state = NULL;
    size_t total_tests = 0;
    size_t total_failed = 0;
    size_t total_passed = 0;
    size_t total_executed = 0;
    size_t total_errors = 0;
    size_t total_skipped = 0;
    double total_runtime = 0;
    size_t i;
    int rc;

    /* Make sure uintmax_t is at least the size of a pointer. */
    assert_true(sizeof(uintmax_t) >= sizeof(void*));

    cm_tests = CMOCKA_INTERNAL_libc_calloc(1, sizeof(struct CMOCKA_INTERNAL_CMUnitTestState) * num_tests);
    if (cm_tests == NULL) {
        return -1;
    }

    /* Setup cmocka test array */
    for (i = 0; i < num_tests; i++) {
        if (tests[i].name != NULL &&
            (tests[i].test_func != NULL
             || tests[i].setup_func != NULL
             || tests[i].teardown_func != NULL)) {
            if (CMOCKA_INTERNAL_global_test_filter_pattern != NULL) {
                int match;

                match = CMOCKA_INTERNAL_c_strmatch(tests[i].name, CMOCKA_INTERNAL_global_test_filter_pattern);
                if (!match) {
                    continue;
                }
            }
            if (CMOCKA_INTERNAL_global_skip_filter_pattern != NULL) {
                int match;

                match = CMOCKA_INTERNAL_c_strmatch(tests[i].name, CMOCKA_INTERNAL_global_skip_filter_pattern);
                if (match) {
                    continue;
                }
            }
            cm_tests[total_tests] = (struct CMOCKA_INTERNAL_CMUnitTestState) {
                .test = &tests[i],
                .status = CMOCKA_INTERNAL_CM_TEST_NOT_STARTED,
                .state = NULL,
            };
            total_tests++;
        }
    }

    CMOCKA_INTERNAL_cmprintf_group_start(group_name, total_tests);

    rc = 0;

    /* Run group setup */
    if (group_setup != NULL) {
        rc = CMOCKA_INTERNAL_cmocka_run_group_fixture("cmocka_group_setup",
                                      group_setup,
                                      NULL,
                                      &group_state,
                                      group_check_point);
    }

    if (rc == 0) {
        /* Execute tests */
        for (i = 0; i < total_tests; i++) {
            struct CMOCKA_INTERNAL_CMUnitTestState *cmtest = &cm_tests[i];
            size_t test_number = i + 1;

            CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_START, test_number, cmtest->test->name, NULL);

            if (group_state != NULL) {
                cmtest->state = group_state;
            } else if (cmtest->test->initial_state  != NULL) {
                cmtest->state = cmtest->test->initial_state;
            }

            rc = CMOCKA_INTERNAL_cmocka_run_one_tests(cmtest);
            total_executed++;
            total_runtime += cmtest->runtime;
            if (rc == 0) {
                switch (cmtest->status) {
                    case CMOCKA_INTERNAL_CM_TEST_PASSED:
                        CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_SUCCESS,
                                 test_number,
                                 cmtest->test->name,
                                 cmtest->error_message);
                        total_passed++;
                        break;
                    case CMOCKA_INTERNAL_CM_TEST_SKIPPED:
                        CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_SKIPPED,
                                 test_number,
                                 cmtest->test->name,
                                 cmtest->error_message);
                        total_skipped++;
                        break;
                    case CMOCKA_INTERNAL_CM_TEST_FAILED:
                        CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_FAILURE,
                                 test_number,
                                 cmtest->test->name,
                                 cmtest->error_message);
                        total_failed++;
                        break;
                    default:
                        CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_ERROR,
                                 test_number,
                                 cmtest->test->name,
                                 "Internal cmocka error");
                        total_errors++;
                        break;
                }
            } else {
                char err_msg[2048] = {0};

                snprintf(err_msg, sizeof(err_msg),
                         "Could not run test: %s",
                         cmtest->error_message);

                CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_ERROR,
                         test_number,
                         cmtest->test->name,
                         err_msg);
                total_errors++;
            }
        }
    } else {
        if (CMOCKA_INTERNAL_cm_error_message != NULL) {
            print_error("[  ERROR   ] --- %s\n", CMOCKA_INTERNAL_cm_error_message);
            CMOCKA_INTERNAL_vcm_free_error(CMOCKA_INTERNAL_cm_error_message);
            CMOCKA_INTERNAL_cm_error_message = NULL;
        }
        CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_ERROR, 0,
                 group_name, "[  FAILED  ] GROUP SETUP");
        total_errors++;
    }

    /* Run group teardown */
    if (group_teardown != NULL) {
        rc = CMOCKA_INTERNAL_cmocka_run_group_fixture("cmocka_group_teardown",
                                      NULL,
                                      group_teardown,
                                      &group_state,
                                      group_check_point);
        if (rc != 0) {
            if (CMOCKA_INTERNAL_cm_error_message != NULL) {
                print_error("[  ERROR   ] --- %s\n", CMOCKA_INTERNAL_cm_error_message);
                CMOCKA_INTERNAL_vcm_free_error(CMOCKA_INTERNAL_cm_error_message);
                CMOCKA_INTERNAL_cm_error_message = NULL;
            }
            CMOCKA_INTERNAL_cmprintf(CMOCKA_INTERNAL_PRINTF_TEST_ERROR, 0,
                     group_name, "[  FAILED  ] GROUP TEARDOWN");
        }
    }

    CMOCKA_INTERNAL_cmprintf_group_finish(group_name,
                          total_executed,
                          total_passed,
                          total_failed,
                          total_errors,
                          total_skipped,
                          total_runtime,
                          cm_tests);

    for (i = 0; i < total_tests; i++) {
        CMOCKA_INTERNAL_vcm_free_error(CMOCKA_INTERNAL_discard_const_p(char, cm_tests[i].error_message));
    }
    CMOCKA_INTERNAL_libc_free(cm_tests);
    CMOCKA_INTERNAL_fail_if_blocks_allocated(group_check_point, "cmocka_group_tests");

    return (int)(total_failed + total_errors);
}
