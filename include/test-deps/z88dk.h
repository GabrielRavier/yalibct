// Derived from code from z88dk which uses "The Clarified Artistic License"

#pragma once

#include <stdio.h>
#include <setjmp.h>
#include <stddef.h>

#define Assert(r,m) Assert_real((r), __FILE__, __LINE__, (m))

static char   *failed_message;
static char   *failed_file;
static int     failed_line;
static jmp_buf jmpbuf;

static void Assert_real(int result, char *file, int line, char *message)
{
    if ( result == 0 ) {
        if ( failed_message == NULL ) {
            failed_file = file;
            failed_line = line;
            failed_message = message;
        }
        longjmp(jmpbuf, 1);
    }
}

#ifndef MAX_TESTS
#define MAX_TESTS 300
#endif

typedef struct {
    const char     *name;
    const char     *testnames[MAX_TESTS];
    void     *tests[MAX_TESTS];
    int       num_tests;
    void    (*setup)();
    void    (*teardown)();
} Suite;

static Suite   suite;

static void suite_setup(char *suitename)
{
    suite.name  = suitename;
    suite.num_tests = 0;
    suite.setup = suite.teardown = NULL;
}

#define suite_add_test(f) suite_add_test_real("" # f "", f)

void suite_add_test_real(char *testname, void (*test)())
{
    int    i;

    i = suite.num_tests++;
    suite.testnames[i] = testname;
    suite.tests[i] = test;
}

static int     passed;
static int     failed;

int suite_run()
{
    volatile int stage;
    int      i;
    const char *extra;
    void    (*func)();

    passed = failed = 0;

    //printf("Starting suite %s (%d tests)\n",suite.name, suite.num_tests);

    for ( i = 0; i < suite.num_tests; i++ ) {
        stage = 0;
        if ( setjmp(jmpbuf) == 0 ) {
#if 0//ndef NO_LOG_RUNNING
            printf("Running test %s..",suite.testnames[i]);
#endif
            failed_message = NULL;
            if ( suite.setup ) {
                suite.setup();
            }
            ++stage;
            func = suite.tests[i];
            func();
            ++stage;
            if ( suite.teardown ) {
                suite.teardown();
            }
#if 0//ndef NO_LOG_PASSED
#ifdef NO_LOG_RUNNING
            printf("Running test %s..",suite.testnames[i]);
#endif
            printf("...passed\n");
#endif
            passed++;
        } else {
            extra = "";
            switch ( stage ) {
            case 0:
                extra = "(in setup) ";
                /* Fall through */
            case 1:
                /* Protect ourselves against teardown failing */
                if ( setjmp(jmpbuf) == 0 ) {
                    if ( suite.teardown != NULL ) {
                        suite.teardown();
                    }
                } else {
                    stage = 3;
                }
                break;
            case 2:
                extra = "(in teardown) ";
                break;
            }
#ifdef NO_LOG_RUNNING
            printf("Running test %s..",suite.testnames[i]);
#endif
            printf("...failed %s%s:%d (%s)%s\n",extra,failed_file, failed_line, failed_message,stage == 3 ? " (and in teardown)" : "");
            failed++;
        }
    }

    //printf("%d run, %d passed, %d failed\n", suite.num_tests, passed, failed);

    return failed != 0;
}
