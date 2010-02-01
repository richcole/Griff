#include <ctype.h>

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

struct Package;
struct Expression;

typedef deque<Expression *>  DExpression;
typedef deque<DExpression *> CExpression;

typedef hash_set<Package *> PackageSet;
typedef hash_map<string, Package *> PackageMap;
typedef hash_map<string, PackageSet> PackageSetMap;

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

struct Expression {
  string name;
  string version;
  string op;
};

struct Package {
  bool             is_available;
  bool             is_virtual;
  string           fullname;
  string           name;
  string           version;
  string           description;
  string           priority;

  hash_set<string> status;
  PackageSet       providers;
  PackageSet       conflict_packages;

  CExpression depends;
  CExpression provides;
  CExpression conflicts;
};

inline string fullname(string const& name, string const& version)
{
  if ( version == "" ) {
    return name;
  }
  else {
    return name + "-" + version;
  }
}

inline bool starts_with(char const* str, char const* pat, int &len)
{
  len = 0;
  while(*str && *pat) {
    if ( *str != *pat ) return false;
    ++str; ++pat; ++len;
  }
  return *pat == 0;
}

inline bool is_space(char const* buf)
{
  while( *buf ) {
    if ( ! isspace(*buf) ) return false;
    ++buf;
  }
  return true;
};

inline int index_of(char const* buf, char c) {
  int i = 0;
  while( *buf && *buf != c ) { ++buf; ++i; }
  return (*buf == c) ? i : -1;
};

inline bool read_expression(char const* inbuf, Expression& expr)
{
  string open, close;
  // skip white space
  char const* buf = inbuf;
  while( *buf && isspace(*buf) ) buf++;
  while( *buf && ! isspace(*buf) ) { expr.name += *buf; buf++; }
  while( *buf && isspace(*buf) ) ++buf;
  while( *buf && *buf == '(' ) { open += *buf; buf++; };
  while( *buf && isspace(*buf) ) ++buf;
  while( *buf && ! isspace(*buf) ) { expr.op += *buf; buf++; };
  while( *buf && isspace(*buf) ) ++ buf;
  while( *buf && (! isspace(*buf)) && (! (*buf == ')')) ) { 
    expr.version += *buf; buf++; 
  }
  while( *buf && isspace(*buf) ) ++buf;
  while( *buf && *buf == ')' ) { close += *buf; buf++; };

  if ( expr.version != "" && ( open != "(" || close != ")") ) {
    cerr << "Error in version expressions: " << inbuf << endl;
    return false;
  }

  return true;
}

inline bool read_dexpression(char const* buf, DExpression &dexpr)
{
  int comma_index;
  char dbuf[MAX_LINE_LEN];

  while ( (comma_index = index_of(buf, '|')) > 0 ) {
    strcpy(dbuf, buf);
    dbuf[comma_index] = 0;
    Expression *expr = new Expression();
    if ( ! read_expression(dbuf, *expr) ) {
      cerr << "Invalid dexpression: " << dbuf << endl;
      return false;
    }
    dexpr.push_back(expr);
    buf += comma_index + 1;
  }

  strcpy(dbuf, buf);
  Expression *expr = new Expression();
  if ( ! read_expression(dbuf, *expr) ) {
    cerr << "Invalid dexpression: " << dbuf << endl;
    return false;
  }
  dexpr.push_back(expr);

  return true;
};

inline bool read_cexpression(char const* buf, CExpression &cexpr) {
  int comma_index;
  char dbuf[MAX_LINE_LEN];
  
  while ( (comma_index = index_of(buf, ',')) > 0 ) {
    strcpy(dbuf, buf);
    dbuf[comma_index] = 0;
    DExpression *dexpr = new DExpression();
    if ( ! read_dexpression(dbuf, *dexpr) ) {
      cerr << "Invalid cexpression: " << dbuf << endl;
      return false;
    }
    cexpr.push_back(dexpr);
    buf += comma_index + 1;
  }

  strcpy(dbuf, buf);
  DExpression *dexpr = new DExpression();
  if ( ! read_dexpression(dbuf, *dexpr) ) {
    cerr << "Invalid cexpression: " << dbuf << endl;
    return false;
  }
  cexpr.push_back(dexpr);

  return true;
};

inline char const* next_tok(char const* buf, char *tok)
{
  while(*buf && isspace(*buf)) ++buf;
  while(*buf && !isspace(*buf)) { *tok = *buf; ++tok; ++buf; };
  *tok = 0;
  return buf;
}

