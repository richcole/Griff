#include "sarl_factor_clauses.h"
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

  {
    sarl_signed_int c1[] = { -1,  2,  3,  0 };
    sarl_signed_int c2[] = { -1, -4,  0     };
    sarl_signed_int c3[] = { -1, -5,  0     };
    sarl_signed_int c4[] = { -2,  4,  0     };
    sarl_signed_int c5[] = { -2,  5,  0     };
    sarl_signed_int c6[] = { -3,  4,  0     };
    sarl_signed_int c7[] = { -3,  5,  0     };

    sarl_sat_solver_t* solver = sarl_sat_solver_default_new();
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c1));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c2));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c3));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c4));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c5));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c6));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c7));

    sarl_clause_factoring_t* factor = sarl_sat_solver_factor(solver);

    fprintf(stdout, "clauses:\n");
    sarl_unsigned_int x, y;
    SARL_BIT_SET_FOR_EACH(x, factor->valid_clauses);
    fprintf(stdout, "  %u: ", x);
    sarl_clause_print(
      (sarl_clause_t *)sarl_array_get(factor->solver->clauses, x), stdout);
    fprintf(stdout, "\n");
    SARL_BIT_SET_END;
  
    fprintf(stdout, "merge mapping:\n");
    SARL_INT_ARRAY_FOR_EACH_INDEX(x, y, factor->merge_map);
    fprintf(stdout, "  %u -> %u\n", x, y);
    SARL_INT_ARRAY_END;
  }

  {
    sarl_signed_int c1[] = { -1, 2, 0 };
    sarl_signed_int c2[] = { -1, 3, 0 };
    sarl_signed_int c3[] = { -2, 4, 0 };
    sarl_signed_int c4[] = { -3, 4, 0 };

    sarl_sat_solver_t* solver = sarl_sat_solver_default_new();
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c1));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c2));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c3));
    sarl_sat_solver_add_clause(solver, sarl_clause_new_from_ints(c4));

    sarl_clause_factoring_t* factor = sarl_sat_solver_factor(solver);

    fprintf(stdout, "clauses:\n");
    sarl_unsigned_int x, y;
    SARL_BIT_SET_FOR_EACH(x, factor->valid_clauses);
    fprintf(stdout, "  %u: ", x);
    sarl_clause_print(
      (sarl_clause_t *)sarl_array_get(factor->solver->clauses, x), stdout);
    fprintf(stdout, "\n");
    SARL_BIT_SET_END;
  
    fprintf(stdout, "merge mapping:\n");
    SARL_INT_ARRAY_FOR_EACH_INDEX(x, y, factor->merge_map);
    fprintf(stdout, "  %u -> %u\n", x, y);
    SARL_INT_ARRAY_END;
  }
};

