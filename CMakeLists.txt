cmake_minimum_required(VERSION 3.4)

project(yalibct
  VERSION 0.1
  DESCRIPTION "Yet another libc testsuite - Tests whatever libc it is run under"
  LANGUAGES C
)

option(YALIBCT_ENABLE_OPTIONAL_TESTS_BY_DEFAULT "If this is set to OFF, then none of the optional tests will be run by default, i.e. without the corresponding options being explicitly set" ON)
option(YALIBCT_ENABLE_LC_NUMERIC_TESTS "Whether or not tests that depend on LC_NUMERIC support being present will be run" ${YALIBCT_ENABLE_OPTIONAL_TESTS_BY_DEFAULT})
option(YALIBCT_ENABLE_PRINTF_ROUNDING_DIRECTION_TESTS "Whether or not tests that depend on the printf family of functions rounding floating point numbers will be run. I am unsure of whether the C standard requires this, but it seems like a very useful property for the functions to have regardless" ${YALIBCT_ENABLE_OPTIONAL_TESTS_BY_DEFAULT})
option(YALIBCT_ENABLE_RARE_LOCALE_TESTS "Whether or not tests that depend on potentially rare locales that the implementation might lie about supporting (as effectively allowed by the standard saying that non-standard locale names have unspecified behavior) will be run" ${YALIBCT_ENABLE_OPTIONAL_TESTS_BY_DEFAULT})
option(YALIBCT_ENABLE_C2X_TESTS_BY_DEFAULT "If this is set to OFF, then none of the optional tests that depend on the upcoming C2x standard will be run by default" ${YALIBCT_ENABLE_OPTIONAL_TESTS_BY_DEFAULT})
option(YALIBCT_ENABLE_PRINTF_B_CONVERSION_SPECIFIER_TESTS "Whether or not tests that depend on the printf family of functions supporting %b/%B will be run by default" ${YALIBCT_ENABLE_C2X_TESTS_BY_DEFAULT})

include(CheckIncludeFile)
include(CheckSymbolExists)

set(YALIBCT_COMPILE_FLAGS "-D_GNU_SOURCE")

macro(yalibct_append_option_to_compiler_flags option_name)

  if (${option_name})
    list(APPEND YALIBCT_COMPILE_FLAGS ${option_name})
  endif()

endmacro()

macro(yalibct_check_include_file include_filename macro_name)

  CHECK_INCLUDE_FILE(${include_filename} ${macro_name})
  if (${macro_name})
    list(APPEND YALIBCT_COMPILE_FLAGS ${macro_name})
  endif()

endmacro()

macro(yalibct_check_libc_symbol symbol_name include_filename macro_name)

  list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
  check_symbol_exists(${symbol_name} ${include_filename} ${macro_name})
  list(POP_BACK CMAKE_REQUIRED_DEFINITIONS)
  if (${macro_name})
    list(APPEND YALIBCT_COMPILE_FLAGS ${macro_name})
  else()
    list(APPEND YALIBCT_COMPILE_FLAGS "${symbol_name}=THE_LIBC_THIS_WAS_COMPILED_UNDER_DOESNT_HAVE_${symbol_name}")
  endif()

endmacro()

macro(yalibct_make_executable target_name target_source_file)

  add_executable(${target_name} ${target_source_file})
  target_include_directories(${target_name}
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
  )
  target_compile_definitions(${target_name} PRIVATE ${YALIBCT_COMPILE_FLAGS})
  set_target_properties(${target_name}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/test-binaries"
  )
  target_link_libraries(${target_name} m)

endmacro()

yalibct_append_option_to_compiler_flags(YALIBCT_ENABLE_LC_NUMERIC_TESTS)
yalibct_append_option_to_compiler_flags(YALIBCT_ENABLE_PRINTF_ROUNDING_DIRECTION_TESTS)
yalibct_append_option_to_compiler_flags(YALIBCT_ENABLE_RARE_LOCALE_TESTS)
yalibct_append_option_to_compiler_flags(YALIBCT_ENABLE_PRINTF_B_CONVERSION_SPECIFIER_TESTS)

yalibct_check_include_file(hedley.h YALIBCT_HAS_SYSTEM_HEDLEY_H)
yalibct_check_include_file(obstack.h YALIBCT_LIBC_HAS_OBSTACK_H)

