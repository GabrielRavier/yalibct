// SPDX-License-Identifier: GPL-2.0-only
/*
 * Test cases for printf facility.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "test-deps/linux-kernel.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define BUF_SIZE 256
#define PAD_SIZE 16
#define FILL_CHAR '$'

#define NOWARN(option, comment, block) GRAVIER_TESTS_EXPRESSION_NO_WARNING(option, block)

KSTM_MODULE_GLOBALS();

static char *test_buffer __initdata;
static char *alloced_buffer __initdata;

static bool no_hash_pointers = false;

static int __printf(4, 0) __init
do_test(int bufsize, const char *expect, int elen,
	const char *fmt, va_list ap)
{
	va_list aq;
	int ret, written;

	total_tests++;

	memset(alloced_buffer, FILL_CHAR, BUF_SIZE + 2*PAD_SIZE);
	va_copy(aq, ap);
	ret = vsnprintf(test_buffer, bufsize, fmt, aq);
	va_end(aq);

	if (ret != elen) {
		pr_warn("vsnprintf(buf, %d, \"%s\", ...) returned %d, expected %d\n",
			bufsize, fmt, ret, elen);
		return 1;
	}

	if (memchr_inv(alloced_buffer, FILL_CHAR, PAD_SIZE)) {
		pr_warn("vsnprintf(buf, %d, \"%s\", ...) wrote before buffer\n", bufsize, fmt);
		return 1;
	}

	if (!bufsize) {
		if (memchr_inv(test_buffer, FILL_CHAR, BUF_SIZE + PAD_SIZE)) {
			pr_warn("vsnprintf(buf, 0, \"%s\", ...) wrote to buffer\n",
				fmt);
			return 1;
		}
		return 0;
	}

	written = min(bufsize-1, elen);
	if (test_buffer[written]) {
		pr_warn("vsnprintf(buf, %d, \"%s\", ...) did not nul-terminate buffer\n",
			bufsize, fmt);
		return 1;
	}

	if (memchr_inv(test_buffer + written + 1, FILL_CHAR, bufsize - (written + 1))) {
		pr_warn("vsnprintf(buf, %d, \"%s\", ...) wrote beyond the nul-terminator\n",
			bufsize, fmt);
		return 1;
	}

	if (memchr_inv(test_buffer + bufsize, FILL_CHAR, BUF_SIZE + PAD_SIZE - bufsize)) {
		pr_warn("vsnprintf(buf, %d, \"%s\", ...) wrote beyond buffer\n", bufsize, fmt);
		return 1;
	}

	if (memcmp(test_buffer, expect, written)) {
		pr_warn("vsnprintf(buf, %d, \"%s\", ...) wrote '%s', expected '%.*s'\n",
			bufsize, fmt, test_buffer, written, expect);
		return 1;
	}
	return 0;
}

static void __printf(3, 4) __init
__test(const char *expect, int elen, const char *fmt, ...)
{
	va_list ap;
	int rand;
	char *p;

	if (elen >= BUF_SIZE) {
		pr_err("error in test suite: expected output length %d too long. Format was '%s'.\n",
		       elen, fmt);
		failed_tests++;
		return;
	}

	va_start(ap, fmt);

	/*
	 * Every fmt+args is subjected to four tests: Three where we
	 * tell vsnprintf varying buffer sizes (plenty, not quite
	 * enough and 0), and then we also test that kvasprintf would
	 * be able to print it as expected.
	 */
	failed_tests += do_test(BUF_SIZE, expect, elen, fmt, ap);
	rand = 1 + prandom_u32_max(elen+1);
	/* Since elen < BUF_SIZE, we have 1 <= rand <= BUF_SIZE. */
	failed_tests += do_test(rand, expect, elen, fmt, ap);
	failed_tests += do_test(0, expect, elen, fmt, ap);

	p = kvasprintf(GFP_KERNEL, fmt, ap);
	if (p) {
		total_tests++;
		if (memcmp(p, expect, elen+1)) {
			pr_warn("kvasprintf(..., \"%s\", ...) returned '%s', expected '%s'\n",
				fmt, p, expect);
			failed_tests++;
		}
		kfree(p);
	}
	va_end(ap);
}

