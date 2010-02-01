#ifndef SARL_CNF_H
#define SARL_CNF_H

#include "sarl_types.h"

enum LoadCNF_Result {
  LOAD_CNF_RESULT_FILE_ERROR,
  LOAD_CNF_RESULT_OK,
  LOAD_CNF_RESULT_UNSAT
};

LoadCNF_Result load_cnf(sarl_sat_solver_t *solver, char *filename);

#endif
