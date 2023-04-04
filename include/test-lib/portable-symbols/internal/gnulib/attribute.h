#pragma once

#include "test-lib/compiler-features.h"
#include "test-lib/hedley.h"

#define _GL_ATTRIBUTE_PURE HEDLEY_PURE
#define _GL_ATTRIBUTE_MAYBE_UNUSED YALIBCT_ATTRIBUTE_MAYBE_UNUSED
#define _GL_UNUSED _GL_ATTRIBUTE_MAYBE_UNUSED
#define _GL_INLINE _GL_UNUSED static
#define _GL_ATTRIBUTE_CONST HEDLEY_CONST
