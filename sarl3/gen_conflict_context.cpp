#include "sarl_sat.h"
#include "sarl_power_set.h"
#include "sarl_cnf.h"
#include <errno.h>

bool is_sat(
  sarl_sat_solver_t *solver, sarl_sat_solver_t *d, sarl_bit_set_t *clauses)
{
  sarl_sat_state_t *state = sarl_sat_save_state(solver);
  sarl_unsigned_int i;
  bool result = false;

  SARL_BIT_SET_FOR_EACH(i, clauses);
    result = sarl_sat_solver_add_clause(
      solver, (sarl_clause_t *)sarl_array_get(d->clauses, i));
    if ( !result ) {
      sarl_sat_restore_state(solver, state);
      return false;
    }
  SARL_BIT_SET_END;

  result = sarl_sat_solver_search(solver);
  sarl_sat_restore_state(solver, state);
  return result;
};

sarl_bit_set_t* search_up(
  sarl_sat_solver_t *b_solver,
  sarl_sat_solver_t *d_solver,
  sarl_bit_set_t    *curr
)
{
  sarl_unsigned_int i;
  SARL_BIT_SET_FOR_EACH(i, curr);
    sarl_bit_set_clear(curr, i);
    if ( ! is_sat(b_solver, d_solver, curr) )
      return search_up(b_solver, d_solver, curr);
    sarl_bit_set_set(curr, i);
  SARL_BIT_SET_END;

  return curr;
};

sarl_bit_set_t* search_down(
  sarl_sat_solver_t *b_solver,
  sarl_sat_solver_t *d_solver,
  sarl_bit_set_t     *avoid_set,
  sarl_unsigned_int k,
  sarl_unsigned_int n
)
{
  sarl_bit_set_t *curr = sarl_bit_set_default_new();

  while ( k <= n ) {
    if ( ! sarl_bit_set_is_set(avoid_set, k) ) {
      sarl_bit_set_set(curr, k);
      if ( ! is_sat(b_solver, d_solver, curr) ) {
        return search_up(b_solver, d_solver, curr);
      }
    }
    ++k;
  }

  return 0;
}

void sarl_search_conflicts(
  sarl_sat_solver_t *b_solver, sarl_sat_solver_t *d_solver, 
  sarl_bit_set_t    *avoid_set, sarl_bit_set_t    *conflict,
  sarl_unsigned_int n
)
;

int main(int argc, char **argv)
{
  if ( argc != 3 ) {
    fprintf(stderr, 
      "Usage: gen_conflict_context <b.cnf> <d.cnf> <output.cxt>\n"
    );
    exit(-1);
  }

  sarl_sat_solver_t *b_solver = sarl_sat_solver_default_new();

  switch ( load_cnf(b_solver, argv[1]) ) {
  case LOAD_CNF_RESULT_FILE_ERROR:
    fprintf(stderr, "Exiting.\n");
    return -1;
  case LOAD_CNF_RESULT_UNSAT:
    fprintf(stderr, "Error: background clauses were unsatisfiable.");
    return -1;
  case LOAD_CNF_RESULT_OK:
    ;
  }

  sarl_sat_solver_t *d_solver = sarl_sat_solver_default_new();

  switch ( load_cnf(d_solver, argv[2]) ) {
  case LOAD_CNF_RESULT_FILE_ERROR:
    fprintf(stderr, "  Exiting.\n");
    return -1;
  case LOAD_CNF_RESULT_UNSAT:
    fprintf(stderr, "Error: direct clauses were unsatisfiable.");
    fprintf(stderr, "  Exiting.\n");
    return -1;
  case LOAD_CNF_RESULT_OK:
    ;
  }

  sarl_unsigned_int  n         = sarl_array_count(d_solver->clauses);
  sarl_bit_set_t    *avoid_set = sarl_bit_set_default_new();
  sarl_bit_set_t    *conflict;

  conflict = search_down(b_solver, d_solver, avoid_set, 1, n);

  if ( conflict ) {
    sarl_bit_set_print_to_file(conflict, stdout);
    fprintf(stdout, "\n");
    sarl_search_conflicts(b_solver, d_solver, avoid_set, conflict, n);
  }
};

void sarl_search_conflicts(
  sarl_sat_solver_t *b_solver, sarl_sat_solver_t *d_solver, 
  sarl_bit_set_t    *avoid_set, sarl_bit_set_t    *conflict,
  sarl_unsigned_int n
)
{
  sarl_unsigned_int k;
  sarl_bit_set_t *tmp_set = sarl_bit_set_default_new();
  sarl_bit_set_t *conflict2;

  SARL_BIT_SET_FOR_EACH(k, conflict);
    sarl_bit_set_assign_equals(tmp_set, avoid_set);
    sarl_bit_set_set(tmp_set, k);
    conflict2 = search_down(b_solver, d_solver, tmp_set, 1, n);
    if ( conflict2 ) {
      sarl_bit_set_print_to_file(conflict2, stdout);
      fprintf(stdout, "\n");
      sarl_search_conflicts(b_solver, d_solver, tmp_set, conflict2, n);
    }
  SARL_BIT_SET_END;
};
