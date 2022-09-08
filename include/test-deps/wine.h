#pragma once

#include "test-lib/assert-with-message.h"
#include <stddef.h>
#include <stdbool.h>

#define __cdecl
#define __ms_va_list va_list
#define WINAPIV
#define ok YALIBCT_ASSERT_WITH_MESSAGE
#define broken(expr) (false)
#define win_skip(string)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define wine_dbgstr_w We aint got some magic function to convert from wide to narrow here, pls probably replace this with appropriate usage of %ls
#define __ms_va_start va_start
#define __ms_va_end va_end
#define START_TEST(name) int main()
#define todo_wine_if(expr)

typedef wchar_t WCHAR;
typedef unsigned long long ULONGLONG;
typedef long long LONGLONG;
