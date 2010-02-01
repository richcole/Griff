#include "sarl_sat.h"

#include <errno.h>

int main(int argc, char **argv)
{
  if ( argc != 2 ) {
    fprintf(stderr, "Usage: sarl_sat <input.cnf>\n");
    exit(-1);
  }

  FILE *inp = fopen(argv[1], "r");

  if ( ! inp ) {
    fprintf(stderr, "Error: unable top open file: '%s'\n", argv[1]);
    fprintf(stderr, "  %s\n", strerror(errno));
    exit(-1);
  }

  sarl_sat_solver_t *solver = sarl_sat_solver_default_new();
  bool result = true;

  result = sarl_sat_solver_read_cnf(solver, inp);

  if ( ! result ) {
    fprintf(stdout, "UNSATISFIABLE: trivial\n");
    exit(0);
  }

  result = sarl_sat_solver_search(solver);

  if ( ! result ) {
    fprintf(stdout, "UNSATIFIABLE");
    exit(0);
  }

  sarl_sat_print_solution(solver, stdout);
};
