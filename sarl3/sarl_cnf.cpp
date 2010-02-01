#include "sarl_cnf.h"
#include "sarl_sat.h"
#include <errno.h>
#include <stdio.h>

LoadCNF_Result load_cnf(sarl_sat_solver_t *solver, char *filename)
{
  FILE *inp = fopen(filename, "r");

  if ( ! inp ) {
    fprintf(stderr, "Error: unable top open file: '%s'\n", filename);
    fprintf(stderr, "  %s\n", strerror(errno));
    return LOAD_CNF_RESULT_FILE_ERROR;
  }

  bool result = sarl_sat_solver_read_cnf(solver, inp);
  fclose(inp);

  return result ? LOAD_CNF_RESULT_OK : LOAD_CNF_RESULT_UNSAT;
};

