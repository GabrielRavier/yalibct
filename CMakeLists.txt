cmake_minimum_required(VERSION 3.4)

project(gravier-tests
  VERSION 0.1
  DESCRIPTION "Tests your libc"
  LANGUAGES C
)

option(GRAVIER_TESTS_ENABLE_LC_NUMERIC_TESTS "Whether or not tests that depend on LC_NUMERIC support being present will be run" ON)

include(CheckIncludeFile)
include(CheckSymbolExists)

set(GRAVIER_TESTS_COMPILE_FLAGS "_GNU_SOURCE")

macro(gravier_tests_append_option_to_compiler_flags option_name)

  if (${option_name})
    list(APPEND GRAVIER_TESTS_COMPILE_FLAGS ${option_name})
  endif()

endmacro()

macro(gravier_tests_check_include_file include_filename macro_name)

  CHECK_INCLUDE_FILE(${include_filename} ${macro_name})
  if (${macro_name})
    list(APPEND GRAVIER_TESTS_COMPILE_FLAGS ${macro_name})
  endif()

endmacro()

macro(gravier_tests_check_libc_function function_name include_filename macro_name)

  check_symbol_exists(${function_name} ${include_filename} ${macro_name})
  if (${macro_name})
    list(APPEND GRAVIER_TESTS_COMPILE_FLAGS ${macro_name})
  endif()

endmacro()

macro(gravier_tests_make_executable target_name target_source_file)

  add_executable(${target_name} ${target_source_file})
  target_include_directories(${target_name}
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
  )
  target_compile_definitions(${target_name} PRIVATE ${GRAVIER_TESTS_COMPILE_FLAGS})
  set_target_properties(${target_name}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/test-binaries"
  )
  target_link_libraries(${target_name} m)

endmacro()

gravier_tests_append_option_to_compiler_flags(GRAVIER_TESTS_ENABLE_LC_NUMERIC_TESTS)

gravier_tests_check_include_file(hedley.h GRAVIER_TESTS_HAS_SYSTEM_HEDLEY_H)

gravier_tests_check_libc_function(vstrdupf string.h GRAVIER_TESTS_LIBC_HAS_VSTRDUPF)
gravier_tests_check_libc_function(memchr_inv string.h GRAVIER_TESTS_LIBC_HAS_MEMCHR_INV)

gravier_tests_make_executable(printf-KOS-mk4-test-positional-printf tests/printf/KOS/mk4/test-positional-printf.c)
gravier_tests_make_executable(printf-linux-kernel-test_printf tests/printf/linux-kernel/test_printf.c)
gravier_tests_make_executable(printf-NetBSD-t_printf tests/printf/NetBSD/t_printf.c)
gravier_tests_make_executable(printf-FreeBSD-printfloat_test tests/printf/FreeBSD/printfloat_test.c)
