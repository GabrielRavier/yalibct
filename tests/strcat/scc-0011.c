#include <assert.h>
#include <stdio.h>
#include <string.h>

/*
output:
end:
*/

int
main(void)
{
	char *s, buf[40];

	strcpy(buf, "case1:");
	s = strcat(buf, "ok");
	assert(s == buf);
	assert(!strcmp(s, "case1:ok"));

	strcpy(buf, "");
	s = strcat(buf, "ok");
	assert(s == buf);
	assert(!strcmp(s, "ok"));

	strcpy(buf, "case1:");
	strcat(buf, "");
	assert(s == buf);
	assert(!strcmp(s, "case1:"));

	return 0;
}
