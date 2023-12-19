/* Copyright © 2013 Bart Massey */
/* This program is licensed under the GPL version 2 or later.
   Please see the file COPYING.GPL2 in this distribution for
   license terms. */

#include "test-lib/portable-symbols/printf.h"
#include "test-lib/portable-symbols/NAN.h"
#include "test-lib/portable-symbols/NL_ARGMAX.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define TEST_ASPRINTF
#define printf_float(x) x

static char buf[1024];

static void failmsg(int serial, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("test %d failed: ", serial);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

#define test(serial, expect, ...) test2((serial), (expect), NULL, __VA_ARGS__)
static int test2(int serial, char *expect, char *expect2, char *fmt, ...) {
    va_list ap;
    char *abuf = NULL;
    va_start(ap, fmt);
    int n;
#ifdef TEST_ASPRINTF
    int an;
    va_list aap;
    va_copy(aap, ap);
#endif
#ifndef NO_FLOATING_POINT
    double dv;
    char *star;
#endif
    switch (fmt[strlen(fmt)-1]) {
    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
    case 'a':
    case 'A':
#ifdef NO_FLOATING_POINT
	    return 0;
#else
	    star = strchr(fmt, '*');
	    if (star) {
		    if (strchr(star+1, '*')) {
			    int iv1 = va_arg(ap, int);
			    int iv2 = va_arg(ap, int);
			    dv = va_arg(ap, double);
			    n = snprintf(buf, 1024, fmt, iv1, iv2, printf_float(dv));
#ifdef TEST_ASPRINTF
			    an = asprintf(&abuf, fmt, iv1, iv2, printf_float(dv));
#endif
		    } else  {
			    int iv = va_arg(ap, int);
			    dv = va_arg(ap, double);
			    n = snprintf(buf, 1024, fmt, iv, printf_float(dv));
#ifdef TEST_ASPRINTF
			    an = asprintf(&abuf, fmt, iv, printf_float(dv));
#endif
		    }
	    } else {
		    dv = va_arg(ap, double);
		    n = snprintf(buf, 1024, fmt, printf_float(dv));
#ifdef TEST_ASPRINTF
		    an = asprintf(&abuf, fmt, printf_float(dv));
#endif
	    }
	    break;
#endif
    default:
	    n = vsnprintf(buf, 1024, fmt, ap);
#ifdef TEST_ASPRINTF
	    an = vasprintf(&abuf, fmt, aap);
#endif
	    break;
    }
    va_end(ap);
#ifdef TEST_ASPRINTF
    va_end(aap);
#endif
//    printf("serial %d expect \"%s\" got \"%s\"\n", serial, expect, buf);
    if (n >= 1024) {
        failmsg(serial, "buffer overflow");
	free(abuf);
        return 1;
    }
    if (n != (int) strlen(expect) && (expect2 == NULL || n != (int) strlen(expect2))) {
        failmsg(serial, "on test case \"%s\", expected \"%s\" (%d), got \"%s\" (%d)",
		fmt, expect, strlen(expect), buf, n);
	free(abuf);
        return 1;
    }
    if (strcmp(buf, expect) && (expect2 == NULL || strcmp(buf, expect2))) {
        failmsg(serial, "expected \"%s\", got \"%s\"", expect, buf);
	free(abuf);
        return 1;
    }
#ifdef TEST_ASPRINTF
    if (an != n) {
	failmsg(serial, "asprintf return %d sprintf return %d\n", an, n);
	free(abuf);
	return 1;
    }
    if (strcmp(abuf, buf)) {
	failmsg(serial, "sprintf return %s asprintf return %s\n", buf, abuf);
	free(abuf);
	return 1;
    }
    free(abuf);
#endif
    return 0;
}

int main(void) {
    int result = 0;
#include "picolibc-testcases.h"
    return result;
}
