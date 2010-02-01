#include "sarl_prune_clauses.h"
#include "sarl_sat.h"
#include "sarl_power_map.h"
#include "sarl_int_array.h"

static sarl_unsigned_int static_dfs_count = 0;

sarl_power_map_t *sarl_sat_dep_graph(sarl_sat_solver_t *solver)
{
  sarl_power_map_t *graph = sarl_power_map_default_new();
  sarl_unsigned_int var;
  sarl_int_array_t *clause_list;
  sarl_unsigned_int clause_index;
  sarl_unsigned_int dep_var;

  SARL_BIT_SET_FOR_EACH(var, solver->vars);
    clause_list = sarl_inverted_lists_get(solver->inv_lists, var);
    SARL_INT_ARRAY_FOR_EACH(clause_index, clause_list);
      sarl_clause_t *clause = 
        (sarl_clause_t*)sarl_array_get(solver->clauses, clause_index);
      if ( 
        sarl_bit_set_is_set(clause->neg, var) && 
        sarl_bit_set_count(clause->neg) == 1 
      ) {
        SARL_BIT_SET_FOR_EACH(dep_var, clause->pos);
          sarl_power_map_insert(graph, dep_var, var);
        SARL_BIT_SET_END;
      }
    SARL_INT_ARRAY_END;
  SARL_BIT_SET_END;

  return graph;
};

void sarl_transitive_closure(
  sarl_power_map_t *graph, 
  sarl_power_map_t *upclosure, 
  sarl_power_map_t *downclosure
)
{
  sarl_unsigned_int x, y, u, d;
  sarl_bit_set_t* upper_covers;
  sarl_bit_set_t* upset;
  sarl_bit_set_t* downset;

  SARL_POWER_MAP_FOR_EACH(x, upper_covers, graph);
  SARL_BIT_SET_FOR_EACH(y, upper_covers);
    upset = sarl_power_map_get(upclosure, y);
    downset = sarl_power_map_get(downclosure, x);
    if ( upset && downset ) {
      SARL_BIT_SET_FOR_EACH(u, upset);
      SARL_BIT_SET_FOR_EACH(d, downset);
        sarl_power_map_insert(upclosure, d, u);
        sarl_power_map_insert(downclosure, u, d);
      SARL_BIT_SET_END;
      SARL_BIT_SET_END;
    }

    if ( downset ) {
      SARL_BIT_SET_FOR_EACH(d, downset);
        sarl_power_map_insert(upclosure, d, y);
        sarl_power_map_insert(downclosure, y, d);
      SARL_BIT_SET_END;
    }
    if ( upset ) {
      SARL_BIT_SET_FOR_EACH(u, upset);
        sarl_power_map_insert(upclosure, x, u);
        sarl_power_map_insert(downclosure, y, d);
        SARL_BIT_SET_END;
    }
    sarl_power_map_insert(upclosure, x, y);
    sarl_power_map_insert(downclosure, y, x);
  SARL_BIT_SET_END;
  SARL_POWER_MAP_END;
};

sarl_bit_set_t *sarl_sat_find_conflicts(
  sarl_sat_solver_t *solver, 
  sarl_power_map_t *upclosure, 
  sarl_power_map_t *downclosure, 
  sarl_unsigned_int target)
{
  sarl_bit_set_t *conflict_vars = sarl_bit_set_default_new();
  sarl_bit_set_t *target_downset = sarl_power_map_get(downclosure, target);
  sarl_clause_t *clause;

  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
  if ( sarl_bit_set_count(clause->neg) == 2 ) {
    if ( sarl_bit_set_is_subset(clause->neg, target_downset) ) {
      sarl_bit_set_or_equals(conflict_vars, clause->neg);
    }
  }
  SARL_ARRAY_END;

  return conflict_vars;
};

sarl_bit_set_t* sarl_power_map_union_closure(
  sarl_bit_set_t *bset, 
  sarl_power_map_t *pmap
)
{
  sarl_bit_set_t *closure = sarl_bit_set_default_new();
  sarl_bit_set_t *image;
  sarl_unsigned_int x;

  SARL_BIT_SET_FOR_EACH(x, bset);
    image = sarl_power_map_get(pmap, x);
    if ( image ) {
      sarl_bit_set_or_equals(closure, image);
    }
  SARL_BIT_SET_END;

  return closure;
};

