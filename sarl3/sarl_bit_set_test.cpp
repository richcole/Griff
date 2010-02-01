#include "sarl_bit_set.h"
#include "sarl_assert.h"

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_bit_set_t* bset = sarl_bit_set_new(1);
  sarl_bit_set_t* bset2 = 0;
  sarl_bit_set_t* bset3 = 0;
  sarl_unsigned_int i = 0;

  for(i=1;i<10;++i) { 
    sarl_bit_set_set(bset, i);
  }

  for(i=1;i<10;++i) {
    sarl_test_assert( sarl_bit_set_is_set(bset, i) );
  }

  for(i=1;i<10;++i) { 
    sarl_bit_set_clear(bset, i);
  }

  for(i=1;i<38;++i) {
    sarl_test_assert( ! sarl_bit_set_is_set(bset, i) );
  }

  bset2 = sarl_bit_set_dup(bset);

  for(i=1;i<65;++i) { 
    sarl_bit_set_set(bset, i);
  }

  for(i=1;i<65;++i) {
    sarl_test_assert( ! sarl_bit_set_is_set(bset2, i) );
    sarl_test_assert( sarl_bit_set_is_set(bset, i) );
  }

  sarl_bit_set_clear(bset);
  sarl_bit_set_clear(bset2);
  sarl_bit_set_set(bset, 1);
  sarl_bit_set_set(bset, 2);
  sarl_bit_set_set(bset2, 1);
  sarl_bit_set_set(bset2, 3);
  bset3 = sarl_bit_set_and(bset, bset2);
  sarl_test_assert( sarl_bit_set_count(bset3) == 1);
  sarl_test_assert( sarl_bit_set_is_set(bset3,1) );
  sarl_test_assert( ! sarl_bit_set_is_set(bset3,2) );
  sarl_test_assert( ! sarl_bit_set_is_set(bset3,3) );

  sarl_bit_set_clear(bset);
  for(i=1;i<192;++i) {
    sarl_bit_set_clear(bset);
    sarl_bit_set_set(bset, i);
    sarl_test_assert(sarl_bit_set_count(sarl_bit_set_neg(i, bset)) == i-1);
  }

  fprintf(stdout, "Finished.\n");
  return 0;
}
