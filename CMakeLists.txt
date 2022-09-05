cmake_minimum_required(VERSION 3.4)

project(yalibct
  VERSION 0.1
  DESCRIPTION "Yet another libc testsuite - Tests whatever libc it is run under"
  LANGUAGES C
)

option(YALIBCT_ENABLE_OPTIONAL_TESTS_BY_DEFAULT "If this is set to OFF, then none of the optional tests will be run by default" ON)
option(YALIBCT_ENABLE_LC_NUMERIC_TESTS "Whether or not tests that depend on LC_NUMERIC support being present will be run" ${YALIBCT_ENABLE_OPTIONAL_TESTS_BY_DEFAULT})

include(CheckIncludeFile)
include(CheckSymbolExists)

set(YALIBCT_COMPILE_FLAGS "_GNU_SOURCE")

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

  check_symbol_exists(${symbol_name} ${include_filename} ${macro_name})
  if (${macro_name})
    list(APPEND YALIBCT_COMPILE_FLAGS ${macro_name})
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

yalibct_check_include_file(hedley.h YALIBCT_HAS_SYSTEM_HEDLEY_H)

yalibct_check_libc_symbol(vstrdupf string.h YALIBCT_LIBC_HAS_VSTRDUPF)
yalibct_check_libc_symbol(memchr_inv string.h YALIBCT_LIBC_HAS_MEMCHR_INV)
yalibct_check_libc_symbol(NAN math.h YALIBCT_LIBC_HAS_NAN)

yalibct_make_executable(printf-KOS-mk4-test-positional-printf tests/printf/KOS/mk4/test-positional-printf.c)
yalibct_make_executable(printf-linux-kernel-test_printf tests/printf/linux-kernel/test_printf.c)
yalibct_make_executable(printf-NetBSD-t_printf tests/printf/NetBSD/t_printf.c)
yalibct_make_executable(printf-FreeBSD-printfloat_test tests/printf/FreeBSD/printfloat_test.c)
yalibct_make_executable(printf-FreeBSD-atf-printf_test tests/printf/FreeBSD/atf-printf_test.c)
yalibct_make_executable(printf-FreeBSD-plain-printf_test tests/printf/FreeBSD/plain-printf_test.c)
