#include "sarl_min_unsat.h"
#include "sarl_sat.h"
#include "sarl_power_set.h"

sarl_unsigned_int sarl_is_subset_sat(
  sarl_sat_solver_t *solver, sarl_bit_set_t *S)
{
  sarl_unsigned_int c;
  sarl_sat_solver_t *s_solver = sarl_sat_solver_default_new();

  SARL_BIT_SET_FOR_EACH(c, S);
    if ( ! 
      sarl_sat_solver_add_clause(
      s_solver, (sarl_clause_t *)sarl_array_get(solver->clauses, c)
      )
    ) {
      return false;
    }
  SARL_BIT_SET_END;

  return sarl_sat_solver_search(s_solver);
};

sarl_power_set_t* sarl_min_unsat_hst(sarl_power_set_t *P)
{
  sarl_power_set_t  *R = sarl_power_set_default_new();
  sarl_bit_set_t    *S;
  sarl_bit_set_t    *n, *r, *t;
  sarl_unsigned_int c;

  SARL_POWER_SET_FOR_EACH(S, P);
    if ( sarl_power_set_count(R) == 0 ) {
      SARL_BIT_SET_FOR_EACH(c, S);
        r = sarl_bit_set_default_new();
        sarl_bit_set_set(r, c);
        sarl_power_set_insert(R, r);
      SARL_BIT_SET_END;
    }
    else {
      sarl_power_set_t *N = sarl_power_set_default_new();
      SARL_POWER_SET_FOR_EACH(r, R);
      SARL_BIT_SET_FOR_EACH(c, S);
        t = sarl_bit_set_dup(r);
        sarl_bit_set_set(t, c);
        sarl_power_set_insert(N, t);
      SARL_BIT_SET_END;
      SARL_POWER_SET_END;

      R = N;
      N = sarl_power_set_default_new();
      SARL_POWER_SET_FOR_EACH(r, R);
      SARL_POWER_SET_FOR_EACH(n, R);
      if ( sarl_bit_set_is_subset(n, r) ) {
        sarl_power_set_insert(N, r);
        break;
      }
      SARL_POWER_SET_END;
      SARL_POWER_SET_END;

      SARL_POWER_SET_FOR_EACH(n, N);
        sarl_power_set_remove(R, n);
      SARL_POWER_SET_END;
    }
  SARL_POWER_SET_END;

  return R;
};


sarl_power_set_t *
  sarl_min_unsat(sarl_bit_set_t *U, sarl_sat_solver_t *solver) 
{
  sarl_power_set_t *PA = sarl_power_set_default_new();
  sarl_power_set_t *PX = sarl_power_set_default_new(); 
  sarl_bit_set_t   *X = sarl_bit_set_default_new();

  do {
    fprintf(stdout, "PX: "); 
    sarl_power_set_print_to_file(PX, stdout);
    fprintf(stdout, "\n");

    sarl_bit_set_t   *M = sarl_min_unsat_grow(X, U, solver);

    fprintf(stdout, "M: "); 
    sarl_bit_set_print_to_file(M, stdout);
    fprintf(stdout, "\n");

    sarl_power_set_insert(PX, sarl_bit_set_minus(U, M));

    fprintf(stdout, "PX: "); 
    sarl_power_set_print_to_file(PX, stdout);
    fprintf(stdout, "\n");

    sarl_power_set_t *N = sarl_min_unsat_hst(PX);

    fprintf(stdout, "N: "); 
    sarl_power_set_print_to_file(N, stdout);
    fprintf(stdout, "\n");

    sarl_bit_set_t   *S;

    sarl_bit_set_clear(X);
    SARL_POWER_SET_FOR_EACH(S, N);
      if ( ! sarl_power_set_is_member(PA, S) ) {
        if ( sarl_is_subset_sat(solver, S) ) {
          X = S;
          break;
        }
        else {
          sarl_power_set_insert(PA, S);
        }
      }
    SARL_POWER_SET_END;
  }
  while ( sarl_bit_set_count(X) > 0 );

  return PA;
};

sarl_bit_set_t *sarl_min_unsat_grow(
  sarl_bit_set_t* S, sarl_bit_set_t *U, 
  sarl_sat_solver_t *solver)
{
  sarl_bit_set_t *cS = sarl_bit_set_minus(U, S);
  sarl_unsigned_int c;
  sarl_sat_solver_t *s_solver = sarl_sat_solver_default_new();

  SARL_BIT_SET_FOR_EACH(c, S);
    if ( ! sarl_sat_solver_add_clause(
      s_solver, (sarl_clause_t *)sarl_array_get(solver->clauses, c)
      )
    ) {
      fprintf(stderr, "Error: these clauses should be sat.\n");
    }
  SARL_BIT_SET_END;
    
  SARL_BIT_SET_FOR_EACH(c, cS);
    sarl_bit_set_set(S, c);
    sarl_sat_state_t *state = sarl_sat_save_state(s_solver);
    if ( 
      sarl_sat_solver_add_clause(
        s_solver, (sarl_clause_t *)sarl_array_get(solver->clauses, c) 
      )
    ) {
      sarl_sat_state_t *added_state = sarl_sat_save_state(s_solver);
      if ( sarl_sat_solver_search(s_solver) ) {
        sarl_sat_restore_state(s_solver, added_state);
      }
      else {
        sarl_sat_restore_state(s_solver, state);
        sarl_bit_set_clear(S, c);
      }
    }
    else {
      sarl_sat_restore_state(s_solver, state);
      sarl_bit_set_clear(S, c);
    }
  SARL_BIT_SET_END;

  return S;
};
