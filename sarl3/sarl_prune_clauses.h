#ifndef SARL_PRUNE_CLAUSES_H
#define SARL_PRUNE_CLAUSED_H

#include "sarl_types.h"

sarl_power_map_t *sarl_sat_dep_graph(
  sarl_sat_solver_t *solver
);

void sarl_transitive_closure(
  sarl_power_map_t *graph, 
  sarl_power_map_t *upclosure, sarl_power_map_t *downclosure
);


sarl_sat_solver_t* sarl_prune_clauses(
  sarl_sat_solver_t *solver, sarl_unsigned_int target
);

sarl_bit_set_t *sarl_sat_find_conflicts(
  sarl_sat_solver_t *solver, 
  sarl_power_map_t *upclosure, 
  sarl_power_map_t *downclosure, 
  sarl_unsigned_int target
);

sarl_bit_set_t* sarl_power_map_union_closure(
  sarl_bit_set_t *bset, 
  sarl_power_map_t *pmap
);

sarl_sat_solver_t* sarl_select_relevent_clauses(
  sarl_sat_solver_t *solver, sarl_bit_set_t *relevent_vars
);


#endif
