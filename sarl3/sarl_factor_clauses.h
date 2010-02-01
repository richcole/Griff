#ifndef SARL_FACTOR_CLAUSES_H
#define SARL_FACTOR_CLAUSES_H

#include "sarl_types.h"
#include "sarl_exceptions.h"

#include <string.h>
#include <gc/gc.h>

struct _sarl_clause_factoring_t {
  sarl_bit_set_t     *valid_clauses;
  sarl_int_array_t   *merge_map;
  sarl_sat_solver_t  *solver;
};


sarl_clause_factoring_t* sarl_clause_factoring_default_new();
sarl_clause_factoring_t* sarl_sat_solver_factor(sarl_sat_solver_t *solver);
sarl_sat_solver_t *sarl_sat_solver_remove_duplicates(sarl_sat_solver_t *solver);

#endif
