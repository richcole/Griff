#include "sarl_context.h"

#include "sarl_power_set.h"
#include "sarl_bit_set.h"
#include "sarl_assert.h"

#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_context_t *context = sarl_context_new();
  sarl_context_insert(context, 1, 1);
  sarl_context_insert(context, 1, 2);
  sarl_context_insert(context, 2, 3);
  sarl_context_insert(context, 2, 1);

  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_object_intent(context, 1), 1) 
  );
  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_object_intent(context, 1), 2) 
  );
  sarl_test_assert( 
    ! sarl_bit_set_is_set(sarl_context_object_intent(context, 1), 3) 
  );

  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_object_intent(context, 2), 1) 
  );
  sarl_test_assert( 
    ! sarl_bit_set_is_set(sarl_context_object_intent(context, 2), 2) 
  );
  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_object_intent(context, 2), 3) 
  );

  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_attribute_extent(context, 1), 1) 
  );
  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_attribute_extent(context, 1), 2) 
  );
  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_attribute_extent(context, 2), 1) 
  );
  sarl_test_assert( 
    sarl_bit_set_is_set(sarl_context_attribute_extent(context, 3), 2) 
  );

  sarl_bit_set_t *A = sarl_bit_set_default_new();
  sarl_bit_set_set(A, 1);
  sarl_bit_set_set(A, 2);

  sarl_bit_set_t *intent = sarl_context_intent(context, A);

  sarl_test_assert( sarl_bit_set_is_set(intent, 1) );
  fprintf(stdout, "BitSet Count: %u\n", sarl_bit_set_count(intent));
  sarl_test_assert( sarl_bit_set_count(intent) == 1);

  fprintf(stdout, "Finished.\n");
};
