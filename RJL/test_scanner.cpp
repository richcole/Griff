#include "scanner.h"

using namespace RJL;
using namespace std;

int main() {
  
  // test the scanner by printing out scanned character to the screen.
  char buf[4096];
  Token::Type type;
  while( readline(cin, buf, sizeof(buf)), !cin.fail() ) {
    cout << "Line: " << buf;
    cout << "Tokens: " << endl;
    Scanner *scanner = new Scanner(new istringstream(buf));
    while( 
      type = scanner->curr()->type(), 
      (type != Token::TOK_EOF && type != Token::TOK_ERROR) 
    ) {
      cout << "  " << *scanner->curr() << endl;
      scanner->advance();
    }
    if ( type == Token::TOK_ERROR ) {
      cout << "  " << *scanner->curr() << endl;
      cout << "Exiting. (Error detected.)" << endl;
      break;
    }
  }
};
