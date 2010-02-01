/* scanner.cpp: copyright Richard Cole Jan 2006. See LICENCE for details. */

#include "scanner.h"

namespace RJL {

  using namespace std;

  WordSetPair reserved_word_pairs[] = {
    { "class", Token::TOK_CLASS }, 
    { "def", Token::TOK_DEF }, 
    { "for", Token::TOK_FOR },
    { "while", Token::TOK_WHILE }, 
    { "if", Token::TOK_IF }, 
    { "extends", Token::TOK_EXTENDS },
    { "end", Token::TOK_END }, 
    { "implements", Token::TOK_IMPLEMENTS },
    { "synchronised", Token::TOK_SYNCHRONISED },
    { "END_OF_SEQUENCE", Token::TOK_ZERO_MARKER }
  };
    
  WordSet Scanner::__reserved_words(reserved_word_pairs);

  static TypeStringPair __type_string_pairs[] = {
    { Token::TOK_EOF, "TOK_EOF" },
    { Token::TOK_ERROR, "TOK_ERROR" },
    { Token::TOK_INT, "TOK_INT" },
    { Token::TOK_REAL, "TOK_REAL" },
    { Token::TOK_IDENT, "TOK_IDENT" },
    { Token::TOK_LEFT, "TOK_LEFT" },
    
    { Token::TOK_RIGHT, "TOK_RIGHT" },
    { Token::TOK_SQ_LEFT, "TOK_SQ_LEFT" },
    { Token::TOK_SQ_RIGHT, "TOK_SQ_RIGHT" },
    { Token::TOK_PLUS, "TOK_PLUS" },
    { Token::TOK_MINUS, "TOK_MINUS" },
    { Token::TOK_EQ, "TOK_EQ" },
    { Token::TOK_DOT, "TOK_DOT" },
    
    { Token::TOK_DOT_DOT, "TOK_DOT_DOT" },
    { Token::TOK_PLUS_EQ, "TOK_PLUS_EQ" },
    { Token::TOK_MINUS_EQ, "TOK_MINUS_EQ" },
    
    { Token::TOK_TYPE_IDENT, "TOK_TYPE_IDENT" },
    
    { Token::TOK_NL, "TOK_NL" },
    { Token::TOK_UNKNOWN, "TOK_UNKNOWN" },
    
    { Token::TOK_LE, "TOK_LE" },
    { Token::TOK_LE_LE, "TOK_LE_LE" },
    { Token::TOK_LE_EQ, "TOK_LE_EQ" },
    { Token::TOK_GR, "TOK_GR" },
    { Token::TOK_GR_GR, "TOK_GR_GR" },
    { Token::TOK_GR_EQ, "TOK_GR_EQ" },
    
    { Token::TOK_CLASS, "TOK_CLASS" },
    { Token::TOK_DEF, "TOK_DEF" },
    { Token::TOK_FOR, "TOK_FOR" },
    { Token::TOK_WHILE, "TOK_WHILE" },
    { Token::TOK_IF, "TOK_IF" },
    { Token::TOK_EXTENDS, "TOK_EXTENDS" },
    { Token::TOK_END, "TOK_END" },
    { Token::TOK_IMPLEMENTS, "TOK_IMPLEMENTS" },
    { Token::TOK_SYNCHRONISED, "TOK_SYNCHRONISED" },

    { Token::TOK_ZERO_MARKER, "TOK_ZERO_MARKER" },
  };

  TypeStrings Token::__type_strings = __type_string_pairs;

  ostream& operator <<(ostream& out, Token const& tok) {
    out << tok.type_string();
    switch(tok.type()) {
    case Token::TOK_ERROR: 
    case Token::TOK_IDENT: 
    case Token::TOK_TYPE_IDENT: 
      out << " [" << tok.str() << "]"; break;
    case Token::TOK_INT: 
      out << " [" << tok.integer() << "]"; break;
    case Token::TOK_REAL: 
      out << " [" << tok.real() << "]"; break;
    default: break;
    };
    return out;
  };