inline void read_package(istream& inp, Package &package) {
  char buf[MAX_LINE_LEN], tok[MAX_LINE_LEN];
  int  prefix_len;
  while( *buf = 0, inp.getline(buf, sizeof(buf)), !inp.fail() ) {
    if ( starts_with(buf, "Provides: ", prefix_len) ) {
      if ( ! read_cexpression(buf + prefix_len, package.provides) ) {
        cerr << "Invalid provides: " << package.name << endl;
      }
    }
    if ( starts_with(buf, "Depends: ", prefix_len) ) {
      if ( ! read_cexpression(buf + prefix_len, package.depends) ) {
        cerr << "Invalid depends: " << package.name << endl;
        cerr << "  depends=" << buf << endl;
      }
    }
    if ( starts_with(buf, "Conflicts: ", prefix_len) ) {
      if ( ! read_cexpression(buf + prefix_len, package.conflicts) ) {
        cerr << "Invalid conflicts: " << package.name << endl;
      }
    }
    if ( starts_with(buf, "Package: ", prefix_len) ) {
      package.name = buf + prefix_len;
    }
    if ( starts_with(buf, "Version: ", prefix_len) ) {
      package.version = buf + prefix_len;
    }
    if ( starts_with(buf, "Package: ", prefix_len) ) {
      package.name = buf + prefix_len;
    }
    if ( starts_with(buf, "Status: ", prefix_len) ) {
      char const* next = buf + prefix_len; 
      while (next=next_tok(next, tok), *tok) {
        package.status.insert(string(tok));
      }
    }
    if ( starts_with(buf, "Priority: ", prefix_len) ) {
      package.priority = buf + prefix_len;
    }
    if ( is_space(buf) ) {
      break;
    }
  }
  package.fullname = fullname(package.name, package.version);
  package.is_virtual = (package.version == "");
};

template <class S, class T>
inline bool is_member(T const& t, S const& s)
{
  typename S::const_iterator it = s.find(t);
  return it != s.end();
};

template <class K, class S>
inline typename S::data_type get(K const& k, S const& s) {
  typename S::const_iterator it = s.find(k);
  if ( it == s.end() ) {
    return 0;
  }
  else {
    return it->second;
  }
};

inline ostream& operator<<(ostream& outp, Expression const& expr) {
  outp << expr.name;
  if ( expr.op != "" ) {
    outp << " (" << expr.op << " " << expr.version << ")";
  }
  return outp;
}

inline ostream& operator<<(ostream& outp, DExpression const& expr) {
  DExpression::const_iterator it = expr.begin();
  while( it != expr.end() ) {
    outp << **it;
    ++it;
    if ( it != expr.end() ) {
      outp << " | ";
    }
  }
  return outp;
}

inline ostream& operator<<(ostream& outp, CExpression const& expr) {
  CExpression::const_iterator it = expr.begin();
  while( it != expr.end() ) {
    outp << **it;
    ++it;
    if ( it != expr.end() ) {
      outp << ", ";
    }
  }
  return outp;
}

template <class S>
  S& operator |=(S &s, S const& t)
{
  typename S::const_iterator it;
  FOR_EACH(it, t) {
    s.insert(*it);
  }
  return s;
}

template <class S>
  S& operator -=(S &s, S const& t)
{
  typename S::const_iterator it;
  FOR_EACH(it, t) {
    s.erase(*it);
  }
  return s;
}

template <class S>
  S& operator &=(S &s, S const& t)
{
  S rem_set;
  typename S::const_iterator it;
  FOR_EACH(it, s) {
    if ( ! is_member(*it, t) ) {
      rem_set.insert(*it);
    }
  }

  s -= rem_set;
  return s;
};

class Analyser {
public:
  PackageMap                packages_by_fullname;
  PackageSetMap             packages_by_name;
  PackageSet                install, installed, install_or_installed;
  PackageSet                all, available, virtual_packages, status;

  void read(string const& available_filename, string const& status_filename);
  void update_install_or_installed();

  bool matches(CExpression const& e, PackageSet const& P, 
    DExpression const*& fail_expr);

  Package* matches(DExpression const& e, PackageSet const& P);
  Package* matches(Expression const& e, PackageSet const& P);
  bool matches(string av, string op, string bv);

  PackageSet *provides_closure(Package *p);
  PackageSet *provides_closure(PackageSet const& P);

  bool check_multiple_versions();
  bool check_dependencies(PackageSet const& target);
  bool check_conflicts(PackageSet const& target);
  void print_new_packages();

  PackageSet* provides(Package const& p);

  void construct_virtuals();
  void read_available(istream &inp);

  void update_conflict_packages(Package& p);
  void make_conflict_reflexive(Package& p);
  PackageSet* matching_packages(Expression const& expr);
  PackageSet* conflicts(Package *p, PackageSet const& p);
  void update_package_conflicts();
  bool solve(
    CExpression &ce, PackageSet & ps, CExpression &unsolved, 
    vector<pair<CExpression *, PackageSet *> > &solutions
  );

  void print_solutions(vector<pair<CExpression *, PackageSet *> > &solutions);
  PackageSet* maching_packages(DExpression const& de);
  void package_closure(CExpression cs, PackageSet &ps);
  void dump_cnf(PackageSet &ps, Lexicon &lex, ostream& out, ostream& outp_reason);
  bool dump_cnf(CExpression const& ce, Lexicon &lex, ostream& out);

  void dump_conflicts();
};

int version_cmp(char const* av, char const* bv);

#define TRACE_ASSERT(x) \
  if ( ! (x) ) { \
    cout << __FILE__ << ":" << __LINE__ << ": Program Error: "; \
    cout << ": Assert failed '" << #x;\
    cout << "'" << endl; \
  };


