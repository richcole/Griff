#ifndef SARL_SAT_H
#define SARL_SAT_H

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_bit_set.h"
#include "sarl_array.h"
#include "sarl_int_array.h"

// #define SARL_TRACE_STATE_MALLOC(x) x
#define SARL_TRACE_STATE_MALLOC(x)

#include <gc/gc.h>

typedef struct _sarl_clause_t      sarl_clause_t;
typedef struct _sarl_sat_state_t  sarl_sat_state_t;
typedef struct _sarl_sat_solver_t sarl_sat_solver_t;
typedef struct _sarl_inverted_lists_t sarl_inverted_lists_t;

struct _sarl_clause_t {
  sarl_bit_set_t *pos, *neg;
};

struct _sarl_sat_state_t {
  sarl_bit_set_t *pos;
  sarl_bit_set_t *neg;
  sarl_bit_set_t *vars;
  sarl_unsigned_int num_clauses;
  sarl_int_array_t *inv_list_lengths;
  sarl_unsigned_int is_unsat;
};

struct _sarl_inverted_lists_t {
  sarl_array_t *lists;  // map from indexes to inverted lists
};

struct _sarl_sat_solver_t {
  sarl_bit_set_t          *pos, *neg;
  sarl_array_t            *clauses;
  sarl_bit_set_t          *vars;
  sarl_inverted_lists_t   *inv_lists;
  sarl_int_array_t        *var_scores;
  sarl_unsigned_int       is_unsat;
};

/* both pos and neg are copied */

inline sarl_clause_t* sarl_clause_new(
  sarl_bit_set_t *pos, sarl_bit_set_t *neg
)
{
  sarl_clause_t *clause = 
    (sarl_clause_t *)GC_MALLOC(sizeof(sarl_clause_t));
  if (clause) {
    clause->pos = sarl_bit_set_dup(pos);
    clause->neg = sarl_bit_set_dup(neg);
  }

  return clause;
};


sarl_clause_t* sarl_clause_default_new();

sarl_sat_solver_t* sarl_sat_solver_default_new();

bool  sarl_sat_solver_add_clause(sarl_sat_solver_t *solver, sarl_clause_t *c);

bool  sarl_sat_solver_update_clause(
  sarl_sat_solver_t *solver, sarl_clause_t *c, sarl_unsigned_int clause_index);

bool sarl_sat_solver_update_var(
  sarl_sat_solver_t *solver, sarl_unsigned_int var);

bool sarl_sat_solver_search(sarl_sat_solver_t *solver);

sarl_sat_solver_t* sarl_sat_solver_dup(sarl_sat_solver_t *sat);

inline void sarl_clause_add_literal(sarl_clause_t *clause, sarl_signed_int lit)
{
  if ( lit == 0 ) {
    sarl_invalid_index_error;
  }
  else if ( lit < 0 ) {
    sarl_bit_set_set(clause->neg, -lit);
  }
  else {
    sarl_bit_set_set(clause->pos, lit);
  }
};

inline sarl_clause_t* sarl_clause_new_from_ints(sarl_signed_int *ints)
{
  sarl_clause_t *clause = sarl_clause_default_new();
  while(*ints) {
    sarl_clause_add_literal(clause, *ints);
    ++ints;
  }

  return clause;
};

bool sarl_sat_solver_read_cnf(sarl_sat_solver_t *solve, FILE *inp);

void sarl_sat_print_solution(sarl_sat_solver_t *solver, FILE *outp);
void sarl_clause_print_solution(sarl_clause_t *clause, FILE *outp);
void sarl_clause_print(sarl_clause_t *clause, FILE *outp);

void sarl_pair_print(
  sarl_bit_set_t* vars,
  sarl_bit_set_t *pos, sarl_bit_set_t *neg, 
  FILE *outp
);

inline sarl_clause_t* sarl_clause_dup(sarl_clause_t *clause)
{
  return sarl_clause_new(sarl_bit_set_dup(clause->pos), sarl_bit_set_dup(clause->neg));
};

extern sarl_unsigned_int sarl_num_allocated_states;
void sarl_finalize_state(GC_PTR ptr, GC_PTR data);


