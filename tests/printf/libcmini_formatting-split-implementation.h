#pragma once
#include "test-lib/chdir-to-tmpdir.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>


#define ARRAYSIZE(arr)  (sizeof(arr) / sizeof(arr[0]))


static void integer_test(FILE* fp, char type);
static void long_integer_test(FILE* fp, char type);
static void make_format_string(char* dest, char type, int is_long, int width, int prec, int flush_left, const char* modifiers);


static const char* modifiers[] = {
    NULL,
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
    "0",
#endif
    "+",
    " ",
#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
    "#",
#endif
#ifndef YALIBCT_DISABLE_PRINTF_0_FLAG_TESTS
    "+0",
    " 0",
#endif
#ifndef YALIBCT_DISABLE_PRINTF_HASH_FLAG_TESTS
    "#0",
    "#+",
    "# ",
    "#0+",
    "#0 "
#endif
};


int main()
{
    const char* type = "diuox"
#ifndef YALIBCT_DISABLE_PRINTF_UPPERCASE_X_CONVERSION_SPECIFIER_TESTS
        "X"
#endif
        ;
    const char* t;
    FILE* fp;

    yalibct_chdir_to_tmpdir();
    fp = fopen("test.txt", "w");

    for (t = type; *t != '\0'; ++t) {
        integer_test(fp, *t);
    }

    for (t = type; *t != '\0'; ++t) {
        long_integer_test(fp, *t);
    }

    assert(fclose(fp) == 0);

    fp = fopen("test.txt", "r");
#ifdef YALIBCT_LIBC_HAS_FGETC
    while (true) {
        int ch = fgetc(fp);
        if (ch == EOF)
            break;
        putchar(ch);
    }
#endif
    assert(fclose(fp) == 0);
    assert(remove("test.txt") == 0);
    yalibct_chdir_to_tmpdir_remove_tmpdir();

    return 0;
}


void integer_test(FILE* fp, char type)
{
    static const int val[] = VALUES;
    register int v, m, w, p, l, i;

    for (v = 0; v < ARRAYSIZE(val); ++v) {
        int value = val[v];

        for (m = 0; m < ARRAYSIZE(modifiers); ++m) {
            const char* mod = modifiers[m];

            for (w = 0; w < 9; ++w) {
#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
                for (p = 0; p < 9; ++p) {
#else
                p = 0;
                {
#endif
                    for (l = 0; l <= 1; ++l) {
                        static char format[128];
                        char result[128];

                        make_format_string(format, type, 0, w, p, l, mod);

                        for (i = 0; i < 2; ++i) {
                            sprintf(result, format, value);
                            fprintf(fp, "\"%s\" %d: \"%s\"\r\n", format, value, result);

                            if (value == 0) {
                                break;
                            }

                            value = -value;
                        }
                    }
                }
            }
        }
    }
}


void long_integer_test(FILE* fp, char type)
{
    static const long val[] = VALUES;
    register int v, m, w, p, l, i;

    for (v = 0; v < ARRAYSIZE(val); ++v) {
        long value = val[v];

        for (m = 0; m < ARRAYSIZE(modifiers); ++m) {
            const char* mod = modifiers[m];

            for (w = 0; w < 9; ++w) {
#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_TESTS
                for (p = 0; p < 9; ++p) {
#else
                p = 0;
                {
#endif
#ifndef YALIBCT_DISABLE_PRINTF_MINUS_FLAG_TESTS
                    for (l = 0; l <= 1; ++l) {
#else
                    l = 0;
                    {
#endif
                        static char format[128];
                        char result[128];

                        make_format_string(format, type, 1, w, p, l, mod);

                        for (i = 0; i < 2; ++i) {
                            sprintf(result, format, value);
                            fprintf(fp, "\"%s\", %ld, \"%s\"\r\n", format, value, result);

                            if (value == 0) {
                                break;
                            }

                            value = -value;
                        }
                    }
                }
            }
        }
    }
}


void make_format_string(char* dest, char type, int is_long, int width, int prec, int flush_left, const char* modifiers)
{
    *dest++ = '%';

    if (flush_left) {
        *dest++ = '-';
    }

    if (modifiers != NULL) {
        strcpy(dest, modifiers);

        while (*dest != '\0') {
            ++dest;
        }
    }

    if (width > 0) {
        dest += sprintf(dest, "%d", width);
    }

    if (prec > 0) {
        dest += sprintf(dest, ".%d", prec);
    }

    if (is_long) {
        *dest++ = 'l';
    }

    *dest++ = type;
    *dest++ = '\0';
}
