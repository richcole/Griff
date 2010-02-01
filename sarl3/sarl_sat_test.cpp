#include "sarl_sat.h"
#include "sarl_assert.h"

int main()
{
  sarl_sat_solver_t *solver = sarl_sat_solver_default_new();
  sarl_clause_t* c = 0;
  bool result;

  c = sarl_clause_default_new();
  sarl_clause_add_literal(c, 1);
  sarl_clause_add_literal(c, 2);

  sarl_test_assert( sarl_sat_solver_add_clause(solver, c) );
  
  c = sarl_clause_default_new();
  sarl_clause_add_literal(c, -2);
  sarl_clause_add_literal(c, -3);

  result = sarl_sat_solver_add_clause(solver, c);
  sarl_test_assert( result );

  result = sarl_sat_solver_search(solver);
  sarl_test_assert( result );

  sarl_test_assert( 
    sarl_bit_set_is_equal(
      sarl_bit_set_or(solver->pos, solver->neg),
      solver->vars)
  );
  sarl_test_assert( 
    sarl_bit_set_count(sarl_bit_set_and(solver->pos, solver->neg)) == 0 
  );

  sarl_sat_print_solution(solver, stdout);

  solver = sarl_sat_solver_default_new();
  c = sarl_clause_default_new();
  sarl_clause_add_literal(c, 1);
  sarl_clause_add_literal(c, -3);
  sarl_test_assert( sarl_sat_solver_add_clause(solver, c) );

  c = sarl_clause_default_new();
  sarl_clause_add_literal(c, -1);
  sarl_clause_add_literal(c, -2);
  sarl_test_assert( sarl_sat_solver_add_clause(solver, c) );

  c = sarl_clause_default_new();
  sarl_clause_add_literal(c, -3);
  sarl_clause_add_literal(c, 2);
  sarl_test_assert( sarl_sat_solver_add_clause(solver, c) );

  c = sarl_clause_default_new();
  sarl_clause_add_literal(c, 3);
  sarl_test_assert( ! sarl_sat_solver_add_clause(solver, c) );

  sarl_sat_print_solution(solver, stdout);

  solver = sarl_sat_solver_default_new();

  FILE *inp = fopen("test.cnf", "r");
  sarl_test_assert(inp != 0);

  sarl_test_assert( sarl_sat_solver_read_cnf(solver, inp) );
  sarl_test_assert( sarl_sat_solver_search(solver) );

  sarl_sat_print_solution(solver, stdout);
};
