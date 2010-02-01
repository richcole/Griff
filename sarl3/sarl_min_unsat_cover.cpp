#include "sarl_sat.h"
#include "sarl_assert.h"
#include "sarl_ksubset.h"
#include "sarl_bit_set.h"
#include "sarl_contraction_map.h"
#include <deque>


// TODO: this can't be constant!
// returns true, if value is in arr
bool sarl_int_array_is_member (sarl_int_array_t* arr, 
			       sarl_unsigned_int value, 
			       sarl_unsigned_int size) {
  for (sarl_unsigned_int i=1; i<=size; i++) {
    if (sarl_int_array_get(arr, i) == value) {
      return true;
    }
  }
  return false;
}
    

// compute all min unSAT
int main()
{
  sarl_sat_solver_t *src_solver = sarl_sat_solver_default_new();
  sarl_sat_solver_t *solver     = sarl_sat_solver_default_new();

  sarl_sat_state_t *solver_state;
  sarl_sat_state_t *solver_state2;
  sarl_sat_state_t *solver_state_inner;
  sarl_sat_state_t *solver_state_inner2;

  sarl_clause_t* clause = 0;
  sarl_int_array_t* arr = 0;
  sarl_bit_set_t* newcover;
  unsigned int bound;               // bound for maximal number of FALSE y
  sarl_unsigned_int i, j, k;
  sarl_unsigned_int CLAUSE_START = 0;
  sarl_unsigned_int clause_counter = 0;
  //  std::deque<sarl_bit_set_t*> covers; // to save the covers
  sarl_array_t *blockingclauses = sarl_array_default_new(); // to save the blocking clauses

  // read cnf into dummy_solver
  //  FILE *inp = fopen("test.cnf", "r");
  sarl_test_assert(stdin != 0);
  sarl_test_assert( sarl_sat_solver_read_cnf(src_solver, stdin) );
  
  // populate the solver with the changed clauses
  sarl_int_array_t *inv_cont_map = 
    sarl_int_array_default_new();
  sarl_int_array_t *cont_map = 
    sarl_contraction_map(src_solver->vars, inv_cont_map);

  CLAUSE_START = sarl_int_array_count(inv_cont_map)+1;
  clause_counter = CLAUSE_START;
  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, src_solver->clauses);
    fprintf (stdout, "clause_counter = %u \n", clause_counter);
    // change the clause   (ADDYVARS)
    sarl_unsigned_int x;
    sarl_clause_t *new_clause = sarl_clause_default_new();
    SARL_BIT_SET_FOR_EACH(x, clause->pos);
      sarl_clause_add_literal(new_clause, sarl_int_array_get(cont_map, x));
    SARL_BIT_SET_END;
    SARL_BIT_SET_FOR_EACH(x, clause->neg);
      sarl_clause_add_literal(new_clause, -(sarl_signed_int)sarl_int_array_get(cont_map, x));
    SARL_BIT_SET_END;
    sarl_clause_add_literal(new_clause, -(sarl_signed_int)clause_counter);
    // add the clause
    if (!sarl_sat_solver_add_clause(solver, new_clause)) {
      fprintf (stderr, "sarl_min_unsat: could not add clause to solver\n");
    }
    // next clause
    clause_counter++;
  SARL_INT_ARRAY_END;

  bound = 1;
  // main loop
  solver_state2 = sarl_sat_save_state(solver);
  while (sarl_sat_solver_search(solver)) {
    sarl_sat_restore_state(solver, solver_state2);
    // save state
    solver_state = sarl_sat_save_state(solver);
    // AddAtMost
    fprintf (stdout, "bound = %u \n", bound);
    //    while (sarl_sat_solver_search(solver)) {
      sarl_sat_restore_state(solver, solver_state);
      // iterate over all k less or equal than bound
      for (k = 1; k <= bound; k++) {
	fprintf (stdout, "\tk = %d\n", k);
	// iterate over all subsets of {y0, y1, ..., yn-1} with k elements
	SARL_KSUBSET_FOR_EACH (arr, clause_counter-CLAUSE_START, k);
   	  // save solver state
	  solver_state_inner = sarl_sat_save_state(solver);
	  // set all y which are in arr to false and otherwise true
	  for (j = 0; j < clause_counter-CLAUSE_START; j++) {
	    clause = sarl_clause_default_new();
	    if (sarl_int_array_is_member (arr, j, k)) {
	      sarl_clause_add_literal(clause, -(sarl_signed_int)(j+CLAUSE_START));
	    } 
	    else {
	      sarl_clause_add_literal(clause, j+CLAUSE_START);
	    }
	    sarl_sat_solver_add_clause(solver, clause);
	    // debug
	    /*
	    printf ("\t\tAddAtMost = ");
	    sarl_clause_print(clause, stdout);
	    printf ("\n");
	    */
	  }
	  /*
	  printf ("next\n");
	  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
  	    sarl_clause_print (clause, stdout);
	    printf ("\n");
	    SARL_ARRAY_END;
	  */
	  solver_state_inner2 = sarl_sat_save_state(solver);
	  while (sarl_sat_solver_search(solver)) {
	    // is satisfiable --> get covers
	    // generate new bit set for y's
	    newcover = sarl_bit_set_default_new();
	    clause   = sarl_clause_default_new();
	    sarl_bit_set_clear_all(newcover);
	    // debug
	    /*
	    printf ("\t\tsolution = ");
	    sarl_sat_print_solution(solver, stdout);  
	    */
	    // get all y which are negative  TODO: loop just over the relevant
	    SARL_BIT_SET_FOR_EACH(i, solver->vars);
	      if (i >= CLAUSE_START && sarl_bit_set_is_set(solver->neg, i)) {
		// y_i is false --> add to cover
		sarl_bit_set_set (newcover, i);
		// generate clause for blocking
		sarl_clause_add_literal(clause, i);
	      }
	    SARL_BIT_SET_END;
	    // debug
	    fprintf (stdout, "\t\ty = ");
	    SARL_BIT_SET_FOR_EACH(i, newcover);
	      fprintf (stdout, "%u ", (i-CLAUSE_START)+1);
	    SARL_BIT_SET_END;
	    fprintf (stdout, "\n");
	    fflush(stdout);
	    // printf (", clause = ");
	    // add cover
	    // covers.push_back (newcover);
	    // debug: print clause
	    //sarl_clause_print (clause, stdout);
	    // add blocking
	    sarl_sat_restore_state(solver, solver_state_inner2);
	    sarl_sat_solver_add_clause(solver, clause);
	    sarl_array_push(blockingclauses, clause);
	    solver_state_inner2 = sarl_sat_save_state(solver);
	  }
	  // restore state
	  sarl_sat_restore_state(solver, solver_state_inner);
	  // add blocking clauses
	  while (sarl_array_count(blockingclauses) > 0) {
	    sarl_sat_solver_add_clause(solver, 
              (sarl_clause_t *)sarl_array_pop(blockingclauses));
	  }
	SARL_KSUBSET_END;
      } // for k in 0..bound loop
      solver_state = sarl_sat_save_state(solver);
      //    }
    bound++;  
    solver_state2 = sarl_sat_save_state(solver);
  }
  return 0;
}
