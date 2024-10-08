/* $OpenBSD: string.c,v 1.2 2020/07/14 16:40:04 kettenis Exp $ */
/*
 * Copyright (c) 2020 Ingo Schwarze <schwarze@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Test the %c, %lc, %s, and %ls conversion specifiers with all their
 * modifiers, in particular with the minus flag, width, and maxbytes.
 * Also verify that other flags do nothing useful.
 */
#include "test-lib/portable-symbols/getopt.h"
#include "test-lib/should-always-be-included.h"
#include <err.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <assert.h>

void	 tc(const char *, int, const char *);
void	 tlc(const char *, wint_t, const char *);
void	 tlc_expect_fail(const char *, wint_t);
void	 ts(const char *, const char *, const char *);
void	 tls(const char *, const wchar_t *, const char *);
void	 tls_expect_fail(const char *, const wchar_t *);

static int	 badret, badlen, badout;	/* Error counters. */
static int	 verbose;			/* For debugging. */


/*
 * Print the single-byte character c with the format fmt,
 * check that the result matches what we want,
 * and report and count the error on failure.
 */
void
tc(const char *fmt, int c, const char *want)
{
	char		 buf[32];
	size_t		 len;
	int		 irc, happy;

	happy = 1;
	irc = snprintf(buf, sizeof(buf), fmt, c);
	len = strlen(want);
	if (irc < 0) {
		warn("printf(\"%s\", %d) returned %d", fmt, c, irc);
		badret++;
		return;
	}
	if ((unsigned long long)irc != len) {
		warnx("printf(\"%s\", %d) returned %d (expected %zu)",
		    fmt, c, irc, len);
		badlen++;
		happy = 0;
	}
	if (strcmp(buf, want) != 0) {
		warnx("printf(\"%s\", %d) wrote \"%s\" (expected \"%s\")",
		    fmt, c, buf, want);
		badout++;
		happy = 0;
	}
	if (verbose && happy)
		warnx("printf(\"%s\", %d) wrote \"%s\" length %d (OK)",
		    fmt, c, buf, irc);
}

/*
 * Print the wide character wc with the format fmt,
 * check that the result matches what we want,
 * and report and count the error on failure.
 */
void
tlc(const char *fmt, wint_t wc, const char *want)
{
	char		 buf[32];
	const char	*charset;
	size_t		 len;
	int		 irc, happy;

	happy = 1;
	charset = MB_CUR_MAX > 1 ? "UTF-8" : "ASCII";
	irc = snprintf(buf, sizeof(buf), fmt, wc);
	len = strlen(want);
	if (irc < 0) {
		warn("%s printf(\"%s\", U+%.4X) returned %d",
		    charset, fmt, (unsigned int)wc, irc);
		badret++;
		return;
	}
	if ((unsigned long long)irc != len) {
		warnx("%s printf(\"%s\", U+%.4X) returned %d (expected %zu)",
		    charset, fmt, (unsigned int)wc, irc, len);
		badlen++;
		happy = 0;
	}
	if (strcmp(buf, want) != 0) {
		warnx("%s printf(\"%s\", U+%.4X) "
		    "wrote \"%s\" (expected \"%s\")",
		    charset, fmt, (unsigned int)wc, buf, want);
		badout++;
		happy = 0;
	}
	if (verbose && happy)
		warnx("%s printf(\"%s\", U+%.4X) wrote \"%s\" length %d (OK)",
		    charset, fmt, (unsigned int)wc, buf, irc);
}

/*
 * Try to print the invalid wide character wc with the format fmt,
 * check that it fails as it should, and report and count if it doesn't.
 */
void
tlc_expect_fail(const char *fmt, wint_t wc)
{
	char		 buf[32];
	const char	*charset;
	int		 irc;

	errno = 0;
	charset = MB_CUR_MAX > 1 ? "UTF-8" : "ASCII";
	irc = snprintf(buf, sizeof(buf), fmt, wc);
	if (irc != -1) {
		warn("%s printf(\"%s\", U+%.4X) returned %d",
		    charset, fmt, (unsigned int)wc, irc);
		badret++;
	} else if (errno != EILSEQ) {
		warnx("%s printf(\"%s\", U+%.4X) errno %d (expected %d)",
		    charset, fmt, (unsigned int)wc, errno, EILSEQ);
		badret++;
	} else if (verbose)
		warnx("%s printf(\"%s\", U+%.4X) returned %d errno %d (OK)",
		    charset, fmt, (unsigned int)wc, irc, errno);
}

