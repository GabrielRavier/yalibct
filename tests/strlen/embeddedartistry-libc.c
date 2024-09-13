/*
 * Copyright © 2017 Embedded Artistry LLC.
 * License: MIT. See LICENSE file for details.
 */

//#include "string_tests.h"
#include "test-deps/cmocka.h"
#include "test-lib/compiler-features.h"
#include <string.h>


static void strlen_test(void** state)
{
	(void)state;
	assert_int_equal(strlen(""), 0);
	assert_int_equal(strlen("a"), 1);
	assert_int_equal(strlen("ab"), 2);
	assert_int_equal(strlen("abasildjfaskjdf;askdfuoisudfoiajsdfaf"), 37);
}

#if defined(__GNUC__) && !defined(__clang__)
YALIBCT_DIAGNOSTIC_IGNORE("-Wunknown-pragmas")
#endif

#pragma mark - Public Functions -

int main(void)
{
	const struct CMUnitTest strlen_tests[] = {
		cmocka_unit_test(strlen_test),
	};

	return cmocka_run_group_tests(strlen_tests, NULL, NULL);
}
