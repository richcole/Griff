#include "sarl_power_set.h"
#include "sarl_assert.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_power_set_t* pset = sarl_power_set_new(13);
  sarl_power_set_t* closure = 0;
  sarl_bit_set_t* bset = sarl_bit_set_new(2);
  sarl_bit_set_t* bset2 = sarl_bit_set_new(2);
  sarl_unsigned_int i;

  for(i=1;i<=10;++i) { 
    sarl_bit_set_set(bset, i);
  }

  sarl_power_set_insert(pset, bset);

  sarl_test_assert( sarl_power_set_is_member(pset, bset) );

  sarl_power_set_insert(pset, bset);

  sarl_test_assert( sarl_power_set_is_member(pset, bset) );

  sarl_power_set_insert(pset, bset2);

  sarl_test_assert( sarl_power_set_is_member(pset, bset2) );
  sarl_test_assert( sarl_power_set_is_member(pset, bset) );

  sarl_bit_set_print_to_file(bset, stdout);
  fprintf(stdout, "\n");
  sarl_power_set_print_to_file(pset, stdout);
  fprintf(stdout, "\n");

  sarl_bit_set_clear(bset);
  sarl_test_assert( sarl_bit_set_count(bset) == 0 );

  sarl_power_set_clear(pset);
  sarl_test_assert( sarl_power_set_count(pset) == 0 );

  sarl_unsigned_int boolean_size = 8;
  for(i=1;i<=boolean_size;++i) {
    sarl_bit_set_set(bset, i);
  }
  
  for(i=1;i<=boolean_size;++i) {
    sarl_potential_code_leak(bset2);
    bset2 = sarl_bit_set_dup(bset);
    sarl_bit_set_clear(bset2, i);
    sarl_power_set_insert(pset, bset2);
  }

  timeval before, after;

  gettimeofday(&before, 0);
  closure = sarl_power_set_closure(pset, bset);
  gettimeofday(&after, 0);

  if ( before.tv_usec > after.tv_usec ) {
    after.tv_sec -= 1;
    after.tv_usec = after.tv_usec + (1000000 - before.tv_usec);
    after.tv_sec -= before.tv_sec;
  }
  else {
    after.tv_sec -= before.tv_sec;
    after.tv_usec -= before.tv_usec;
  }
  fprintf(stdout, "Closure Time: %2d.%06d\n", after.tv_sec, after.tv_usec);

  sarl_test_assert( sarl_power_set_count(pset) == boolean_size );

  fprintf(stdout, "Closure Count: %u\n", sarl_power_set_count(closure));
  fprintf(stdout, "Expected Count: %u\n", 
    (((sarl_unsigned_int)0x1) << boolean_size)
  );
  
  sarl_test_assert( 
    sarl_power_set_count(closure) == 
    (((sarl_unsigned_int)0x1) << boolean_size)
  );

  sarl_bit_set_clear(bset);
  do {
    sarl_test_assert( sarl_power_set_is_member(closure, bset) );
    if ( ! sarl_power_set_is_member(closure, bset) ) {
      fprintf(stdout, "  missing bset = ");
      sarl_bit_set_print_to_file(bset, stdout);
      fprintf(stdout, "\n");
    }
  } while( sarl_bit_set_next(bset, boolean_size) );

  sarl_potential_code_leak(bset);
  sarl_potential_code_leak(bset2);
  sarl_potential_code_leak(pset);

  fprintf(stdout, "Finished.\n");
  return 0;
}
