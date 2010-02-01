#include "sarl_prune_clauses.h"
#include "sarl_sat.h"
#include "sarl_assert.h"
#include "sarl_power_set.h"
#include "sarl_power_map.h"
#include "sarl_cnf.h"

int main(int argc, char **argv)
{
  if ( argc != 4 ) {
    fprintf(stderr, 
      "Usage: %s <package.cnf> <target_index> <pruned.cnf>", argv[0]);
    exit(0);
  }  
  
  sarl_unsigned_int target_index;
  if ( 1 != sscanf(argv[2], "%u", &target_index) ) {
    fprintf(stderr, "Error: unable to parse target index: '%s'\n", argv[2]);
  }

  FILE *outp = fopen(argv[3], "w");
  if ( outp == 0 ) {
    fprintf(stderr, "Error opening output file '%s'\n", argv[3]);
  };

  sarl_sat_solver_t *solver = sarl_sat_solver_default_new();

  switch ( load_cnf(solver, argv[1]) ) {
  case LOAD_CNF_RESULT_FILE_ERROR:
    fprintf(stderr, "Exiting.\n");
    return -1;
  case LOAD_CNF_RESULT_UNSAT:
    break;
  case LOAD_CNF_RESULT_OK:
    ;
  }

  fprintf(stdout, "Number of Clauses: %u\n", 
    sarl_array_count(solver->clauses));
  
  // collect the up and downclosure of elements

  sarl_power_map_t* graph = sarl_sat_dep_graph(solver);
  sarl_power_map_t *upclosure = sarl_power_map_default_new();
  sarl_power_map_t *downclosure = sarl_power_map_default_new();

  sarl_transitive_closure(graph, upclosure, downclosure);
  sarl_bit_set_t *conflict_vars = 
    sarl_sat_find_conflicts(solver, upclosure, downclosure, target_index);
  sarl_bit_set_t *relevent_vars = 
    sarl_power_map_union_closure(conflict_vars, upclosure);
  sarl_bit_set_set(relevent_vars, target_index);

  fprintf(stdout, "Number of Vars: %u\n", 
    sarl_bit_set_count(solver->vars));
  fprintf(stdout, "Number of Relevent Vars: %u\n", 
    sarl_bit_set_count(relevent_vars));

  solver = sarl_select_relevent_clauses(solver, relevent_vars);

  fprintf(stdout, "Number of Relevent Clauses: %u\n", 
    sarl_array_count(solver->clauses));

  sarl_clause_t *clause;
  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
    sarl_clause_print(clause, outp);
    fprintf(outp, "\n");
  SARL_ARRAY_END;

  fclose(outp);
};
