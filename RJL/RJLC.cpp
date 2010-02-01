#include "Parser.h"
#include "Scanner.h"
#include <iostream>

using namespace std;
using namespace RJL;

int main (int argc, char *argv[]) {

  if (argc > 1) {
    Scanner *scanner = new Scanner(argv[1]);
    Parser *parser = new Parser(scanner);
    parser->Parse();
    if (parser->errors->count == 0) {
      cout << "Parse Complete: No Errors" << endl;
    }
  } 
  else {
    printf("-- No source file specified\n");
  }

  return 0;

}
