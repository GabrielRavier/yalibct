// SPDX-License-Identifier: GPL-2.0
#include "test-deps/linux-kernel.h"
#include "test-lib/portable-symbols/alignalloc.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum {
    SIZE = 256,
    ITERATIONS = 1000,
    ITERATIONS_BENCH = 100000,
};

static inline size_t test_strlen(const char *s)
{
        const char *sc;

        for (sc = s; *sc != '\0'; ++sc)
                /* nothing */;
        return sc - s;
}

/* test all offsets and lengths */
static void test_one(char *s)
{
	unsigned long offset;

	for (offset = 0; offset < SIZE; offset++) {
		int x, y;
		unsigned long i;

		x = test_strlen(s + offset);
		y = strlen(s + offset);

		if (x != y) {
			printf("strlen() returned %d, should have returned %d (%p offset %ld)\n", x, y, s, offset);

			for (i = offset; i < SIZE; i++)
				printf("%02x ", s[i]);
			printf("\n");
		}
	}
}

static void bench_test(char *s)
{
	struct timespec ts_start, ts_end;
	int i;

	clock_gettime(CLOCK_MONOTONIC, &ts_start);

	for (i = 0; i < ITERATIONS_BENCH; i++)
		test_strlen(s);

	clock_gettime(CLOCK_MONOTONIC, &ts_end);

	//printf("len %3.3zu : time = %.6f\n", test_strlen(s), ts_end.tv_sec - ts_start.tv_sec + (ts_end.tv_nsec - ts_start.tv_nsec) / 1e9);
}

static int testcase(void)
{
	char *s;
	unsigned long i;

	s = alignalloc(128, SIZE + 1);
	if (!s) {
		perror("memalign");
		exit(1);
	}

	srandom(1);

	memset(s, 0, SIZE + 1);
	for (i = 0; i < SIZE; i++) {
		char c;

		do {
			c = random() & 0x7f;
		} while (!c);
		s[i] = c;
		test_one(s);
	}

	for (i = 0; i < ITERATIONS; i++) {
		unsigned long j;

		for (j = 0; j < SIZE; j++) {
			char c;

			do {
				c = random() & 0x7f;
			} while (!c);
			s[j] = c;
		}
		for (j = 0; j < sizeof(long); j++) {
			s[SIZE - 1 - j] = 0;
			test_one(s);
		}
	}

	for (i = 0; i < SIZE; i++) {
		char c;

		do {
			c = random() & 0x7f;
		} while (!c);
		s[i] = c;
	}

	bench_test(s);

	s[16] = 0;
	bench_test(s);

	s[8] = 0;
	bench_test(s);

	s[4] = 0;
	bench_test(s);

	s[3] = 0;
	bench_test(s);

	s[2] = 0;
	bench_test(s);

	s[1] = 0;
	bench_test(s);

	return 0;
}

int main(void)
{
	return test_harness(testcase, "strlen");
}
