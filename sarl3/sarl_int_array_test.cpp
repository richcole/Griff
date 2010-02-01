#include "sarl_int_array.h"
#include "sarl_assert.h"

int main()
{
  sarl_test_begin("sarl_int_array");

  // test heap functionality
  sarl_int_array_t *values = sarl_int_array_default_new();
  sarl_int_array_t *heap   = sarl_int_array_default_new();

  sarl_int_array_set(values, 1, 4);
  sarl_int_array_set(values, 2, 2);
  sarl_int_array_set(values, 3, 8);
  sarl_int_array_set(values, 4, 2);
  sarl_int_array_set(values, 5, 1);

  sarl_unsigned_int i;
  for(i=1;i<=5;++i) {
    sarl_int_array_heap_push(heap, values, i);
  };

  sarl_test_assert( sarl_int_array_count(heap) == 5 );

  sarl_test_assert( sarl_int_array_get(heap, 1) == 5 );
  sarl_test_assert( sarl_int_array_get(heap, 2) == 4 );
  sarl_test_assert( sarl_int_array_get(heap, 3) == 2 );
  sarl_test_assert( sarl_int_array_get(heap, 4) == 1 );
  sarl_test_assert( sarl_int_array_get(heap, 5) == 3 );

  fprintf(stdout, "heap=[");
  for(i=1;i<=5;++i) {
    fprintf(stdout, "%u ", sarl_int_array_get(heap, i));
  }
  fprintf(stdout, "]\n");

  sarl_test_assert( sarl_int_array_pop(heap) == 3 );
  sarl_test_assert( sarl_int_array_count(heap) == 4 );
  sarl_test_assert( sarl_int_array_pop(heap) == 1 );
  sarl_test_assert( sarl_int_array_count(heap) == 3 );
  sarl_test_assert( sarl_int_array_pop(heap) == 2 );
  sarl_test_assert( sarl_int_array_pop(heap) == 4 );
  sarl_test_assert( sarl_int_array_pop(heap) == 5 );

  sarl_test_assert( sarl_int_array_count(heap) == 0 );

  sarl_test_end;
};

