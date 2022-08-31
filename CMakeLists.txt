cmake_minimum_required(VERSION 3.4)

project(gravier-tests
  VERSION 0.1
  DESCRIPTION "Tests your libc"
  LANGUAGES C
)

include(CheckIncludeFile)
include(CheckSymbolExists)

set(GRAVIER_TESTS_COMPILE_FLAGS "_GNU_SOURCE")

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

endmacro()

gravier_tests_check_include_file(hedley.h GRAVIER_TESTS_HAS_SYSTEM_HEDLEY_H)

gravier_tests_check_libc_function(vstrdupf string.h GRAVIER_TESTS_LIBC_HAS_VSTRDUPF)

gravier_tests_make_executable(printf-test-positional-printf printf/test-positional-printf.c)
