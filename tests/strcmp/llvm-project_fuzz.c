//===-- strcmp_fuzz.cpp ---------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// Fuzzing test for llvm-libc strcmp implementation.
///
//===----------------------------------------------------------------------===//
#include "test-deps/llvm-project.h"
#include "test-deps/lib-fuzzer.h"
#include <stdlib.h>
#include <string.h>

#include <stddef.h>
#include <stdint.h>

// The general structure is to take the value of the first byte, set size1 to
// that value, and add the null terminator. size2 will then contain the rest of
// the bytes in data.
// For example, with inputs (data={2, 6, 4, 8, 0}, size=5):
//         size1: data[0] = 2
//         data1: {2, 6} + '\0' = {2, 6, '\0'}
//         size2: size - size1 = 3
//         data2: {4, 8, '\0'}
extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // Verify the size is at least 1 and the data is null terminated.
  if (!size || data[size - 1] != '\0')
    return 0;

  const size_t size1 = (data[0] <= size ? data[0] : size);
  const size_t size2 = size - size1;

  // The first size will always be at least 1 since
  // we need to append the null terminator. The second size
  // needs to be checked since it must also contain the null
  // terminator.
  if (!size2)
    return 0;

  // Copy the data into new containers.
  // Add one to data1 for null terminator.
  uint8_t *data1 = malloc(size1 + 1);
  uint8_t *data2 = calloc(1, size2);
  if (!data1 || !data2)
    __builtin_trap();

  size_t i;
  for (i = 0; i < size1; ++i)
    data1[i] = data[i];
  data1[size1] = '\0'; // Add null terminator to data1.

  for (size_t j = 0; j < size2; ++j)
    data2[j] = data[i++];

  const char *s1 = (const char *)(data1);
  const char *s2 = (const char *)(data2);
  size_t k = 0;
  // Iterate until a null terminator is hit or the character comparison is
  // different.
  while (s1[k] && s2[k] && s1[k] == s2[k])
    ++k;

  const unsigned char ch1 = (unsigned char)(s1[k]);
  const unsigned char ch2 = (unsigned char)(s2[k]);
  // The expected result should be the difference between the first non-equal
  // characters of s1 and s2. If all characters are equal, the expected result
  // should be '\0' - '\0' = 0.
  // Yalibct correction: not actually required by the standard, result only has to have the correct relation to 0 (i.e. appropriate for qsort purposes)
  //if (__llvm_libc strcmp(s1, s2) != ch1 - ch2)
  //  __builtin_trap();
  int strcmp_result = strcmp(s1, s2);
  if ((strcmp_result < 0 && (ch1 >= ch2)) ||
      (strcmp_result == 0 && (ch1 != ch2)) ||
      (strcmp_result > 0 && (ch1 <= ch2)))
      __builtin_trap();

  // Verify reversed operands. This should be the negated value of the previous
  // result, except of course if the previous result was zero.
  // Yalibct correction: see above correction
  //if (__llvm_libc strcmp(s2, s1) != ch2 - ch1)
  //  __builtin_trap();
  strcmp_result = strcmp(s2, s1);
  if ((strcmp_result < 0 && (ch1 <= ch2)) ||
      (strcmp_result == 0 && (ch1 != ch2)) ||
      (strcmp_result > 0 && (ch1 >= ch2)))
      __builtin_trap();

  free(data1);
  free(data2);
  return 0;
}
