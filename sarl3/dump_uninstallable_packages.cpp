#include "sarl_sat.h"
#include "sarl_power_set.h"
#include "sarl_cnf.h"
#include "sarl_variable_closure.h"
#include "sarl_int_array.h"
#include <errno.h>

#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include <ext/hash_map>
#include <ext/hash_set>

using namespace __gnu_cxx;
using namespace std;

#define FOR_EACH(it, S) for((it)=(S).begin();(it)!=(S).end();++(it))
#define MAX_LINE_LEN    (1024 * 12)

namespace __gnu_cxx {
  template<> struct hash<std::string>  {
    size_t operator()( const std::string& x ) const
    {
      return hash< const char* >()( x.c_str() );
    }
  };
  template<class T> struct hash<T *>  {
    size_t operator()( T* x ) const
    {
      return (size_t)x;
    }
  };
}          

class Lexicon {
public:
  typedef std::vector<string> Words;
  typedef hash_map<string,int> Indexes;
  string error;

  Indexes indexes;
  Words words;

  Lexicon() {
    error = "Error: invalid index";
  }

  int index(string const& s) {
    Indexes::const_iterator it = indexes.find(s);
    if ( it == indexes.end() ) {
      it = indexes.insert(Indexes::value_type(s, words.size())).first;
      words.push_back(s);
    }
    return it->second+1;
  }

  string const& word(int index) {
    if ( index == 0 || index > (int)words.size() ) {
      return error;
    }
    else {
      return words[index-1];
    }
  }
};

bool load_lexicon(char const* filename, Lexicon& lex)
{
  char buf[4096], *s;
  ifstream inp(filename);
  int index;

  while( inp.getline(buf, sizeof(buf)), !inp.fail() ) {
    index = 0;
    for(s=buf; isdigit(*s); ++s) {
      index *= 10; index += *s - '0';
    }
    while(isspace(*s)) ++s;
    if ( lex.index(s) != index ) {
      return false;
    }
  }

  return true;
};

void dump_clauses_to_file(sarl_sat_solver_t *solver, sarl_int_array_t *inv_cont_map, string filename)
{
  ofstream outp(filename.c_str());
  sarl_clause_t *clause;
  sarl_unsigned_int i;

  SARL_ARRAY_FOR_EACH(sarl_clause_t, clause, solver->clauses);
    SARL_BIT_SET_FOR_EACH(i, clause->pos);
      outp << sarl_int_array_get(inv_cont_map, i) << " ";
    SARL_BIT_SET_END;
    SARL_BIT_SET_FOR_EACH(i, clause->neg);
      outp << "-" << sarl_int_array_get(inv_cont_map, i) << " ";
    SARL_BIT_SET_END;
    outp << endl;
  SARL_ARRAY_END;
};

int main(int argc, char **argv)
{
  if ( argc != 5 ) {
    fprintf(stderr, 
	    "Usage: %s <packages.cnf> <packages.lex> <first_package_id> <num_of_packages>\n", argv[0]
    );
    exit(-1);
  }

  int first_package_id, num_of_packages;

  if ( 1 != sscanf(argv[3], "%d", &first_package_id) ) {
    fprintf(stderr, "Error: unable to parse first_package_id, '%s'\n", argv[3]);
  };

  if ( 1 != sscanf(argv[4], "%d", &num_of_packages) ) {
    fprintf(stderr, "Error: unable to parse num_of_packages, '%s'\n", argv[4]);
  };

  sarl_sat_solver_t *S = sarl_sat_solver_default_new();

  switch ( load_cnf(S, argv[1]) ) {
  case LOAD_CNF_RESULT_FILE_ERROR:
    fprintf(stderr, "Exiting.\n");
    return -1;
  case LOAD_CNF_RESULT_UNSAT:
    fprintf(stderr, "Error: background clauses were unsatisfiable.");
    return -1;
  case LOAD_CNF_RESULT_OK:
    ;
  }

  Lexicon lex;
  if ( ! load_lexicon(argv[2], lex) ) {
    cerr << "Error: unable to load lexicon" << endl;
    return -1;
  }

  sarl_unsigned_int v;
  sarl_bit_set_t *v_set = sarl_bit_set_default_new();
  sarl_bit_set_t *empty = sarl_bit_set_default_new();

  sarl_unsigned_int n = first_package_id + num_of_packages;
  if ( n > sarl_bit_set_last(S->vars) ) {
    fprintf(stderr, "Warning: first_package_id + num_of_packages exceeds number of clauses... truncating :)\n");
    n = sarl_bit_set_last(S->vars);
  };

  for(v = first_package_id; v <= n; ++v) {

    cout << "Checking " << lex.word(v) << ": ";
    cout.flush();

    sarl_bit_set_clear_all(v_set);
    sarl_bit_set_set(v_set, v);

    sarl_variable_closure(v_set, S, v);

    cout << "num vars=" << sarl_bit_set_count(v_set);
    cout.flush();

    sarl_int_array_t *cont_map = sarl_int_array_default_new();
    sarl_int_array_t *inv_cont_map = sarl_int_array_default_new();
    sarl_sat_solver_t *v_solver = 
      sarl_collect_relevant_clauses(v_set, S, cont_map, inv_cont_map);

    cout << " num relevant clauses=" << sarl_array_count(v_solver->clauses);
    cout.flush();

    if ( sarl_sat_solver_contains_unitary_negated_clause(v_solver, sarl_int_array_get(cont_map, v)) ) {
      cout << " state=Uninstallable" << endl;
    }
    else {
      sarl_bit_set_t* v_singleton_set = sarl_bit_set_default_new();
      sarl_bit_set_set(v_singleton_set, sarl_int_array_get(cont_map, v));
      if ( 
        ! sarl_sat_solver_add_clause(
          v_solver, sarl_clause_new(v_singleton_set, empty)
        ) ||
        ! sarl_sat_solver_search(v_solver) 
      ) {
        cout << " state=Uninstallable" << endl;
        dump_clauses_to_file(v_solver, inv_cont_map, lex.word(v) + ".cnf");
      }
      else {
        cout << " state=Ok" << endl;
      }
    }
  }
};

 