inline sarl_sat_state_t* sarl_sat_state_new(sarl_sat_solver_t *solver)
{
  sarl_sat_state_t *state = 
    (sarl_sat_state_t *)GC_MALLOC(sizeof(sarl_sat_state_t));
  SARL_TRACE_STATE_MALLOC(
    ++sarl_num_allocated_states;
    GC_REGISTER_FINALIZER_NO_ORDER(state, sarl_finalize_state, 0, 0, 0);
  );
  sarl_unsigned_int list_index;
  sarl_int_array_t* inv_list;

  if (state) {
    state->pos = sarl_bit_set_dup(solver->pos);
    state->neg = sarl_bit_set_dup(solver->neg);
    state->vars = sarl_bit_set_dup(solver->vars);
    state->num_clauses = sarl_array_count(solver->clauses);
    state->is_unsat = solver->is_unsat;

    state->inv_list_lengths = sarl_int_array_default_new();
    SARL_ARRAY_FOR_EACH_INDEX(
      list_index, sarl_int_array_t, inv_list, solver->inv_lists->lists
    );
      sarl_int_array_set(
        state->inv_list_lengths, list_index, sarl_int_array_count(inv_list)
      );
    SARL_ARRAY_END;
  }

  return state;
};
  

inline sarl_sat_state_t* sarl_sat_save_state(sarl_sat_solver_t *solver)
{
  return sarl_sat_state_new(solver);
};

inline void sarl_sat_restore_state(
  sarl_sat_solver_t *solver, sarl_sat_state_t *state)
{
  sarl_bit_set_assign_equals(solver->pos, state->pos);
  sarl_bit_set_assign_equals(solver->neg, state->neg);
  sarl_bit_set_assign_equals(solver->vars, state->vars);
  solver->is_unsat = state->is_unsat;
  sarl_array_truncate(solver->clauses, state->num_clauses);

  sarl_unsigned_int v;
  SARL_BIT_SET_FOR_EACH(v, solver->vars);
    sarl_int_array_t *list = 
      (sarl_int_array_t *)sarl_array_get(solver->inv_lists->lists, v);
    if ( list ) {
      sarl_int_array_truncate(list, sarl_int_array_get(state->inv_list_lengths, v));
    }
  SARL_BIT_SET_END;
};

inline sarl_inverted_lists_t* sarl_inverted_lists_default_new()
{
  sarl_inverted_lists_t *cc = (sarl_inverted_lists_t *)
    GC_MALLOC(sizeof(sarl_inverted_lists_t));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);

  cc->lists = sarl_array_default_new();

  return cc;
};

inline void 
  sarl_inverted_lists_insert(sarl_inverted_lists_t *inv, 
    sarl_unsigned_int index, sarl_unsigned_int value)
{
  sarl_int_array_t *list = 
    (sarl_int_array_t *)sarl_array_get(inv->lists, index);

  if ( list == 0 ) {
    sarl_array_set(inv->lists, index, list = sarl_int_array_default_new());
  }
  sarl_int_array_push(list, value);
};

inline sarl_int_array_t* sarl_inverted_lists_get(sarl_inverted_lists_t *inv, 
  sarl_unsigned_int index)
{
  return (sarl_int_array_t *)sarl_array_get(inv->lists, index);
};


inline sarl_unsigned_int sarl_sat_solver_contains_unitary_negated_clause(
  sarl_sat_solver_t *solver, sarl_unsigned_int var)
{
  sarl_int_array_t *relevent_clauses = sarl_inverted_lists_get(solver->inv_lists, var);
  sarl_unsigned_int i;
  if ( relevent_clauses ) {
    SARL_INT_ARRAY_FOR_EACH(i, relevent_clauses);
      sarl_clause_t *clause = (sarl_clause_t *)sarl_array_get(solver->clauses, i);
      if ( 
        sarl_bit_set_count(clause->neg) == 1 && 
        sarl_bit_set_count(clause->pos) == 0 && 
        sarl_bit_set_is_set(clause->neg, var) 
      ) {
        return 1;
      }
    SARL_INT_ARRAY_END;
  }
  return 0;
};

  
    
#endif
