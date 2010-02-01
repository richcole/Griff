#include "package_analyser.h"
#include <sstream>

int main(int argc, char **argv)
{
  if ( argc < 3 ) {
    cerr << "Usage dpkg available status [extras]" << endl;
    exit(-1);
  }

  Analyser analyser;
  analyser.read(argv[1], argv[2]);
  analyser.update_package_conflicts();

  ostringstream st_buf;
  int i;

  for(i=3;i<argc;++i) {
    st_buf << argv[i];
    if ( i+1 < argc ) { st_buf << ", "; };
  }
  
  CExpression target;
  CExpression unsolved;
  Lexicon     lex;

  ofstream    outp("packages.cnf");

  PackageSet ps;
  vector<pair<CExpression *, PackageSet *> > solutions;
  bool       result;

  read_cexpression(st_buf.str().c_str(), target);

  // analyser.dump_conflicts();
  result = analyser.dump_cnf(target, lex, outp);
  if ( result ) {
    analyser.package_closure(target, ps);
    analyser.dump_cnf(ps, lex, outp, cerr);

    Lexicon::Words::const_iterator it;
    ofstream lex_outp("packages.lex");
    int index = 1;
    FOR_EACH(it, lex.words) {
      lex_outp << index << " " << *it << endl;
      ++index;
    };
  }
  else {
    cerr << "Error: some dependencies couldn't be resolved." << endl;
    return -1;
  }

};



  
