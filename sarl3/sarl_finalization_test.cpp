#include "sarl_finalization.h"
#include "sarl_array.h"

int one()
{
    sarl_array_t *a = sarl_array_default_new();
    sarl_array_push(a, sarl_array_default_new());
    fprintf(stderr, "a=%p, a[0]=%p\n", a, sarl_array_get(a, 1));
    return 0;
};

int main()
{
  one();
  GC_gcollect();
};
