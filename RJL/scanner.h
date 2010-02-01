/* scanner.h: copyright Richard Cole Jan 2006. See LICENCE for details. */

#include <string>
#include <iostream>
#include <sstream>

#include <map>
#include <ctype.h>
#include <set>

namespace RJL {

  using namespace std;

  struct TypeStringPair {
    int    type;
    string str;
  };
  
  class TypeStrings : public map<int, string> {
  public:
    TypeStrings(TypeStringPair *pairs) {
      while(pairs->type != 0) {
	(*this)[pairs->type] = pairs->str;
	++pairs;
      };
    };
  };

  class Token {
  public:
    enum Type { 
      TOK_ZERO_MARKER = 0,
      TOK_EOF, TOK_ERROR, TOK_INT, TOK_REAL, TOK_IDENT, TOK_TYPE_IDENT, TOK_LEFT, 
      TOK_RIGHT, TOK_SQ_LEFT, TOK_SQ_RIGHT, TOK_PLUS, TOK_MINUS, TOK_EQ,
      TOK_LE, TOK_LE_LE, TOK_LE_EQ, TOK_GR, TOK_GR_GR, TOK_GR_EQ,
      TOK_MINUS_EQ, TOK_PLUS_EQ, TOK_DOT_DOT, TOK_DOT, 

      TOK_DEF, TOK_FOR, TOK_WHILE, TOK_IF, TOK_EXTENDS, TOK_END, 
      TOK_IMPLEMENTS, TOK_SYNCHRONISED, TOK_CLASS,

      TOK_NL, /* newline */

      TOK_UNKNOWN, /* used to denote tokens missing */
    };

    Token(int line_num, int char_num, Type type) { 
       _line_num = line_num; _char_num = char_num; 
       _type = type; _int  = 0;     _real = 0; 
    };
    Token(int line_num, int char_num, Type type, string str)  {
      _line_num = line_num; _char_num = char_num;
      _type = type; _str  = str;     
    };
    Token(int line_num, int char_num, Type type, int integer) {
      _line_num = line_num; _char_num = char_num;
      _type = type; _int  = integer; 
    };
    Token(int line_num, int char_num, Type type, double real) {
      _line_num = line_num; _char_num = char_num;
      _type = type; _real = real;    
    };

    Type          type()      const { return _type; };
    string const& str()       const { return _str;  };
    int           integer()   const { return _int;  };
    double        real()      const { return _real; };

    void set_type(Type type) { _type = type; };

    void push_char(char c) { _str += c; };
    
  private:
    static TypeStrings __type_strings;

  public:
    string const& type_string() const {
      TypeStrings::const_iterator it = __type_strings.find(_type);
      if ( it == __type_strings.end() ) {
	return __type_strings[TOK_UNKNOWN];
      }
      else {
	return it->second;
      }
    };

    static string const& type_string(Token::Type type) {
      TypeStrings::const_iterator it = __type_strings.find(type);
      if ( it == __type_strings.end() ) {
	return __type_strings[TOK_UNKNOWN];
      }
      else {
	return it->second;
      }
    };      
    
  private:
    Type     _type;
    string   _str;
    int      _int;
    double   _real;

    int      _line_num;
    int      _char_num;
    string   *filename;
  };
  
  struct WordSetPair {
    string str; Token::Type type;
  };

  class WordSet : public map<string, Token::Type> {
  public:
    WordSet(WordSetPair const* word_pairs) {
      while(word_pairs->type != 0) { 
	(*this)[word_pairs->str] = word_pairs->type;
	++word_pairs; 
      };
    }
  };

  ostream& operator <<(ostream& out, Token const& tok);

  class Scanner {
  public:
    Scanner(istream* in) { 
      _in = in; 
      ok = 0; next_ok = 0; c = 0; next_c = 0; _curr = _next = 0; 
      _line_num = 1; _char_num = 1;
      advance_c(); advance_c();
      advance(); advance(); 
    };
    Token *curr() const  { return _curr; };
    Token *next() const  { return _next; };
    void advance();

  protected:
    void push_token(Token *token) {
      _curr = _next;
      _next = token;
    };

    void push_token(Token::Type type, double real) {
      push_token(new Token(_line_num, _char_num, type, real));
    };

    void push_token(Token::Type type, int num) {
      push_token(new Token(_line_num, _char_num, type, num));
    };

    void push_token(Token::Type type) {
      push_token(new Token(_line_num, _char_num, type));
    };

    void push_token(Token::Type type, string str) {
      push_token(new Token(_line_num, _char_num, type, str));
    };

    void advance_c() {
      if ( c == '\n' ) ++_line_num;
      c = next_c;
      ok = next_ok;
      next_c = _in->get();
      next_ok = ! _in->fail();
      if ( next_ok ) ++_char_num;
    };

  private:
    static WordSet __reserved_words;

    istream* _in;
    Token*   _curr;
    Token*   _next;
    int      c;
    int      next_c;
    int      ok;
    int      next_ok;

    int      _line_num;
    int      _char_num;
  };

  void readline(istream& in, char *buf, int len);

}; // namespace RJL
