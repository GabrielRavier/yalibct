cmake_minimum_required(VERSION 3.4)

project(gravier-tests
  VERSION 0.1
  DESCRIPTION "Tests your libc"
  LANGUAGES C
)

set(GRAVIER_TESTS_COMPILE_FLAGS "_GNU_SOURCE")

macro(gravier_tests_make_executable target_name target_source_file)

  add_executable(${target_name} ${target_source_file})
  target_include_directories(${target_name}
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
  )
  target_compile_definitions(${target_name} PRIVATE ${GRAVIER_TESTS_COMPILE_FLAGS})

endmacro()


gravier_tests_make_executable(printf-test-positional-printf printf/test-positional-printf.c)
