#include "sarl_variable_closure.h"
#include "sarl_sat.h"
#include "sarl_int_array.h"
#include <deque>

/* closes s with respect to coincidence in clauses in solver */

void sarl_variable_closure(
  sarl_bit_set_t *s, sarl_sat_solver_t *solver, sarl_unsigned_int variable)
{
  std::deque<sarl_unsigned_int> bound;
  bound.push_back(variable);

  sarl_unsigned_int v, new_var;
  sarl_clause_t *clause;
  sarl_bit_set_t *new_vars;

  while( bound.size() > 0 ) {
    v = bound.back();
    bound.pop_back();
    SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
      if ( sarl_bit_set_is_set(clause->neg, v) ) {
        new_vars = sarl_bit_set_minus(clause->neg, s);
        sarl_bit_set_or_equals(new_vars, sarl_bit_set_minus(clause->pos, s));
        SARL_BIT_SET_FOR_EACH(new_var, new_vars);
          bound.push_back(new_var);
        SARL_BIT_SET_END;
	sarl_bit_set_or_equals(s, new_vars);
      }
    SARL_ARRAY_END;
  }
};

sarl_sat_solver_t* sarl_collect_relevant_clauses(
  sarl_bit_set_t *s, sarl_sat_solver_t *solver, 
  sarl_int_array_t *cont_map, sarl_int_array_t *inv_cont_map)
{
  sarl_clause_t *clause;
  sarl_sat_solver_t *result = sarl_sat_solver_default_new();
  sarl_unsigned_int i, count;

  count = 1;
  SARL_BIT_SET_FOR_EACH(i, s);
    sarl_int_array_set(cont_map, i, count);
    sarl_int_array_set(inv_cont_map, count, i);
    count += 1;
  SARL_BIT_SET_END;

  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
  if ( 
    ! sarl_bit_set_is_disjoint(clause->neg, s) 
  ) {
    sarl_clause_t *new_clause = sarl_clause_default_new();
    SARL_BIT_SET_FOR_EACH(i, clause->neg);
      sarl_bit_set_set(new_clause->neg, sarl_int_array_get(cont_map, i));
    SARL_BIT_SET_END;
    SARL_BIT_SET_FOR_EACH(i, clause->pos);
      sarl_bit_set_set(new_clause->pos, sarl_int_array_get(cont_map, i));
    SARL_BIT_SET_END;
    sarl_sat_solver_add_clause(result, new_clause);
  }
  SARL_ARRAY_END;

  return result;
};
