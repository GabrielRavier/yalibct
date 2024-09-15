/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2018 Nexenta Systems, Inc.
 */

/*
 * Note that this file is easiest edited with a UTF-8 capable editor,
 * as there are embedded UTF-8 symbols in some of the strings.
 */

#include <err.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

struct {
    const char *locale;
    const char *convspec;
    const float fp;
    const char *expected;
    const char *expected2;
} fpconv[] = {
#ifndef YALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS
    "C", "%a", 3.2, "0x1.99999a0000000p+1", "0x1.99999ap+1",
#endif
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
    "C", "%e", 3.2, "3.200000e+00", NULL,
#endif
    "C", "%f", 3.2, "3.200000", NULL,
    "C", "%g", 3.2, "3.2", NULL,
#ifndef YALIBCT_DISABLE_PRINTF_A_CONVERSION_SPECIFIER_TESTS
    "en_US.UTF-8", "%a", 3.2, "0x1.99999a0000000p+1", "0x1.99999ap+1",
#endif
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
    "en_US.UTF-8", "%e", 3.2, "3.200000e+00", NULL,
#endif
    "en_US.UTF-8", "%f", 3.2, "3.200000", NULL,
    "en_US.UTF-8", "%g", 3.2, "3.2", NULL,
#ifndef YALIBCT_DISABLE_RARE_LOCALE_TESTS
    "ru_RU.UTF-8", "%a", 3.2, "0x1,99999a0000000p+1", "0x1,99999ap+1",
    "ru_RU.UTF-8", "%e", 3.2, "3,200000e+00", NULL,
    "ru_RU.UTF-8", "%f", 3.2, "3,200000", NULL,
    "ru_RU.UTF-8", "%g", 3.2, "3,2", NULL,
#endif
    NULL, NULL, 0, NULL, NULL,
};

int
main(void)
{
    char buf[100];
    int i;

    for (i = 0; fpconv[i].locale != NULL; i++) {
        if (setlocale(LC_NUMERIC, fpconv[i].locale) == NULL)
            continue;

        (void) sprintf(buf, fpconv[i].convspec, fpconv[i].fp);
        if (strcmp(fpconv[i].expected, buf) != 0) {
            if (fpconv[i].expected2 == NULL)
                errx(1, "locale=%s, convspec=%s, expected=%s, got=%s",
                     fpconv[i].locale, fpconv[i].convspec,
                     fpconv[i].expected, buf);
            else if (strcmp(fpconv[i].expected2, buf) != 0)
                errx(1, "locale=%s, convspec=%s, expected=%s, expected2=%s, got=%s",
                     fpconv[i].locale, fpconv[i].convspec,
                     fpconv[i].expected, fpconv[i].expected2, buf);
        }
    }

    return (0);
}
