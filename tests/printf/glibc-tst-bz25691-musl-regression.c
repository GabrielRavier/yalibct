#include <wchar.h>
#include <assert.h>

int main(void) {
   char buffer[500];
   for (size_t i = 0; i < sizeof(buffer); ++i)
      buffer[i] = (i % 3) ? 0 : 42;

   wchar_t result;

#ifdef YALIBCT_LIBC_HAS_SWPRINFT
   int ret = swprintf(&result, 1, L"%1$s", "");
   assert(ret != -1);
#endif
}