/*
 * Print the string s with the format fmt,
 * check that the result matches what we want,
 * and report and count the error on failure.
 */
void
ts(const char *fmt, const char *s, const char *want)
{
	char		 buf[32];
	size_t		 len;
	int		 irc, happy;

	happy = 1;
	irc = snprintf(buf, sizeof(buf), fmt, s);
	len = strlen(want);
	if (irc < 0) {
		warn("printf(\"%s\", \"%s\") returned %d", fmt, s, irc);
		badret++;
		return;
	}
	if ((unsigned long long)irc != len) {
		warnx("printf(\"%s\", \"%s\") returned %d (expected %zu)",
		    fmt, s, irc, len);
		badlen++;
		happy = 0;
	}
	if (strcmp(buf, want) != 0) {
		warnx("printf(\"%s\", \"%s\") wrote \"%s\" (expected \"%s\")",
		    fmt, s, buf, want);
		badout++;
		happy = 0;
	}
	if (verbose && happy)
		warnx("printf(\"%s\", \"%s\") wrote \"%s\" length %d (OK)",
		    fmt, s, buf, irc);
}

/*
 * Print the wide character string ws with the format fmt,
 * check that the result matches what we want,
 * and report and count the error on failure.
 */
void
tls(const char *fmt, const wchar_t *ws, const char *want)
{
	char		 buf[32];
	const char	*charset;
	size_t		 len;
	int		 irc, happy;

	happy = 1;
	charset = MB_CUR_MAX > 1 ? "UTF-8" : "ASCII";
	irc = snprintf(buf, sizeof(buf), fmt, ws);
	len = strlen(want);
	if (irc < 0) {
		warn("%s printf(\"%s\", \"%ls\") returned %d",
		    charset, fmt, ws, irc);
		badret++;
		return;
	}
	if ((unsigned long long)irc != len) {
		warnx("%s printf(\"%s\", \"%ls\") returned %d (expected %zu)",
		    charset, fmt, ws, irc, len);
		badlen++;
		happy = 0;
	}
	if (strcmp(buf, want) != 0) {
		warnx("%s printf(\"%s\", \"%ls\") "
		    "wrote \"%s\" (expected \"%s\")",
		    charset, fmt, ws, buf, want);
		badout++;
		happy = 0;
	}
	if (verbose && happy)
		warnx("%s printf(\"%s\", \"%ls\") wrote \"%s\" length %d (OK)",
		    charset, fmt, ws, buf, irc);
}

/*
 * Try to print the invalid wide character string ws with the format fmt,
 * check that it fails as it should, and report and count if it doesn't.
 */
void
tls_expect_fail(const char *fmt, const wchar_t *ws)
{
	char		 buf[32];
	const char	*charset;
	int		 irc;

	errno = 0;
	charset = MB_CUR_MAX > 1 ? "UTF-8" : "ASCII";
	irc = snprintf(buf, sizeof(buf), fmt, ws);
	if (irc != -1) {
		warn("%s printf(\"%s\", U+%.4X, ...) returned %d",
		    charset, fmt, (unsigned int)*ws, irc);
		badret++;
	} else if (errno != EILSEQ) {
		warnx("%s printf(\"%s\", U+%.4X, ...) errno %d (expected %d)",
		    charset, fmt, (unsigned int)*ws, errno, EILSEQ);
		badret++;
	} else if (verbose)
		warnx("%s printf(\"%s\", U+%.4X, ...) "
		    "returned %d errno %d (OK)",
		    charset, fmt, (unsigned int)*ws, irc, errno);
}

