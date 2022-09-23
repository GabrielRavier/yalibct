// commit 109048e031f39fbb370211fde44ababf6c04c8fb 2014-04-07
// float printf out-of-bounds access
#include "test-deps/libc-test.h"
#include <stdio.h>
#include <string.h>

static void t(const char *fmt, double d, const char *want)
{
	char buf[256];
	int n = strlen(want);
	int r = snprintf(buf, sizeof buf, fmt, d);
	if (r != n || memcmp(buf, want, n+1) != 0)
		t_error("snprintf(\"%s\",%f) want %s got %s\n", fmt, d, want, buf);
}

int main()
{
	// fill stack with something
	t("%.1f", 123123123123123.0, "123123123123123.0");
	// test for out-of-bounds access
#ifndef YALIBCT_DISABLE_PRINTF_G_CONVERSION_SPECIFIER_TESTS
	t("%g", 999999999.0, "1e+09");
#endif
#ifndef YALIBCT_DISABLE_PRINTF_E_CONVERSION_SPECIFIER_TESTS
	t("%.3e", 999999999.75, "1.000e+09");
#endif
	return t_status;
}
