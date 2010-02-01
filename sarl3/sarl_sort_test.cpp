#include "sarl_sort.h"
#include "sarl_assert.h"

int main()
{
  sarl_test_begin("sarl_sort");

  sarl_int_array_t *list = sarl_int_array_default_new();
  sarl_int_array_t *perm = 0;
  sarl_unsigned_int i, n = 10;

  // test on a sorted list
  for(i=1;i<=n;++i) {
    sarl_int_array_set(list, i, i);
  }

  perm = sarl_int_array_sort(list);

  for(i=1;i<=n;++i) {
    sarl_test_assert(sarl_int_array_get(perm, i) == i );
    sarl_test_assert(sarl_int_array_get(list, i) == i );
  }

  sarl_test_assert(sarl_int_array_count(list) == n);
  sarl_test_assert(sarl_int_array_count(perm) == n);

  // test on a reverse sorted list
  for(i=1;i<=n;++i) {
    sarl_int_array_set(list, i, n+1-i);
  }

  perm = sarl_int_array_sort(list);

  for(i=1;i<=n;++i) {
    sarl_test_assert(sarl_int_array_get(perm, i) == n+1-i );
    sarl_test_assert(sarl_int_array_get(list, i) == i );
  }

  sarl_test_assert(sarl_int_array_count(list) == n);
  sarl_test_assert(sarl_int_array_count(perm) == n);

  sarl_test_end;
};