yalibct_check_libc_symbol(vstrdupf string.h YALIBCT_LIBC_HAS_VSTRDUPF)
yalibct_check_libc_symbol(memchr_inv string.h YALIBCT_LIBC_HAS_MEMCHR_INV)
yalibct_check_libc_symbol(NAN math.h YALIBCT_LIBC_HAS_NAN)
yalibct_check_libc_symbol(fcvt stdlib.h YALIBCT_LIBC_HAS_FCVT)
yalibct_check_libc_symbol(ecvt stdlib.h YALIBCT_LIBC_HAS_ECVT)
yalibct_check_libc_symbol(xmalloc xalloc.h YALIBCT_LIBC_HAS_XMALLOC)
yalibct_check_libc_symbol(xalloc_die xalloc.h YALIBCT_LIBC_HAS_XALLOC_DIE)
yalibct_check_libc_symbol(error error.h YALIBCT_LIBC_HAS_ERROR)
yalibct_check_libc_symbol(getprogname stdlib.h YALIBCT_LIBC_HAS_GETPROGNAME)
yalibct_check_libc_symbol(program_invocation_short_name errno.h YALIBCT_LIBC_HAS_PROGRAM_INVOCATION_SHORT_NAME)
yalibct_check_libc_symbol(program_invocation_name errno.h YALIBCT_LIBC_HAS_PROGRAM_INVOCATION_NAME)
yalibct_check_libc_symbol(getexecname stdlib.h YALIBCT_LIBC_HAS_GETEXECNAME)
yalibct_check_libc_symbol(__argv stdlib.h YALIBCT_LIBC_HAS___ARGV)
yalibct_check_libc_symbol(__progname stdlib.h YALIBCT_LIBC_HAS___PROGNAME)
yalibct_check_libc_symbol(mtrace mcheck.h YALIBCT_LIBC_HAS_MTRACE)
yalibct_check_libc_symbol(mcheck_pedantic mcheck.h YALIBCT_LIBC_HAS_MCHECK_PEDANTIC)
yalibct_check_libc_symbol(mcheck_check_all mcheck.h YALIBCT_LIBC_HAS_MCHECK_CHECK_ALL)

yalibct_make_executable(printf-KOS-mk4-test-positional-printf tests/printf/KOS-mk4-test-positional-printf.c)
yalibct_make_executable(printf-linux-kernel-test_printf tests/printf/linux-kernel-test_printf.c)
yalibct_make_executable(printf-NetBSD-t_printf tests/printf/NetBSD-t_printf.c)
yalibct_make_executable(printf-FreeBSD-printfloat_test tests/printf/FreeBSD-printfloat_test.c)
yalibct_make_executable(printf-FreeBSD-atf-printf_test tests/printf/FreeBSD-atf-printf_test.c)
yalibct_make_executable(printf-FreeBSD-plain-printf_test tests/printf/FreeBSD-plain-printf_test.c)
yalibct_make_executable(printf-FreeBSD-tap-printf_test tests/printf/FreeBSD-tap-printf_test.c)
yalibct_make_executable(printf-fuchsia-printf_tests tests/printf/fuchsia-printf_tests.c)
yalibct_make_executable(printf-illumos-gate-printf-6961 tests/printf/illumos-gate-printf-6961.c)
yalibct_make_executable(printf-illumos-gate-printf-9511 tests/printf/illumos-gate-printf-9511.c)
yalibct_make_executable(printf-reactos-printf tests/printf/reactos-printf.c)
yalibct_make_executable(printf-littlekernel-printf_tests tests/printf/littlekernel-printf_tests.c)
yalibct_make_executable(printf-toaruos-test-printf tests/printf/toaruos-test-printf.c)
yalibct_make_executable(printf-newsys-test-printf tests/printf/newsys-test.printf.c)
yalibct_make_executable(printf-osv-tst-printf tests/printf/osv-tst-printf.c)
yalibct_make_executable(printf-OpenBSD-fp tests/printf/OpenBSD-fp.c)
yalibct_make_executable(printf-OpenBSD-int tests/printf/OpenBSD-int.c)
yalibct_make_executable(printf-OpenBSD-string tests/printf/OpenBSD-string.c)
yalibct_make_executable(printf-llvm-project-printf_test tests/printf/llvm-project-printf_test.c)
yalibct_make_executable(printf-gcc-printf tests/printf/gcc-printf.c)
yalibct_make_executable(printf-gcc-printf-1 tests/printf/gcc-printf-1.c)
yalibct_make_executable(printf-gcc-printf-2 tests/printf/gcc-printf-2.c)
yalibct_make_executable(printf-llvm-test-suite-2002-04-17-PrintfChar tests/printf/llvm-test-suite-2002-04-17-PrintfChar.c)
yalibct_make_executable(printf-tcc-02_printf tests/printf/tcc-02_printf.c)
yalibct_make_executable(printf-wine-msvcrt-printf tests/printf/wine-msvcrt-printf.c)
yalibct_make_executable(printf-wine-ucrtbase-printf tests/printf/wine-ucrtbase-printf.c)
yalibct_make_executable(printf-glibc-tst-printf-binary tests/printf/glibc-tst-printf-binary.c)
yalibct_make_executable(printf-glibc-tst-obprintf tests/printf/glibc-tst-obprintf.c)
yalibct_make_executable(printf-glibc-tst-printf-bz18872-sh-output tests/printf/glibc-tst-printf-bz18872-sh-output.c)
