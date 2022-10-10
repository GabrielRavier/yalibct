// Derived from code with this license:
/* Copyright (C) 2009-2022 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#pragma once

#include "test-lib/compiler-features.h"

#include <stdio.h>
#include <stdlib.h>

#if defined __MACH__ && defined __APPLE__
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/thread_status.h>
#include <mach/exception.h>
#include <mach/task.h>
#include <pthread.h>
#elif defined _WIN32 && ! defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winerror.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

/* Check that the function FN takes the specified arguments ARGS with
   a return type of RET.  This header is designed to be included after
   <config.h> and the one system header that is supposed to contain
   the function being checked, but prior to any other system headers
   that are necessary for the unit test.  Therefore, this file does
   not include any system headers, nor reference anything outside of
   the macro arguments.  For an example, if foo.h should provide:

   extern int foo (char, float);

   then the unit test named test-foo.c would start out with:

   #include <config.h>
   #include <foo.h>
   #include "signature.h"
   SIGNATURE_CHECK (foo, int, (char, float));
   #include <other.h>
   ...
*/
# define SIGNATURE_CHECK(fn, ret, args) \
  SIGNATURE_CHECK1 (fn, ret, args, __LINE__)

/* Necessary to allow multiple SIGNATURE_CHECK lines in a unit test.
   Note that the checks must not occupy the same line.  */
# define SIGNATURE_CHECK1(fn, ret, args, id) \
  SIGNATURE_CHECK2 (fn, ret, args, id) /* macroexpand line */
# define SIGNATURE_CHECK2(fn, ret, args, id) \
  _GL_UNUSED static ret (*signature_check ## id) args = fn
#define _GL_ATTRIBUTE_MAYBE_UNUSED YALIBCT_ATTRIBUTE_MAYBE_UNUSED
#define _GL_UNUSED _GL_ATTRIBUTE_MAYBE_UNUSED

#define SIZEOF(array) (sizeof (array) / sizeof (array[0]))

/* Define ASSERT_STREAM before including this file if ASSERT must
   target a stream other than stderr.  */
#ifndef ASSERT_STREAM
# define ASSERT_STREAM stderr
#endif


/* ASSERT (condition);
   verifies that the specified condition is fulfilled.  If not, a message
   is printed to ASSERT_STREAM if defined (defaulting to stderr if
   undefined) and the program is terminated with an error code.

   This macro has the following properties:
     - The programmer specifies the expected condition, not the failure
       condition.  This simplifies thinking.
     - The condition is tested always, regardless of compilation flags.
       (Unlike the macro from <assert.h>.)
     - On Unix platforms, the tester can debug the test program with a
       debugger (provided core dumps are enabled: "ulimit -c unlimited").
     - For the sake of platforms where no debugger is available (such as
       some mingw systems), an error message is printed on the error
       stream that includes the source location of the ASSERT invocation.
 */
#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
        {                                                                    \
          (void)fprintf (ASSERT_STREAM, "%s:%d: assertion '%s' failed\n",    \
                   __FILE__, __LINE__, #expr);                               \
          (void)fflush (ASSERT_STREAM);                                      \
          abort ();                                                          \
        }                                                                    \
    }                                                                        \
  while (0)

/* The Microsoft MSVC 9 compiler chokes on the expression 1.0 / 0.0.
   The IBM XL C compiler on z/OS complains.
   PGI 16.10 complains.  */
#if defined _MSC_VER || (defined __MVS__ && defined __IBMC__) || defined __PGI
static double
Infinityd ()
{
  static double zero = 0.0;
  return 1.0 / zero;
}
#else
# define Infinityd() (1.0 / 0.0)
#endif

#if defined __MACH__ && defined __APPLE__
/* The exception port on which our thread listens.  */
static mach_port_t gnulib_nocrash_init_internal_our_exception_port;
/* The main function of the thread listening for exceptions of type
   EXC_BAD_ACCESS.  */
static inline void *
gnulib_nocrash_init_internal_mach_exception_thread (void *arg)
{
  /* Buffer for a message to be received.  */
  struct {
    mach_msg_header_t head;
    mach_msg_body_t msgh_body;
    char data[1024];
  } msg;
  mach_msg_return_t retval;
  /* Wait for a message on the exception port.  */
  retval = mach_msg (&msg.head, MACH_RCV_MSG | MACH_RCV_LARGE, 0, sizeof (msg),
                     gnulib_nocrash_init_internal_our_exception_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
  if (retval != MACH_MSG_SUCCESS)
    abort ();
  exit (1);
}
static inline void
nocrash_init (void)
{
  mach_port_t self = mach_task_self ();
  /* Allocate a port on which the thread shall listen for exceptions.  */
  if (mach_port_allocate (self, MACH_PORT_RIGHT_RECEIVE, &gnulib_nocrash_init_internal_our_exception_port)
      == KERN_SUCCESS) {
    /* See https://web.mit.edu/darwin/src/modules/xnu/osfmk/man/mach_port_insert_right.html.  */
    if (mach_port_insert_right (self, gnulib_nocrash_init_internal_our_exception_port, gnulib_nocrash_init_internal_our_exception_port,
                                MACH_MSG_TYPE_MAKE_SEND)
        == KERN_SUCCESS) {
      /* The exceptions we want to catch.  Only EXC_BAD_ACCESS is interesting
         for us.  */
      exception_mask_t mask = EXC_MASK_BAD_ACCESS;
      /* Create the thread listening on the exception port.  */
      pthread_attr_t attr;
      pthread_t thread;
      if (pthread_attr_init (&attr) == 0
          && pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED) == 0
          && pthread_create (&thread, &attr, gnulib_nocrash_init_internal_mach_exception_thread, NULL) == 0) {
        pthread_attr_destroy (&attr);
        /* Replace the exception port info for these exceptions with our own.
           Note that we replace the exception port for the entire task, not only
           for a particular thread.  This has the effect that when our exception
           port gets the message, the thread specific exception port has already
           been asked, and we don't need to bother about it.
           See https://web.mit.edu/darwin/src/modules/xnu/osfmk/man/task_set_exception_ports.html.  */
        task_set_exception_ports (self, mask, gnulib_nocrash_init_internal_our_exception_port,
                                  EXCEPTION_DEFAULT, MACHINE_THREAD_STATE);
      }
    }
  }
}
#elif defined _WIN32 && ! defined __CYGWIN__
static inline LONG WINAPI
gnulib_nocrash_init_internal_exception_filter (EXCEPTION_POINTERS *ExceptionInfo)
{
  switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
    case EXCEPTION_IN_PAGE_ERROR:
    case EXCEPTION_STACK_OVERFLOW:
    case EXCEPTION_GUARD_PAGE:
    case EXCEPTION_PRIV_INSTRUCTION:
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_DATATYPE_MISALIGNMENT:
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      exit (1);
    }
  return EXCEPTION_CONTINUE_SEARCH;
}
static inline void
nocrash_init (void)
{
  SetUnhandledExceptionFilter ((LPTOP_LEVEL_EXCEPTION_FILTER) gnulib_nocrash_init_internal_exception_filter);
}
#else
/* A POSIX signal handler.  */
static inline void
gnulib_nocrash_init_internal_exception_handler (int sig)
{
  _exit (1);
}
static inline void
nocrash_init (void)
{
#ifdef SIGSEGV
  signal (SIGSEGV, gnulib_nocrash_init_internal_exception_handler);
#endif
#ifdef SIGBUS
  signal (SIGBUS, gnulib_nocrash_init_internal_exception_handler);
#endif
}
#endif
