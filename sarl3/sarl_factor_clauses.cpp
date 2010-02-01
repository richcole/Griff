#include "sarl_factor_clauses.h"
#include "sarl_bit_set.h"
#include "sarl_int_array.h"
#include "sarl_array.h"
#include "sarl_sat.h"
#include "sarl_power_set.h"
#include "sarl_power_map.h"

inline sarl_clause_factoring_t* sarl_clause_factoring_default_new()
{
  sarl_clause_factoring_t *cc = (sarl_clause_factoring_t *)
    GC_MALLOC(sizeof(sarl_clause_factoring_t));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);

  cc->valid_clauses = sarl_bit_set_default_new();
  cc->merge_map     = sarl_int_array_default_new();
  cc->solver        = 0;

  return cc;
};

// permute with the permutation u->v, v->u
void sarl_bit_set_permute(
  sarl_bit_set_t *bs, sarl_unsigned_int u, sarl_unsigned_int v)
{
  if ( sarl_bit_set_is_set(bs, u) ) {
    sarl_bit_set_clear(bs, u);
    sarl_bit_set_set(bs, v);
  }
};


sarl_unsigned_int sarl_clause_is_clone(
  sarl_clause_t *u_c, sarl_clause_t *v_c, 
  sarl_unsigned_int u, sarl_unsigned_int v)
{
  sarl_clause_t *clause = sarl_clause_dup(u_c);
  sarl_bit_set_permute(clause->pos, u, v);
  sarl_bit_set_permute(clause->neg, u, v);
  return 
    sarl_bit_set_is_equal(clause->pos, v_c->pos) &&
    sarl_bit_set_is_equal(clause->neg, v_c->neg);
};


sarl_clause_factoring_t* sarl_sat_solver_factor(sarl_sat_solver_t *solver)
{
  sarl_clause_factoring_t *factor = sarl_clause_factoring_default_new();
  
  factor->solver = solver;
  sarl_bit_set_populate(
    factor->valid_clauses, 
    sarl_array_count(factor->solver->clauses)
  );
  
  sarl_unsigned_int u, v, c_u, c_v, found;
  SARL_BIT_SET_FOR_EACH(v, solver->vars);
  SARL_BIT_SET_FOR_EACH(u, solver->vars);
  if ( u != v && sarl_int_array_get(factor->merge_map, u) == 0 ) {
    sarl_int_array_t *u_clause_lists = 
      sarl_inverted_lists_get(solver->inv_lists, u);
    sarl_int_array_t *v_clause_lists = 
      sarl_inverted_lists_get(solver->inv_lists, v);
    found = false;
    SARL_INT_ARRAY_FOR_EACH(c_v, v_clause_lists);
      if ( sarl_bit_set_is_set(factor->valid_clauses, c_v) ) {
        found = false;
        SARL_INT_ARRAY_FOR_EACH(c_u, u_clause_lists);
          if ( sarl_bit_set_is_set(factor->valid_clauses, c_u) ) {
            sarl_clause_t *clause_v = 
              (sarl_clause_t *)sarl_array_get(solver->clauses, c_v);
            sarl_clause_t *clause_u = 
              (sarl_clause_t *)sarl_array_get(solver->clauses, c_u);
            if (0) { // don't do it man
              fprintf(stdout, "Matching: \n  ");
              sarl_clause_print(clause_v, stdout);
              fprintf(stdout, "\n  ");
              sarl_clause_print(clause_u, stdout);
              fprintf(stdout, "\n");
            }
            if ( 
              sarl_clause_is_clone(clause_v, clause_u, v, u)
            ) {
              found = true;
              break;
            }
          }
        SARL_INT_ARRAY_END;
        if ( ! found ) break;
      }
    SARL_INT_ARRAY_END;
    if ( ! found ) continue;
    found = false;
    SARL_INT_ARRAY_FOR_EACH(c_u, u_clause_lists);
      if ( sarl_bit_set_is_set(factor->valid_clauses, c_u) ) {
        found = false;
        SARL_INT_ARRAY_FOR_EACH(c_v, v_clause_lists);
          if ( sarl_bit_set_is_set(factor->valid_clauses, c_v) ) {
            if ( 
              sarl_clause_is_clone(
                (sarl_clause_t *)sarl_array_get(solver->clauses, c_u), 
                (sarl_clause_t *)sarl_array_get(solver->clauses, c_v),
                u, v)
            ) {
              found = true;
              break;
            }
          }
        SARL_INT_ARRAY_END;
        if ( ! found ) break;
      }
    SARL_INT_ARRAY_END;
    if ( ! found ) continue;
    // otherwise merge u into v
    sarl_int_array_set(factor->merge_map, u, v);
    SARL_INT_ARRAY_FOR_EACH(c_u, u_clause_lists);
      sarl_bit_set_clear(factor->valid_clauses, c_u);
    SARL_INT_ARRAY_END;
  }
  SARL_BIT_SET_END;
  SARL_BIT_SET_END;

  return factor;
};

sarl_sat_solver_t *sarl_sat_solver_remove_duplicates(sarl_sat_solver_t *solver)
{
  sarl_power_set_t *pset = sarl_power_set_default_new();
  sarl_power_map_t *pset_map = sarl_power_map_default_new();
  sarl_power_map_t *clauses = sarl_power_map_default_new();
  sarl_clause_t    *clause;
  sarl_unsigned_int p, n;
  sarl_bit_set_t   *n_set;

  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
    p = sarl_power_set_insert(pset, clause->pos);
    n = sarl_power_set_insert(pset, clause->neg);
    sarl_power_map_set(pset_map, p, clause->pos);
    sarl_power_map_set(pset_map, n, clause->neg);
    sarl_power_map_insert(clauses, p, n);
  SARL_ARRAY_END;

  sarl_sat_solver_t *result = sarl_sat_solver_default_new();

  SARL_POWER_MAP_FOR_EACH(p, n_set, clauses);
    SARL_BIT_SET_FOR_EACH(n, n_set);
      sarl_sat_solver_add_clause(
        result,
        sarl_clause_new(
          sarl_power_map_get(pset_map, p), 
          sarl_power_map_get(pset_map, n)
        )
      );
    SARL_BIT_SET_END;
  SARL_POWER_MAP_END;

  return result;
};
