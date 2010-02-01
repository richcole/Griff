#include "sarl_sat.h"
#include "sarl_power_set.h"
#include "sarl_cnf.h"
#include "sarl_string_table.h"
#include "sarl_context.h"

#include <errno.h>

#include <fstream>
#include <iostream>
#include <ext/hash_set>

using namespace std;
using namespace __gnu_cxx;

#define FOR_EACH(it, S) for((it)=(S).begin();(it)!=(S).end();++(it))

void set_clause_name(
  sarl_string_table_t* object_st, sarl_unsigned_int g, sarl_sat_solver_t *D)
{
};


int main(int argc, char **argv)
{
  if ( argc != 5 ) {
    fprintf(stderr, 
      "Usage: gen_sat_context <background.cnf> <foreground.cnf> <clauses.txt> > <lexicon> <output.cxt>\n"
    );
    exit(-1);
  }

  // load the background
  sarl_sat_solver_t *B = sarl_sat_solver_default_new();

  switch ( load_cnf(B, argv[1]) ) {
  case LOAD_CNF_RESULT_FILE_ERROR:
    fprintf(stderr, "Exiting.\n");
    return -1;
  case LOAD_CNF_RESULT_UNSAT:
    fprintf(stderr, "Error: background clauses were unsatisfiable.");
    return -1;
  case LOAD_CNF_RESULT_OK:
    ;
  }

  // load the variable name lexicon
  sarl_string_table_t* variable_st = sarl_string_table_default_new();
  sarl_string_table_t* object_st = sarl_string_table_default_new();

  { 
    FILE *inp = fopen(argv[4], "r");
    if ( inp == 0 ) {
      fprintf(stderr, "Error: unable to load lexicon '%s'\n", argv[4]);
    }
    char buf[4096];
    while( fgets(buf, sizeof(buf), inp) != 0 ) {
      sarl_unsigned_int index;
      char *remainder;
      if ( 1 != sscanf(buf, "%u", &index) ) {
        for(remainder=buf; *remainder != 0 && *remainder != ' '; ++remainder);
        while(*remainder == ' ' ) ++remainder;
        if ( *remainder == 0 ) {
          fprintf(stderr, 
            "Error: while reading lexicon, unable to parse line '%s'\n", 
            buf);
        }
        else {
          sarl_string_table_set(variable_st, index, remainder);
        }
      }
    }
  }

  // load the foreground
  sarl_sat_solver_t *D = sarl_sat_solver_default_new();

  switch ( load_cnf(D, argv[2]) ) {
  case LOAD_CNF_RESULT_FILE_ERROR:
    fprintf(stderr, "Exiting.\n");
    return -1;
  case LOAD_CNF_RESULT_UNSAT:
    fprintf(stderr, "Error: background clauses were unsatisfiable.");
    return -1;
  case LOAD_CNF_RESULT_OK:
    ;
  }

  // load the clauses
  hash_set<int> clauses;
  { 
    ifstream inp(argv[3]);
    int clause = 0;
    char c;
    bool have_clause = false;
    while( inp.get(c), (! inp.eof() && ! inp.fail()) ) {
      if ( isdigit(c) ) {
        have_clause = true;
        clause *= 10; clause += c - '0';
      }
      else if ( have_clause ) {
        clauses.insert(clause);
        clause = 0;
        have_clause = false;
      }
    }
  }
  
  // Now for each clause in clauses determine which positive literals
  // are turned on and which are turned off
  sarl_unsigned_int var, clause_index;
  sarl_bit_set_t*   var_set = sarl_bit_set_default_new();
  sarl_bit_set_t*   empty = sarl_bit_set_default_new();

  sarl_context_t*   context = sarl_context_new();
  sarl_unsigned_int g;

  hash_set<int>::const_iterator it;
  FOR_EACH(it, clauses) {
    clause_index = *it;
    sarl_clause_t *clause = 
      (sarl_clause_t *)sarl_array_get(D->clauses, clause_index);

    sarl_sat_state_t *state = sarl_sat_save_state(B);
    if ( ! sarl_sat_solver_add_clause(B, clause) ) {
      fprintf(stderr, "Error: clause is UNSAT:");
      sarl_clause_print(clause, stderr);
      fprintf(stderr, "\n");
    }
    else {
      g = clause_index;
      set_clause_name(object_st, g, D);
      SARL_BIT_SET_FOR_EACH(var, B->vars);
        if ( sarl_bit_set_is_set(B->pos, var) ) {
          sarl_context_insert(context, g, var);
        } else if ( sarl_bit_set_is_set(B->neg, var) ) {
          sarl_context_insert(context, g, var + sarl_bit_set_last(B->vars));
        }
        else {
          sarl_bit_set_clear_all(var_set);
          sarl_bit_set_set(var_set, var);
          sarl_sat_state_t *state = sarl_sat_save_state(B);
          if ( 
            ! sarl_sat_solver_add_clause(B, sarl_clause_new(var_set, empty)) &&
            ! sarl_sat_solver_search(B)
          ) {
            sarl_context_insert(context, g, var + sarl_bit_set_last(B->vars));
          }
          else {
            sarl_sat_restore_state(B, state);
            if ( 
              ! sarl_sat_solver_add_clause(
                B, sarl_clause_new(empty, var_set)) &&
              ! sarl_sat_solver_search(B)
            ) {
              sarl_context_insert(context, g, var);
            }
            sarl_sat_restore_state(B, state);
          }
        } 
        fflush(stdout);
      SARL_BIT_SET_END;
    }
    sarl_sat_restore_state(B, state);
  };
};

 
