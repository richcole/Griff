#include "sarl_factor_clauses.h"
#include "sarl_sat.h"
#include "sarl_assert.h"
#include "sarl_power_set.h"
#include "sarl_cnf.h"

int main(int argc, char **argv)
{
  if ( argc != 4 ) {
    fprintf(stderr, 
      "Usage: %s <package.cnf> <reduced.cnf> <merge_mapping.txt>");
  }

  FILE *reduced_p = fopen(argv[2], "w");
  FILE *merge_p = fopen(argv[3], "w");

  if ( ! reduced_p ) {
    fprintf(stderr, "Error: unable to open '%s'\n", argv[2]);
    exit(-1);
  };

  if ( ! merge_p ) {
    fprintf(stderr, "Error: unable to open '%s'\n", argv[3]);
    exit(-1);
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

  fprintf(stdout, "Number of Clauses: %u\n", sarl_array_count(solver->clauses));
  // reduce the number of clauses
  solver = sarl_sat_solver_remove_duplicates(solver);
  fprintf(stdout, "  after removing duplcates: %u\n", sarl_array_count(solver->clauses));

  // now factor the graph
  sarl_clause_factoring_t* factor = sarl_sat_solver_factor(solver);

  // print out the result
  fprintf(stdout, "clauses: count=%u\n", sarl_bit_set_count(factor->valid_clauses));
  sarl_unsigned_int x, y;
  SARL_BIT_SET_FOR_EACH(x, factor->valid_clauses);
  sarl_clause_print(
    (sarl_clause_t *)sarl_array_get(factor->solver->clauses, x), reduced_p);
  fprintf(reduced_p, "\n");
  SARL_BIT_SET_END;
  
  fprintf(stdout, "merge mapping:\n");
  SARL_INT_ARRAY_FOR_EACH_INDEX(x, y, factor->merge_map);
  fprintf(merge_p, "  %u %u\n", x, y);
  SARL_INT_ARRAY_END;

  fclose(reduced_p);
  fclose(merge_p);
};

