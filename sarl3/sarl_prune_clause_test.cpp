#include "sarl_prune.h"
#include "sarl_assert.h"

int main()
{
  sarl_sat_solver_t *solver = sarl_sat_solver_default_new();
  
  sarl_test_assert( 
    load_cnf(solver, "test/test-1.cnf") == LOAD_CNF_RESULT_OK 
  );

  sarl_power_map_t* graph = dep_graph(solver);

  sarl_test_assert(
    sarl_bit_set_count(sarl_power_map_get(graph, 1)) == 0
  );
  sarl_test_assert(
    sarl_bit_set_count(sarl_power_map_get(graph, 2)) == 3
  );
  sarl_test_assert(
    sarl_bit_set_count(sarl_power_map_get(graph, 3)) == 1
  );
  sarl_test_assert(
    sarl_bit_set_count(sarl_power_map_get(graph, 5)) == 2
  );

};