#define test(expect, fmt, ...)					\
	__test(expect, strlen(expect), fmt, ##__VA_ARGS__)

static void __init
test_basic(void)
{
	/* Work around annoying "warning: zero-length gnu_printf format string". */
	char nul = '\0';

        GRAVIER_TESTS_EXPRESSION_NO_WARNING(-Wformat-security, {
            test("", &nul);
        })
	test("100%", "100%%");
	test("xxx%yyy", "xxx%cyyy", '%');
	__test("xxx\0yyy", 7, "xxx%cyyy", '\0');
}

static void __init
test_number(void)
{
	test("0x1234abcd  ", "%#-12x", 0x1234abcd);
	test("  0x1234abcd", "%#12x", 0x1234abcd);
	test("0|001| 12|+123| 1234|-123|-1234", "%d|%03d|%3d|%+d|% d|%+d|% d", 0, 1, 12, 123, 1234, -123, -1234);
	NOWARN(-Wformat, "Intentionally test narrowing conversion specifiers.", {
		test("0|1|1|128|255", "%hhu|%hhu|%hhu|%hhu|%hhu", 0, 1, 257, 128, -1);
		test("0|1|1|-128|-1", "%hhd|%hhd|%hhd|%hhd|%hhd", 0, 1, 257, 128, -1);
		test("2015122420151225", "%ho%ho%#ho", 1037, 5282, -11627);
	})
	/*
	 * POSIX/C99: »The result of converting zero with an explicit
	 * precision of zero shall be no characters.« Hence the output
	 * from the below test should really be "00|0||| ". However,
	 * the kernel's printf also produces a single 0 in that
	 * case. This test case simply documents the current
	 * behaviour.
	 */
        // Note: Test obviously modified to correspond to the POSIX behavior described above
	test("00|0||| ", "%.2d|%.1d|%.0d|%.*d|%1.0d", 0, 0, 0, 0, 0, 0);
#ifndef __CHAR_UNSIGNED__
	{
		/*
		 * Passing a 'char' to a %02x specifier doesn't do
		 * what was presumably the intention when char is
		 * signed and the value is negative. One must either &
		 * with 0xff or cast to u8.
		 */
		char val = -16;
		test("0xfffffff0|0xf0|0xf0", "%#02x|%#02x|%#02x", val, val & 0xff, (u8)val);
	}
#endif
}

static void __init
test_string(void)
{
	test("", "%s%.0s", "", "123");
	test("ABCD|abc|123", "%s|%.3s|%.*s", "ABCD", "abcdef", 3, "123456");
	test("1  |  2|3  |  4|5  ", "%-3s|%3s|%-*s|%*s|%*s", "1", "2", 3, "3", 3, "4", -3, "5");
	test("1234      ", "%-10.4s", "123456");
	test("      1234", "%10.4s", "123456");
	/*
	 * POSIX and C99 say that a negative precision (which is only
	 * possible to pass via a * argument) should be treated as if
	 * the precision wasn't present, and that if the precision is
	 * omitted (as in %.s), the precision should be taken to be
	 * 0. However, the kernel's printf behave exactly opposite,
	 * treating a negative precision as 0 and treating an omitted
	 * precision specifier as if no precision was given.
	 *
	 * These test cases document the current behaviour; should
	 * anyone ever feel the need to follow the standards more
	 * closely, this can be revisited.
	 */
        // Modified so the test checks for POSIX behavior
	test("123456", "%4.*s", -5, "123456");
	test("", "%.s", "123456");
	test("||", "%.s|%.0s|%.*s", "a", "b", 0, "c");
	test("   |   |   ", "%-3.s|%-3.0s|%-3.*s", "a", "b", 0, "c");
}

#define PLAIN_BUF_SIZE 64	/* leave some space so we don't oops */

#if BITS_PER_LONG == 64

#define PTR_WIDTH 16
#define PTR ((void *)0xffff0123456789abUL)
#define PTR_STR "ffff0123456789ab"
#define PTR_VAL_NO_CRNG "(____ptrval____)"
#define ZEROS "00000000"	/* hex 32 zero bits */
#define ONES "ffffffff"		/* hex 32 one bits */

static int __init
plain_format(void)
{
	char buf[PLAIN_BUF_SIZE];
	int nchars;

	nchars = snprintf(buf, PLAIN_BUF_SIZE, "%p", PTR);

	if (nchars != PTR_WIDTH)
		return -1;

	if (strncmp(buf, PTR_VAL_NO_CRNG, PTR_WIDTH) == 0) {
		pr_warn("crng possibly not yet initialized. plain 'p' buffer contains \"%s\"",
			PTR_VAL_NO_CRNG);
		return 0;
	}

	if (strncmp(buf, ZEROS, strlen(ZEROS)) != 0)
		return -1;

	return 0;
}

#else

#define PTR_WIDTH 8
#define PTR ((void *)0x456789ab)
#define PTR_STR "456789ab"
#define PTR_VAL_NO_CRNG "(ptrval)"
#define ZEROS ""
#define ONES ""

static int __init
plain_format(void)
{
	/* Format is implicitly tested for 32 bit machines by plain_hash() */
	return 0;
}

#endif	/* BITS_PER_LONG == 64 */

static void __init
symbol_ptr(void)
{
}

static void __init
kernel_ptr(void)
{
	/* We can't test this without access to kptr_restrict. */
}

static void __init
struct_resource(void)
{
}

static void __init
addr(void)
{
}

static void __init
escaped_str(void)
{
}

static void __init
struct_va_format(void)
{
}

static void __init
struct_clk(void)
{
}

static void __init
netdev_features(void)
{
}

static void __init
errptr(void)
{
	/* Check that %pe with a non-ERR_PTR gets treated as ordinary %p. */
	BUILD_BUG_ON(IS_ERR(PTR));

#ifdef CONFIG_SYMBOLIC_ERRNAME
	test("(-ENOTSOCK)", "(%pe)", ERR_PTR(-ENOTSOCK));
	test("(-EAGAIN)", "(%pe)", ERR_PTR(-EAGAIN));
	BUILD_BUG_ON(EAGAIN != EWOULDBLOCK);
	test("(-EAGAIN)", "(%pe)", ERR_PTR(-EWOULDBLOCK));
	test("[-EIO    ]", "[%-8pe]", ERR_PTR(-EIO));
	test("[    -EIO]", "[%8pe]", ERR_PTR(-EIO));
	test("-EPROBE_DEFER", "%pe", ERR_PTR(-EPROBE_DEFER));
#endif
}

// A few tests here have been disabled as they are not portable at all
static void __init
test_pointer(void)
{
    //plain();
    //null_pointer();
    //error_pointer();
    //invalid_pointer();
	symbol_ptr();
	kernel_ptr();
	struct_resource();
	addr();
	escaped_str();
	//hex_string();
	//mac();
	//ip();
	//uuid();
	//dentry();
	struct_va_format();
	//time_and_date();
	struct_clk();
	//bitmap();
	netdev_features();
	//flags();
	errptr();
	//fwnode_pointer();
	//fourcc_pointer();
}

int __init main(void)
{
	alloced_buffer = kmalloc(BUF_SIZE + 2*PAD_SIZE, GFP_KERNEL);
	if (!alloced_buffer)
		return 1;
	test_buffer = alloced_buffer + PAD_SIZE;

	test_basic();
	test_number();
	test_string();
	test_pointer();

	kfree(alloced_buffer);
        return failed_tests == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

KSTM_MODULE_LOADERS(test_printf);
MODULE_AUTHOR("Rasmus Villemoes <linux@rasmusvillemoes.dk>");
MODULE_LICENSE("GPL");
