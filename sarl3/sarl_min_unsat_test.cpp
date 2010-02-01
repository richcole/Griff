#include "sarl_min_unsat.h"
#include "sarl_sat.h"
#include "sarl_assert.h"
#include "sarl_power_set.h"

void sarl_print_hst(sarl_power_set_t *hst)
{
  fprintf(stdout, "HST: ");
  sarl_power_set_print_to_file(hst, stdout);
  fprintf(stdout, "\n");
};

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_unsigned_int a1s[] = {1,3,4,0};
  sarl_unsigned_int a2s[] = {1,2,4,0};
  sarl_unsigned_int a3s[] = {1,2,3,0};

  sarl_power_set_t *ps = sarl_power_set_default_new();
  
  sarl_power_set_insert(ps, sarl_bit_set_new_from_array(a1s));
  sarl_power_set_insert(ps, sarl_bit_set_new_from_array(a2s));

  sarl_power_set_t *rs = sarl_min_unsat_hst(ps);

  sarl_test_assert( sarl_power_set_count(rs) == 3 );
  sarl_print_hst(rs);

  sarl_power_set_insert(ps, sarl_bit_set_new_from_array(a3s));
  rs = sarl_min_unsat_hst(ps);

  sarl_test_assert( sarl_power_set_count(rs) == 4 );
  sarl_print_hst(rs);

  sarl_signed_int c1[] = { -1, 2, 4, 0 };
  sarl_signed_int c2[] = { -1, 6, 0 };
  sarl_signed_int c3[] = { -1, -5, 0 };
  sarl_signed_int c4[] = { -1, -3, 0 };
  sarl_signed_int c5[] = { -2, 3, 0 };
  sarl_signed_int c6[] = { -4, 5, 0 };
  sarl_signed_int c7[] = { 1, 0 };

  sarl_sat_solver_t* solver = sarl_sat_solver_default_new();
  sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c1));
  sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c2));
  sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c3));
  sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c4));
  sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c5));
  sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c6));
  sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c7));

  sarl_unsigned_int i;
  sarl_bit_set_t *U = sarl_bit_set_default_new();
  for(i=1;i<=7;++i) sarl_bit_set_set(U, i);
  sarl_power_set_t* min_unsat = sarl_min_unsat(U, solver);

  fprintf(stdout, "Min Unsat: ");
  sarl_power_set_print_to_file(min_unsat, stdout);
  fprintf(stdout, "\n");
  
  fprintf(stdout, "Finished.\n");
};

