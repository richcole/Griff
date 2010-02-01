#include "package_analyser.h"
#include <sstream>

int main(int argc, char **argv)
{
  if ( argc != 1 ) {
    cerr << "Usage " << argv[0] << " < Packages.txt" << endl;
    cerr << "  output files are packages.cnf, packages.lex, and reasons.txt";
    cerr << endl;
    exit(-1);
  }

  Analyser analyser;
  analyser.read_available(cin);
  analyser.construct_virtuals();
  analyser.update_package_conflicts();

  ofstream    outp("packages.cnf");
  ofstream    outp_reasons("reasons.txt");
  ofstream    outp_lex("packages.lex");

  PackageSet  ps = analyser.available;
  Lexicon     lex;

  analyser.dump_cnf(ps, lex, outp, outp_reasons);

  Lexicon::Words::const_iterator it;
  int index = 1;
  FOR_EACH(it, lex.words) {
    outp_lex << index << " " << *it << endl;
    ++index;
  };
};



  
