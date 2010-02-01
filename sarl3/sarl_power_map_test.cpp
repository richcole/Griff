#include "sarl_power_map.h"

#include "sarl_power_set.h"
#include "sarl_assert.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_power_map_t *pmap = sarl_power_map_default_new();
  sarl_power_map_insert(pmap, 1, 1);
  sarl_power_map_insert(pmap, 1, 2);
  sarl_power_map_insert(pmap, 2, 3);
  sarl_test_assert( sarl_bit_set_is_set(sarl_power_map_get(pmap, 1), 1) );
  sarl_test_assert( sarl_bit_set_is_set(sarl_power_map_get(pmap, 1), 2) );
  sarl_test_assert( ! sarl_bit_set_is_set(sarl_power_map_get(pmap, 1), 3) );
  sarl_test_assert( ! sarl_bit_set_is_set(sarl_power_map_get(pmap, 2), 1) );
  sarl_test_assert( ! sarl_bit_set_is_set(sarl_power_map_get(pmap, 2), 2) );
  sarl_test_assert( sarl_bit_set_is_set(sarl_power_map_get(pmap, 2), 3) );

  fprintf(stdout, "Finished.\n");
};
