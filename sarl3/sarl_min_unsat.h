#ifndef SARL_MIN_UNSAT_H
#define SARL_MIN_UNSAT_H

#include "sarl_types.h"

sarl_unsigned_int sarl_is_subset_sat(
  sarl_sat_solver_t *solver, sarl_bit_set_t *S);

sarl_power_set_t *
  sarl_min_unsat(sarl_bit_set_t *U, sarl_sat_solver_t *solver);

sarl_bit_set_t *sarl_min_unsat_grow(
  sarl_bit_set_t* S, sarl_bit_set_t *U, 
  sarl_sat_solver_t *solver);

sarl_power_set_t* sarl_min_unsat_hst(sarl_power_set_t *P);

#endif


