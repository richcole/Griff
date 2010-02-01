#include "sarl_prune_clauses.h"
#include "sarl_sat.h"
#include "sarl_assert.h"
#include "sarl_power_set.h"
#include "sarl_power_map.h"
#include "sarl_cnf.h"

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  {
    sarl_signed_int a1[] = {-1, 2, 3, 4, 0 };
    sarl_signed_int a2[] = {-2, 5, 0       };
    sarl_signed_int a3[] = {-3,-5, 0       };
    sarl_signed_int a4[] = {-4, 6, 7,0     };

    sarl_sat_solver_t *solver = sarl_sat_solver_default_new();
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(a1));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(a2));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(a3));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(a4));

    sarl_sat_solver_t *reduced_solver = sarl_prune_clauses(solver, 1);

    fprintf(stdout, "  Reduced clauses:\n");
    sarl_clause_t* clause;
    SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, reduced_solver->clauses);
      fprintf(stdout, "    ");
      sarl_clause_print(clause, stdout);  
      fprintf(stdout, "\n");
    SARL_ARRAY_END;
  }

  {
    fprintf(stdout, "\n  Testing Graph and Upset\n");
    sarl_sat_solver_t *solver = sarl_sat_solver_default_new();
    
    sarl_test_assert( 
      load_cnf(solver, "test/test-1.cnf") == LOAD_CNF_RESULT_OK 
    );
    
    sarl_power_map_t* graph = sarl_sat_dep_graph(solver);
    sarl_power_map_fill_nulls(graph, sarl_bit_set_last(solver->vars));
    
    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(graph, 1)) == 0
    );
    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(graph, 2)) == 2
    );
    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(graph, 3)) == 1
    );
    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(graph, 5)) == 1
    );

    sarl_power_map_t *upclosure = sarl_power_map_default_new();
    sarl_power_map_t *downclosure = sarl_power_map_default_new();

    sarl_transitive_closure(graph, upclosure, downclosure);
    sarl_power_map_fill_nulls(upclosure, sarl_bit_set_last(solver->vars));
    sarl_power_map_fill_nulls(downclosure, sarl_bit_set_last(solver->vars));

    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(upclosure, 1)) == 0
    );
    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(upclosure, 2)) == 3
    );
    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(upclosure, 3)) == 1
    );
    sarl_test_assert(
      sarl_bit_set_count(sarl_power_map_get(upclosure, 5)) == 2
    );

  }
  
  fprintf(stdout, "Finished.\n");
};
