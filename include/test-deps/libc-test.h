// Derived from code with this license:
/*
libc-test is licensed under the following standard MIT license:

Copyright © 2005-2013 libc-test AUTHORS

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


Portions of this software is derived from software authored by
third parties:

math tests use numbers under BSD and GPL licenses see src/math/ucb/*
and src/math/crlibm/* for details
*/

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define T_LOC2(l) __FILE__ ":" #l
#define T_LOC1(l) T_LOC2(l)
#define t_error(...) t_printf(T_LOC1(__LINE__) ": " __VA_ARGS__)

volatile static int t_status = 0;

static inline int t_printf(const char *s, ...)
{
        va_list ap;
        char buf[512];
        int n;

        t_status = 1;
        va_start(ap, s);
        n = vsnprintf(buf, sizeof buf, s, ap);
        va_end(ap);
        if (n < 0)
                n = 0;
        else if (n >= sizeof buf) {
                n = sizeof buf;
                buf[n - 1] = '\n';
                buf[n - 2] = '.';
                buf[n - 3] = '.';
                buf[n - 4] = '.';
        }
        return write(1, buf, n);
}