int
main(int argc, char *argv[])
{
	const wchar_t	 ws[] = { 0x0421, 0x043e, 0x0444, 0x044f, 0 };
	const wchar_t	 wsbad[] = { 0x0391, 0xdeef, 0x3c9, 0 };
	int		 badarg, picky;
	int		 ch;

	badarg = 0;
        picky = 1;
	while ((ch = getopt(argc, argv, "pv")) != -1) {
		switch (ch) {
		case 'p':
			picky = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			badarg = 1;
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc > 0) {
		warnx("unexpected argument \"%s\"", *argv);
		badarg = 1;
	}
	if (badarg) {
            assert(fputs("usage: string [-pv]\n", stderr) >= 0);
            return 1;
	}

	/*
	 * Valid use cases of %c and %s.
	 */

#ifndef YALIBCT_DISABLE_PRINTF_C_CONVERSION_SPECIFIER_TESTS
	tc("<%c>", '=', "<=>");
	tc("<%c>", '\t', "<\t>");
        tc("<%c>", 0xfe, "<\xfe>");
	tc("<%-c>", '=', "<=>");
	tc("<%2c>", '=', "< =>");
	tc("<%-2c>", '=', "<= >");
#endif

	ts("<%s>", "text", "<text>");
	ts("<%-s>", "text", "<text>");
	ts("<%6s>", "text", "<  text>");
	ts("<%-6s>", "text", "<text  >");
	ts("<%.2s>", "text", "<te>");
	ts("<%4.2s>", "text", "<  te>");
	ts("<%-4.2s>", "text", "<te  >");

	/*
	 * Undefined behaviour of %c and %s.
	 * Do not test by default to avoid noise.
	 * But provide the tests anyway to help track down
	 * unintended changes of behaviour when needed.
	 */

	if (picky) {
#ifndef YALIBCT_DISABLE_PRINTF_C_CONVERSION_SPECIFIER_TESTS
		tc("<%#c>", '=', "<=>");
		tc("<% -3c>", '=', "<=  >");
#ifndef YALIBCT_DISABLE_PRINTF_PLUS_FLAG_WITH_C_CONVERSION_SPECIFIER_TESTS
                tc("<%+-3c>", '=', "<=  >");
#endif
                // UB and not widely supported
		//tc("<%03c>", '=', "<00=>");
		tc("<%-03c>", '=', "<=  >");
		tc("<%3.2c>", '=', "<  =>");
                // UB and not widely supported
		//tc("<%hc>", '=', "<=>");
#endif

		ts("<%#s>", "text", "<text>");
		ts("<% -6s>", "text", "<text  >");
#ifndef YALIBCT_DISABLE_PRINTF_PLUS_FLAG_WITH_S_CONVERSION_SPECIFIER_TESTS
                ts("<%+-6s>", "text", "<text  >");
#endif
                // UB and not widely supported
		//ts("<%06s>", "text", "<00text>");
		ts("<%-06s>", "text", "<text  >");
                // UB and not widely supported
		//ts("<%hs>", "text", "<text>");
	}

	/*
	 * Valid use cases of %lc and %ls in the POSIX locale.
	 */

#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_C_CONVERSION_SPECIFIER_TESTS
	tlc("<%lc>", L'=', "<=>");
	tlc("<%lc>", L'\t', "<\t>");
        // Unspecified and not widely supported
	//tlc_expect_fail("<%lc>", 0x03c0);
	tlc("<%-lc>", L'=', "<=>");
	tlc("<%2lc>", L'=', "< =>");
	tlc("<%-2lc>", L'=', "<= >");
#endif

#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS
	tls("<%ls>", L"text", "<text>");
        // Unspecified and not widely supported
	//tls_expect_fail("<%ls>", ws);
	//tls_expect_fail("<%ls>", wsbad);
	tls("<%-ls>", L"text", "<text>");
	tls("<%6ls>", L"text", "<  text>");
	tls("<%-6ls>", L"text", "<text  >");
	tls("<%.2ls>", L"text", "<te>");
	tls("<%4.2ls>", L"text", "<  te>");
	tls("<%-4.2ls>", L"text", "<te  >");
#endif

	/*
	 * Undefined behaviour of %lc and %ls in the POSIX locale.
	 */

	if (picky) {
            // Unspecified behavior and not widely supported
            //tlc("<%lc>", 0x00fe, "<\xfe>");
#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_C_CONVERSION_SPECIFIER_TESTS
		tlc("<%#lc>", L'=', "<=>");
		tlc("<% -3lc>", L'=', "<=  >");
                tlc("<%+-3lc>", L'=', "<=  >");
		// UB and not widely supported
		//tlc("<%03lc>", L'=', "<00=>");
		tlc("<%-03lc>", L'=', "<=  >");
		tlc("<%3.2lc>", L'=', "<  =>");
#endif
                // UB and not widely supported
		//tc("<%llc>", '=', "<=>");

#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS
		tls("<%#ls>", L"text", "<text>");
		tls("<% -6ls>", L"text", "<text  >");
                tls("<%+-6ls>", L"text", "<text  >");
		// UB and not widely supported
		//tls("<%06ls>", L"text", "<00text>");
		tls("<%-06ls>", L"text", "<text  >");
#endif
                // UB and not widely supported
		//ts("<%lls>", "text", "<text>");
	}

	/*
	 * Valid use cases of %lc and %ls in a UTF-8 locale.
	 */
        // TODO: handle the case where this fails on a system that doesn't have any UTF-8-like locale by default
	(void)setlocale(LC_CTYPE, "C.UTF-8");

#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_C_CONVERSION_SPECIFIER_TESTS
	tlc("<%lc>", L'=', "<=>");
	tlc("<%lc>", L'\t', "<\t>");
	tlc("<%lc>", 0x00fe, "<\xc3\xbe>");
	tlc("<%lc>", 0x03c0, "<\xcf\x80>");
        // Unspecified and not widely supported
	//tlc_expect_fail("<%lc>", 0x123456);
	tlc("<%-lc>", L'=', "<=>");
	tlc("<%-lc>", 0x03c0, "<\xcf\x80>");
#ifndef YALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS
	tlc("<%2lc>", L'=', "< =>");
	tlc("<%3lc>", 0x03c0, "< \xcf\x80>");
	tlc("<%-2lc>", L'=', "<= >");
	tlc("<%-3lc>", 0x03c0, "<\xcf\x80 >");
#endif
#endif

#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS
	tls("<%ls>", ws, "<\xd0\xa1\xd0\xbe\xd1\x84\xd1\x8f>");
	// Unspecified and not widely supported
	//tls_expect_fail("<%ls>", wsbad);
	tls("<%-ls>", ws, "<\xd0\xa1\xd0\xbe\xd1\x84\xd1\x8f>");
#ifndef YALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_WIDE_S_CONVERSION_SPECIFIER_TESTS
	tls("<%9ls>", ws, "< \xd0\xa1\xd0\xbe\xd1\x84\xd1\x8f>");
	tls("<%-9ls>", ws, "<\xd0\xa1\xd0\xbe\xd1\x84\xd1\x8f >");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_PRECISION_ON_WIDE_S_CONVERSION_SPECIFIER_TESTS
	tls("<%.4ls>", ws, "<\xd0\xa1\xd0\xbe>");
	tls("<%.3ls>", ws, "<\xd0\xa1>");
	tls("<%6.4ls>", ws, "<  \xd0\xa1\xd0\xbe>");
	tls("<%3.3ls>", ws, "< \xd0\xa1>");
	tls("<%-6.4ls>", ws, "<\xd0\xa1\xd0\xbe  >");
	tls("<%-3.3ls>", ws, "<\xd0\xa1 >");
#endif
#endif

	/*
	 * Undefined behaviour of %lc and %ls in a UTF-8 locale.
	 */

	if (picky) {
#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_C_CONVERSION_SPECIFIER_TESTS
		tlc("<%#lc>", 0x03c0, "<\xcf\x80>");
#ifndef YALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_WIDE_C_CONVERSION_SPECIFIER_TESTS
		tlc("<% -4lc>", 0x03c0, "<\xcf\x80  >");
		tlc("<%+-4lc>", 0x03c0, "<\xcf\x80  >");
                // UB and not widely supported
		//tlc("<%04lc>", 0x03c0, "<00\xcf\x80>");
		tlc("<%-04lc>", 0x03c0, "<\xcf\x80  >");
		tlc("<%4.5lc>", 0x03c0, "<  \xcf\x80>");
		tlc("<%4.3lc>", 0x03c0, "<  \xcf\x80>");
		tlc("<%4.1lc>", 0x03c0, "<  \xcf\x80>");
#endif
#endif
                // UB and not widely supported
		//tc("<%llc>", 0xfe, "<\xfe>");

#ifndef YALIBCT_DISABLE_PRINTF_L_FLAG_ON_S_CONVERSION_SPECIFIER_TESTS
		tls("<%#ls>", ws + 2, "<\xd1\x84\xd1\x8f>");
#ifndef YALIBCT_DISABLE_PRINTF_FIELD_WIDTH_ON_WIDE_S_CONVERSION_SPECIFIER_TESTS
		tls("<% -6ls>", ws + 2, "<\xd1\x84\xd1\x8f  >");
		tls("<%+-6ls>", ws + 2, "<\xd1\x84\xd1\x8f  >");
                // UB and not widely supported
		//tls("<%06ls>", ws + 2, "<00\xd1\x84\xd1\x8f>");
		tls("<%-06ls>", ws + 2, "<\xd1\x84\xd1\x8f  >");
#endif
#endif
                // UB and not widely supported
		//ts("<%lls>", "text", "<text>");
	}

	/*
	 * Summarize the results.
	 */

	if (badret + badlen + badout)
		errx(1, "ERRORS: %d fail + %d mismatch (incl. %d bad length)",
		    badret, badout, badlen);
	else if (verbose)
		warnx("SUCCESS");
	return 0;
}
