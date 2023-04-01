// Derived from code with this license:
//
// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// StartsWith()
//
// Returns whether a given string `text` begins with `prefix`.
static inline bool absl_StartsWith(const char *text,
                       const char *prefix) {
  return prefix == NULL || *prefix == '\0' ||
      (strlen(text) >= strlen(prefix) &&
       memcmp(text, prefix, strlen(prefix)) == 0);
}

// ----------------------------------------------------------------------
// StrCat()
//    This merges the given strings or integers, with no delimiter. This
//    is designed to be the fastest possible way to construct a string out
//    of a mix of raw C strings, string_views, strings, and integer values.
// ----------------------------------------------------------------------

// Append is merely a version of memcpy that returns the address of the byte
// after the area just overwritten.
static char* absl_StrCat_internal_Append(char* out, const char *x) {
  // memcpy is allowed to overwrite arbitrary memory, so doing this after the
  // call would force an extra fetch of x.size().
    char* after = out + strlen(x);
    if (strlen(x) != 0) {
        memcpy(out, x, strlen(x));
    }
    return after;
}

static char *absl_StrCat(const char *a, const char *b) {
    char *result = malloc(strlen(a) + strlen(b) + 1);
    assert(result != NULL);
  char* const begin = &result[0];
  char* out = begin;
  out = absl_StrCat_internal_Append(out, a);
  out = absl_StrCat_internal_Append(out, b);
  assert(out == begin + strlen(a) + strlen(b));
  *out = '\0';
  return result;
}

// It's possible to call StrAppend with an absl::string_view that is itself a
// fragment of the string we're appending to.  However the results of this are
// random. Therefore, check for this in debug mode.  Use unsigned math so we
// only have to do one comparison. Note, there's an exception case: appending an
// empty string is always allowed.
#define absl_StrAppend_internal_ASSERT_NO_OVERLAP(dest, src) \
    assert((strlen(src) == 0) ||                                        \
           ((uintptr_t)(src - dest) > (uintptr_t)(strlen(dest))))

static void absl_StrAppend(char** dest, const char *a) {
    char *new_str = absl_StrCat(*dest, a);
    free(*dest);
    *dest = new_str;
}