  void Scanner::advance() {
    // skip white space
    while(ok && (c == ' ' || c == '\t') ) {
      advance_c();
    }

    // skip comments
    if ( ok && c == '#' ) {
      while( ok && c != '\n' && c != '\r' && c != '\f' ) {
	advance_c();
      }
      if ( ok ) {
	push_token(Token::TOK_NL);
	advance_c();
      }
      else {
	push_token(Token::TOK_EOF);
      }
    }
    else if ( (! ok) ) {
      push_token(Token::TOK_EOF);
    }

    // otherwise c is the first character of the token
    // check for ident
    else if ( isalpha(c) || c == '_' ) {
      Token *tok = new Token(_line_num, _char_num, Token::TOK_IDENT);
      while( c == '_' || c == '-' || isalnum(c) ) {
	tok->push_char(c);
	advance_c();
	if ( (! ok) ) break;
      }
      WordSet::const_iterator it = __reserved_words.find(tok->str());
      if ( it != __reserved_words.end() ) {
	tok->set_type(it->second);
      }
      push_token(tok);
    }

    // check for a number
    else if ( isdigit(c) || (c == '.' && isdigit(next_c)) ) {
      int num = 0;
      while( isdigit(c) ) {
	num = (num * 10) + (c - '0');
	advance_c();
	if (! ok) break;
      }
      if ( c == '.' ) {
	double real = num;
	double m = 10;
	if ( next_c == '.' ) {
	  push_token(Token::TOK_INT, num);
	}
	else if ( isdigit(next_c) ) {
	  advance_c();
	  while ( ok && isdigit(c) ) {
	    real += ((double)(c - '0')) / m;
	    m *= 10;
	    advance_c();
	  }
	  push_token(Token::TOK_REAL, real);
	}
	else {
	  push_token(Token::TOK_ERROR, "Invalid Number");
	}
      }
      else {
	push_token(Token::TOK_INT, num);
      }
    }
    else if ( ispunct(c) ) {
      switch(c) {
      case '(': push_token(Token::TOK_LEFT); break;
      case ')': push_token(Token::TOK_RIGHT); break;
      case '[': push_token(Token::TOK_SQ_LEFT); break;
      case ']': push_token(Token::TOK_SQ_RIGHT); break;
      case '.': 
	if ( next_c == '.' ) { 
	  advance_c(); push_token(Token::TOK_DOT_DOT);
	}
	else { push_token(Token::TOK_DOT); };
	break; 
      case '<': 
	if ( next_c == '.' ) { 
	  advance_c(); push_token(Token::TOK_LE_LE);
	}
	else if ( next_c == '=' ) {
	  advance_c(); push_token(Token::TOK_LE_EQ);
	}
	else { push_token(Token::TOK_LE); };
	break; 
      case '>': 
	if ( next_c == '>' ) { 
	  advance_c(); push_token(Token::TOK_GR_GR);
	}
	else if ( next_c == '=' ) {
	  advance_c(); push_token(Token::TOK_GR_EQ);
	}
	else { push_token(Token::TOK_GR); };
	break; 
      case '+': 
	if ( next_c == '=' ) {
	  advance_c(); push_token(Token::TOK_PLUS_EQ);
	}
	else { push_token(Token::TOK_PLUS); }
	break;
      case '-': 
	if ( next_c == '=' ) {
	  advance_c(); push_token(Token::TOK_MINUS_EQ);
	}
	else { push_token(Token::TOK_MINUS); }
	break;
      case '\'':
	if ( isalpha(next_c) ) {
	  Token *tok = new Token(_line_num, _char_num, Token::TOK_TYPE_IDENT);
	  while(isalpha(next_c)) {
	    tok->push_char(next_c);
	    advance_c();
	  }
	  push_token(tok);
	}
	else {
	  push_token(Token::TOK_ERROR, "Error, ident must follow a quote (')");
	}
	break;
      case '=': 
	push_token(Token::TOK_EQ); break;
      default: 
	push_token(Token::TOK_ERROR, "Unrecognised punctuation");
      }
      advance_c();
    }
    else if ( c == '\n' || c == '\r' || c == '\f' ) {
      push_token(Token::TOK_NL);
      advance_c();
    }
    else {
      push_token(Token::TOK_ERROR, "Unrecognised character");
      advance_c();
    }
  }


  void readline(istream& in, char *buf, int len) {
    int c;
    c = in.get();
    while(! in.fail() && len > 1) {
      if ( c == '\n' || c == '\r' || c == '\f' ) {
	*buf = c; --len; ++buf;
	*buf = 0;
	return;
      }
      else {
	*buf = c; --len; ++buf;
	c = in.get();
      }
    }
  };

} // namespace RJL





