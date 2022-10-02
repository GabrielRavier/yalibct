/* Test to verify that a strlen() call with a pointer to a dynamic type
   doesn't make assumptions based on the static type of the original
   pointer.  See g++.dg/init/strlen.C for the corresponding C++ test.  */

#include "test-lib/portable-symbols/__builtin_abort.h"
#include "test-lib/compiler-features.h"

struct A { int i; char a[1]; void (*p)(); };
struct B { char a[sizeof (struct A) - __builtin_offsetof (struct A, a)]; };

YALIBCT_ATTRIBUTE_NOIPA void
init (char *d, const char *s)
{
  __builtin_strcpy (d, s);
}

struct B b;

YALIBCT_ATTRIBUTE_NOIPA void
test_dynamic_type (struct A *p)
{
  /* The following call is undefined because it writes past the end
     of the p->a subobject, but the corresponding GIMPLE considers
     it valid and there's apparently no way to distinguish invalid
     cases from ones like it that might be valid.  If/when GIMPLE
     changes to make this possible this test can be removed.  */
  char *q = (char*)__builtin_memcpy (p->a, &b, sizeof b);

  init (q, "foobar");

  if (6 != __builtin_strlen (q))
    __builtin_abort();
}

int main (void)
{
  struct A *p = (struct A*)__builtin_malloc (sizeof *p);
  test_dynamic_type (p);
  return 0;
}
