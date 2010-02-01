#include "sarl_sat.h"

sarl_sat_solver_t* sarl_sat_solver_default_new()
{
  sarl_sat_solver_t *solver = 
    (sarl_sat_solver_t *)GC_MALLOC(sizeof(sarl_sat_solver_t));
  if (solver) {
    solver->pos = sarl_bit_set_default_new();
    solver->neg = sarl_bit_set_default_new();
    solver->clauses = sarl_array_new();
  }

  return solver;
};

bool  sarl_sat_solver_add_clause(sarl_sat_solver_t *solver, sarl_clause_t *c)
{
  if ( ! sarl_bit_set_empty(sarl_bit_set_and(c->pos, c->neg)) ) {
    // tautology, ignore it
    return true;
  }

  sarl_array_push(solver->clauses, c);
  sarl_sat_solver_update_clause(solver, c);
};

bool  sarl_sat_solver_add_clause(sarl_sat_solver_t *solver, sarl_clause_t *c)
{
  // is the clause currently true
  if ( 
    sarl_bit_set_or(
      sarl_bit_set_and(c->pos, solver->pos),
      sarl_bit_set_and(c->neg, solver->neg)
    )
  ) {
    return true; // clause is true
  }

  // look for a unitary clause
  rp = sarl_bit_set_minus(c->pos, solver->neg);
  rn = sarl_bit_set_minus(c->neg, solver->pos);

  rp_count = sarl_bit_set_count(rp);
  rn_count = sarl_bit_set_count(rn);

  if ( rp_count + rn_count == 0 ) {
    return false; // contradiction
  }
  else if ( rp_count + rn_count == 1 ) {
    // clause is unitary
    if ( rp_count == 1 ) {
      var = sarl_bit_set_first(rp);
      sarl_bit_set_set(solver->pos, var);
      return sarl_sat_solver_update_clauses(solver);
    }
    if ( rn_count == 1 ) {
      var = sarl_bit_set_first(rn);
      sarl_bit_set_set(solver->neg, var);
      return sarl_sat_solver_update_var(solver, );
    }
  }

  return true;
};

bool sarl_sat_solver_update_var(
  sarl_sat_solver_t *solver, sarl_unsigned_int var)
{
  sarl_clause_t *c;

  SARL_ARRAY_FOR_EACH(sarl_clause_t, c, solver->clauses);
    if ( ! updateClause(solver, c) ) {
      return false;
    };
  SARL_ARRAY_END;

  return true;
};
  
bool sarl_sat_solver_search(sarl_sat_solver_t *solver)
{
  sarl_bit_set_t *curr_pos = sarl_bit_set_dup(solver->pos);
  sarl_bit_set_t *curr_neg = sarl_bit_set_dup(solver->neg);

  sarl_unsigned_int x =
    sarl_bit_set_first(
      sarl_bit_set_neg(solver->num_vars, sarl_bit_set_or(curr_pos, curr_neg))
    );

  if (x != 0) {
    sarl_bit_set_set(solver->pos, x);
    if ( 
      sarl_sat_solver_update_var(solver, x) == false || 
      sarl_sat_solver_search(solver) == false 
    ) {
      solver->pos = curr_pos;
      solver->neg = curr_neg;
      sarl_bit_set_set(solver->neg, x);
      return 
        sarl_sat_solver_update_var(solver, x) != false &&
        sarl_sat_solver_search(solver) != false
      );
    }
  }

  return true;
};
      
