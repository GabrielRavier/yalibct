#include "test-deps/libc-test.h"
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))

int main(void)
{
	struct stat st;
	FILE *f;
	time_t t;

	if (TEST(stat(".",&st)==0, "errno = %s\n", strerror(errno))) {
		TEST(S_ISDIR(st.st_mode), "\n");
		TEST(st.st_nlink>0, "%ju\n", (uintmax_t)st.st_nlink);
		t = time(0);
		TEST(st.st_ctime<=t, "%jd > %jd\n", (intmax_t)st.st_ctime, (intmax_t)t);
		TEST(st.st_mtime<=t, "%jd > %jd\n", (intmax_t)st.st_mtime, (intmax_t)t);
		TEST(st.st_atime<=t, "%jd > %jd\n", (intmax_t)st.st_atime, (intmax_t)t);
	}

	if (TEST(stat("/dev/null",&st)==0, "errno = %s\n", strerror(errno))) {
		TEST(S_ISCHR(st.st_mode), "\n");
	}

	f = tmpfile();
	if (f) {
		assert(fputs("hello", f) >= 0);
		assert(fflush(f) == 0);
		if (TEST(fstat(fileno(f),&st)==0, "errnp = %s\n", strerror(errno))) {
			TEST(st.st_uid==geteuid(), "%d vs %d\n", (int)st.st_uid, (int)geteuid());
			TEST(st.st_gid==getegid(), "%d vs %d\n", (int)st.st_uid, (int)geteuid());
			TEST(st.st_size==5, "%jd vs 5\n", (intmax_t)st.st_size);
		}
		assert(fclose(f) == 0);
	}

	return t_status;
}
