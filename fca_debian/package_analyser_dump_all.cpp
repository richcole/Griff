#include "package_analyser.h"
#include <sstream>

int main(int argc, char **argv)
{
  if ( argc != 3 ) {
    cerr << "Usage package_analyser_dump_all available status" << endl;
    exit(-1);
  }

  Analyser analyser;
  analyser.read(argv[1], argv[2]);
  analyser.update_package_conflicts();

  ofstream    outp("packages.cnf");
  PackageSet  ps = analyser.available;
  Lexicon     lex;

  analyser.dump_cnf(ps, lex, outp, cerr);

  Lexicon::Words::const_iterator it;
  ofstream lex_outp("packages.lex");
  int index = 1;
  FOR_EACH(it, lex.words) {
    lex_outp << index << " " << *it << endl;
    ++index;
  };
};



  
