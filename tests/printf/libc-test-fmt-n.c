// commit: 5cbd76c6b05b381f269e0e204e10690d69f1d6ea 2011-02-16
// commit: bdc9ed15651b70e89f83c5a9f7d1ba349e624503 2011-02-20
// printf %n fmt
#include "test-deps/libc-test.h"
#include <stdint.h>
#include <stdio.h>

#define T(n,nfmt,fmt) do { \
        ret = sprintf(buf, "%256d%d" nfmt "%d", 1, 2, &n, 3); \
	if (ret != 258) \
		t_error("expexted sprintf to write 258 chars, got %d\n", ret); \
	if (n != 257) \
		t_error("%%n format failed: wanted 257, got " fmt "\n", n); \
} while(0)

int main(void)
{
	char buf[1024];
	int ret;
	int i = -1;
	long l = -1;
	long long ll = -1;
	short h = -1;
	size_t z = SIZE_MAX;
	uintmax_t j = UINTMAX_MAX;

#ifndef YALIBCT_DISABLE_PRINTF_N_CONVERSION_SPECIFIER_TESTS
	T(i,    "%n",   "%d");
	T(l,   "%ln",  "%ld");
	T(ll, "%lln", "%lld");
	T(h,   "%hn",   "%d");
	T(z,   "%zn",  "%zd");
	T(j,   "%jn",  "%jd");
#endif

	return t_status;
}
