#include "sarl_min_unsat.h"
#include "sarl_sat.h"
#include "sarl_assert.h"
#include "sarl_power_set.h"

#include "sarl_cnf.h"

int main(int argc, char **argv)
{
  if ( argc != 3 ) {
    fprintf(stderr, "Usage: %s <package.cnf> <unsat_clauses.cnf>\n", argv[0]);
    exit(-1);
  }

  FILE *outp = fopen(argv[2], "w");
  if ( ! outp ) {
    fprintf(stderr, "Error: unable to open '%s' for writing.\n");
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

  sarl_bit_set_t *U = sarl_bit_set_default_new();
  sarl_unsigned_int clause_index;
  sarl_unsigned_int num_clauses = sarl_array_count(solver->clauses);
  for(clause_index=1;clause_index<=num_clauses;++clause_index) {
    sarl_bit_set_set(U, clause_index);
  }

  sarl_power_set_t* min_unsat = sarl_min_unsat(U, solver);

  fprintf(stdout, "Min Unsat: ");
  sarl_power_set_print_to_file(min_unsat, stdout);
  fprintf(stdout, "\n");

  fprintf(stdout, "Dumping Clauses: ");
  sarl_bit_set_t *relevent_clauses = sarl_bit_set_default_new();
  sarl_bit_set_t *bs;
  SARL_POWER_SET_FOR_EACH(bs, min_unsat);
    sarl_bit_set_or_equals(relevent_clauses, bs);
  SARL_POWER_SET_END;

  sarl_clause_t *clause;
  SARL_BIT_SET_FOR_EACH(clause_index, relevent_clauses);
    clause = (sarl_clause_t *)sarl_array_get(solver->clauses, clause_index);
    sarl_clause_print(clause, outp);
    fprintf(outp, "\n");
  SARL_BIT_SET_END;
  fprintf(stdout, "Finished.\n");
};

