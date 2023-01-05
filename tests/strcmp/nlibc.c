#include "string.h"
#include "assert.h"

int main()
{
	char *a="hello";
	char *b="iello";

	assert((strcmp(a, b) < 0));
	assert((strcmp(b, a) > 0));
	assert((strcmp(a, a) == 0));
	return 0;
}
