#include "sarl_sat.h"
#include "sarl_bit_set.h"
#include "sarl_array.h"
#include "sarl_exceptions.h"

#include <ctype.h>

sarl_sat_solver_t* sarl_sat_solver_default_new()
{
  sarl_sat_solver_t *solver = 
    (sarl_sat_solver_t *)GC_MALLOC(sizeof(sarl_sat_solver_t));
  if (solver) {
    solver->pos = sarl_bit_set_default_new();
    solver->neg = sarl_bit_set_default_new();
    solver->clauses = sarl_array_default_new();
    solver->vars = sarl_bit_set_default_new();
    solver->inv_lists = sarl_inverted_lists_default_new();
    solver->var_scores = sarl_int_array_default_new();
    solver->is_unsat = false;
  }

  return solver;
};

sarl_inverted_lists_t *sarl_inverted_lists_dup(sarl_inverted_lists_t *inv)
{
  sarl_inverted_lists_t* result = sarl_inverted_lists_default_new();
  sarl_unsigned_int list_index;
  sarl_int_array_t *inv_list;

  SARL_ARRAY_FOR_EACH_INDEX(list_index, sarl_int_array_t, inv_list, inv->lists);
    sarl_array_set(result->lists, list_index, sarl_int_array_dup(inv_list));
  SARL_ARRAY_END;

  return result;
};

sarl_sat_solver_t* sarl_sat_solver_dup(sarl_sat_solver_t *src_solver)
{
  sarl_sat_solver_t *solver = 
    (sarl_sat_solver_t *)GC_MALLOC(sizeof(sarl_sat_solver_t));
  if (solver) {
    solver->pos = sarl_bit_set_dup(src_solver->pos);
    solver->neg = sarl_bit_set_dup(src_solver->neg);
    solver->clauses = sarl_array_dup_ref(src_solver->clauses);
    solver->vars = sarl_bit_set_dup(src_solver->vars);
    solver->inv_lists = sarl_inverted_lists_dup(src_solver->inv_lists);
    solver->var_scores = sarl_int_array_dup(src_solver->var_scores);
  }

  return solver;
};

sarl_clause_t* sarl_clause_default_new()
{
  sarl_clause_t *clause = 
    (sarl_clause_t *)GC_MALLOC(sizeof(sarl_clause_t));
  if (clause) {
    clause->pos = sarl_bit_set_default_new();
    clause->neg = sarl_bit_set_default_new();
  }

  return clause;
};

bool  sarl_sat_solver_add_clause(sarl_sat_solver_t *solver, sarl_clause_t *c)
{
  if ( ! sarl_bit_set_is_empty(sarl_bit_set_and(c->pos, c->neg)) ) {
    // tautology, ignore it
    return true;
  }

  // insert the vars into our collection of vars
  sarl_bit_set_or_equals(solver->vars, c->pos);
  sarl_bit_set_or_equals(solver->vars, c->neg);

  // add the clause
  sarl_array_push(solver->clauses, c);
  sarl_unsigned_int clause_index = sarl_array_count(solver->clauses);

  // add the variables of the clause to the inverted list
  sarl_unsigned_int i;
  SARL_BIT_SET_FOR_EACH(i, c->pos);
    sarl_inverted_lists_insert(solver->inv_lists, i, clause_index);
  SARL_BIT_SET_END;
  SARL_BIT_SET_FOR_EACH(i, c->neg);
    sarl_inverted_lists_insert(solver->inv_lists, i, clause_index);
  SARL_BIT_SET_END;
  
  // update the solver state
  return sarl_sat_solver_update_clause(solver, c, clause_index);
};

bool  sarl_sat_solver_update_clause(
  sarl_sat_solver_t *solver, sarl_clause_t *c, sarl_unsigned_int clause_index)
{
  // is the clause currently true
  if ( 
    sarl_bit_set_count(
      sarl_bit_set_or(
        sarl_bit_set_and(c->pos, solver->pos),
        sarl_bit_set_and(c->neg, solver->neg)
      )
    ) != 0
  ) {
    return true; // clause is true
  }

  // look for a unitary clause
  sarl_bit_set_t *rp = sarl_bit_set_minus(c->pos, solver->neg);
  sarl_bit_set_t *rn = sarl_bit_set_minus(c->neg, solver->pos);

  sarl_unsigned_int rp_count = sarl_bit_set_count(rp);
  sarl_unsigned_int rn_count = sarl_bit_set_count(rn);
  sarl_unsigned_int var = 0;

  if ( rp_count + rn_count == 0 ) {
    solver->is_unsat = true;
    return false; // contradiction
  }
  else if ( rp_count + rn_count == 1 ) {
    // clause is unitary
    if ( rp_count == 1 ) {
      var = sarl_bit_set_first(rp);
      sarl_bit_set_set(solver->pos, var);
      return sarl_sat_solver_update_var(solver, var);
    }
    if ( rn_count == 1 ) {
      var = sarl_bit_set_first(rn);
      sarl_bit_set_set(solver->neg, var);
      return sarl_sat_solver_update_var(solver, var);
    }
  }

  return true;
};

bool sarl_sat_solver_update_var(
  sarl_sat_solver_t *solver, sarl_unsigned_int var)
{
  sarl_clause_t    *clause;
  sarl_int_array_t *relevent_clauses = 
    sarl_inverted_lists_get(solver->inv_lists, var);
  sarl_unsigned_int clause_index;

  if ( relevent_clauses ) {
    SARL_INT_ARRAY_FOR_EACH(clause_index, relevent_clauses);
      clause = (sarl_clause_t*)sarl_array_get(solver->clauses, clause_index);
      if ( clause == 0 ) {
        fprintf(stderr, "ERROR: invalid clause in inverted list! (HELP)\n");
      } 
      else 
        if ( ! sarl_sat_solver_update_clause(solver, clause, clause_index) ) {
	  solver->is_unsat = true;
          return false;
        };
    SARL_INT_ARRAY_END;
  }

  return true;
};
  
