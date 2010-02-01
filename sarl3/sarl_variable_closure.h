#ifndef SARL_VARIABLE_CLOSURE_H
#define SARL_VARIABLE_CLOSURE_H

#include "sarl_types.h"

void sarl_variable_closure(
  sarl_bit_set_t *s, sarl_sat_solver_t *solver, sarl_unsigned_int variable);

sarl_sat_solver_t* sarl_collect_relevant_clauses(
  sarl_bit_set_t *s, sarl_sat_solver_t *solver, sarl_int_array_t *cont_map, sarl_int_array_t *inv_cont_map);

#endif