sarl_sat_solver_t* sarl_select_relevent_clauses(
  sarl_sat_solver_t *solver, sarl_bit_set_t *relevent_vars
)
{
  sarl_sat_solver_t *new_solver = sarl_sat_solver_default_new();
  sarl_clause_t *clause;

  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
  if ( 
    sarl_bit_set_is_subseteq(clause->pos, relevent_vars) && 
    sarl_bit_set_is_subseteq(clause->neg, relevent_vars) 
  ) {
    sarl_sat_solver_add_clause(new_solver, clause);
  }
  SARL_ARRAY_END;

  return new_solver;
};

void sarl_sat_dfs_upclosure(
  sarl_sat_solver_t *solver, sarl_unsigned_int var, sarl_bit_set_t *visited, 
  sarl_bit_set_t *contra_vars, sarl_power_map_t *upclosure
)
{
  sarl_int_array_t *relevent_clauses = sarl_inverted_lists_get(solver->inv_lists, var);
  sarl_unsigned_int x;
  sarl_clause_t *clause;

  ++static_dfs_count;

  // store the up closure
  SARL_BIT_SET_FOR_EACH(x, visited);
    sarl_power_map_insert(upclosure, var, x);
  SARL_BIT_SET_END;

  // recurse through the children avoiding cycles
  sarl_unsigned_int clause_index;
  SARL_INT_ARRAY_FOR_EACH(clause_index, relevent_clauses);
    clause = (sarl_clause_t *)sarl_array_get(solver->clauses, clause_index);
    if ( sarl_bit_set_count(clause->neg) > 1 ) { 
      // this is a contradiction, so store the contradicting clauses 
      sarl_bit_set_or_equals(contra_vars, clause->neg);
    }
    else {
      // this is an implication
      if ( sarl_bit_set_is_set(clause->neg, var) ) {
        // this an involving var
        SARL_BIT_SET_FOR_EACH(x, clause->pos);
          if ( ! sarl_bit_set_is_set(visited, x) ) {
            sarl_bit_set_set(visited, x);
            sarl_sat_dfs_upclosure(solver, x, visited, contra_vars, upclosure);
            sarl_bit_set_clear(visited, x);
          }
        SARL_BIT_SET_END;
      }
    }
  SARL_INT_ARRAY_END;
};

sarl_sat_solver_t* sarl_prune_clauses(sarl_sat_solver_t *solver, sarl_unsigned_int target)
{
  sarl_bit_set_t *visited = sarl_bit_set_default_new();
  sarl_power_map_t *upclosure = sarl_power_map_default_new();
  sarl_bit_set_t *contra_vars = sarl_bit_set_default_new();

  sarl_sat_dfs_upclosure(solver, target, visited, contra_vars, upclosure);
  
  // take the union of the upclosure of the contra vars
  sarl_bit_set_t *relevent_vars = sarl_bit_set_default_new();
  sarl_unsigned_int x;
  SARL_BIT_SET_FOR_EACH(x, contra_vars);
    sarl_bit_set_t *closure = sarl_power_map_get(upclosure, x);
    if ( closure ) {
      sarl_bit_set_or_equals(relevent_vars, closure);
    }
  SARL_BIT_SET_END;
  sarl_bit_set_set(relevent_vars, target);
  
  // now select only clauses which have a relevent variable in them
  sarl_sat_solver_t *new_solver = sarl_sat_solver_default_new();
  sarl_clause_t *clause;
  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
    // is the clause an implication implying a relevent clauses, if so include it
    if ( sarl_bit_set_count(clause->neg) == 1 && 
        ! sarl_bit_set_is_disjoint(relevent_vars, clause->pos) ) 
    {
      sarl_sat_solver_add_clause(new_solver, clause);  
    }
    // is the clause a contradiction, if so then include it
    else if ( sarl_bit_set_count(clause->neg) > 1) {
      sarl_sat_solver_add_clause(new_solver, clause);
    }
    // if the clause is a unitary negation then include it
    else if ( sarl_bit_set_count(clause->neg) == 1 && sarl_bit_set_count(clause->pos) == 0) {
      sarl_unsigned_int negated_var = sarl_bit_set_first(clause->neg);
      if ( sarl_bit_set_is_set(relevent_vars, negated_var) ) {
        sarl_sat_solver_add_clause(new_solver, clause);
      }
    }
  SARL_ARRAY_END;

  return new_solver;
};