bool sarl_sat_solver_search(sarl_sat_solver_t *solver)
{
  // check for unsat state
  if ( solver->is_unsat ) {
    return false;
  }

  sarl_bit_set_t *curr_pos = sarl_bit_set_dup(solver->pos);
  sarl_bit_set_t *curr_neg = sarl_bit_set_dup(solver->neg);

  // find the variable with the highest score
  sarl_unsigned_int v = 0, x = 0;
  sarl_unsigned_int x_score = 0;
  sarl_unsigned_int v_score = 0;

  SARL_BIT_SET_FOR_EACH(v, solver->vars);
    if ( 
      (! sarl_bit_set_is_set(curr_pos, v)) && 
      (! sarl_bit_set_is_set(curr_neg, v))
    ) {
      v_score = sarl_int_array_get(solver->var_scores, v);
      if ( x == 0 || v_score > x_score ) {
        x = v;
        x_score = v_score;
      }
    }
  SARL_BIT_SET_END;

  /* old code before variable ordering came in :) 
    sarl_bit_set_first(
      sarl_bit_set_neg(solver->num_vars, sarl_bit_set_or(curr_pos, curr_neg))
    );
  */
  sarl_unsigned_int had_to_search = 0;

  if (x != 0) {
    sarl_bit_set_set(solver->pos, x);
    if ( 
      sarl_sat_solver_update_var(solver, x) == false || 
      ((had_to_search = 1), sarl_sat_solver_search(solver) == false)
    ) {
      if ( ! had_to_search ) sarl_int_array_incr(solver->var_scores, x);
      solver->pos = curr_pos;
      solver->neg = curr_neg;
      solver->is_unsat = false;
      sarl_bit_set_set(solver->neg, x);
      if ( sarl_sat_solver_update_var(solver, x) ) {
        return sarl_sat_solver_search(solver);
      }
      else {
        sarl_int_array_incr(solver->var_scores, x);
	solver->is_unsat = true;
        return false;
      };
    }
  }

  return true;
};
  
bool sarl_sat_solver_read_cnf(sarl_sat_solver_t *solver, FILE *inp)
{
  sarl_return_zero_error_if_zero_2(solver, inp);

  int c;
  sarl_signed_int literal_value = 0;
  sarl_signed_int literal_sign  = 1;
  sarl_clause_t *clause = sarl_clause_default_new();
  
  bool have_literal = false;
  bool have_clause = false;
  bool is_unsat = false;

  while ( (c = fgetc(inp)) != EOF ) {
    if ( c == 'p' || c == 'c' ) {
      while ( c != '\n' && c != '\r' && c != '\f' ) {
        c = fgetc(inp);
      }
    }
    if ( c == '-' ) {
      literal_sign = -1;
    }
    else if ( isdigit(c) ) {
      literal_value *= 10; literal_value += c - '0';
      have_literal = true;
    }
    else if ( c == '\n' ) {
      if ( have_literal ) {
        if (literal_value != 0) 
          sarl_clause_add_literal(clause, literal_value * literal_sign);
        literal_value = 0; literal_sign = 1;
        have_literal = false;
        have_clause = true;
      }
      if ( have_clause ) {
        if ( ! sarl_sat_solver_add_clause(solver, clause) ) {
          is_unsat = true;
        }
        clause = sarl_clause_default_new();
        have_clause = false;
      }
    }
    else if ( isspace(c) ) {
      if ( have_literal ) {
        if (literal_value != 0) 
          sarl_clause_add_literal(clause, literal_value * literal_sign);
        literal_value = 0; literal_sign = 1;
        have_literal = false;
        have_clause = true;
      }
    }
  }

  if ( have_literal && literal_value != 0) {
    sarl_clause_add_literal(clause, literal_value * literal_sign);
  }
  if ( have_clause ) {
    if ( ! sarl_sat_solver_add_clause(solver, clause) ) {
      is_unsat = true;
    };
  }
  return ! is_unsat;
};

void sarl_pair_print(
  sarl_bit_set_t *vars, sarl_bit_set_t *pos, sarl_bit_set_t *neg, 
  FILE *outp)
{
  sarl_unsigned_int i;
  SARL_BIT_SET_FOR_EACH(i, vars);
    if ( sarl_bit_set_is_set(pos, i) ) {
      fprintf(outp, "%d ", i);
    }
    else if ( sarl_bit_set_is_set(neg, i) ) {
      fprintf(outp, "%d ", -i);
    }
  SARL_BIT_SET_END;
};

void sarl_clause_print(sarl_clause_t *clause, FILE *outp)
{
  sarl_bit_set_t *vars = sarl_bit_set_or(clause->pos, clause->neg);
  sarl_pair_print(vars, clause->pos, clause->neg, outp);
};
  

void sarl_sat_print_solution(sarl_sat_solver_t *solver, FILE *outp)
{
  sarl_unsigned_int i;
  SARL_BIT_SET_FOR_EACH(i, solver->vars);
    if ( sarl_bit_set_is_set(solver->pos, i) ) {
      fprintf(outp, "%d ", i);
    }
    else if ( sarl_bit_set_is_set(solver->neg, i) ) {
      fprintf(outp, "%d ", -i);
    }
    else {
      fprintf(outp, "\n Error: %d is UNDEEFINED \n", i);
    }
  SARL_BIT_SET_END;
  fprintf(outp, "\n");
};

sarl_unsigned_int sarl_num_allocated_states = 0;

void sarl_finalize_state(GC_PTR ptr, GC_PTR data)
{
  --sarl_num_allocated_states;
};

