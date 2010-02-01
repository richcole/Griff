/* parser.h: copyright Richard Cole Jan 2006. See LICENCE for details. */

#include "scanner.h"
#include <vector>

namespace RJL {

  using namespace std;

  class StartSet : public set<Token::Type> {
  public:
    StartSet(Token::Type const* types) {
      while(*types != 0) { insert(*types); ++types; };
    };
  };

  class Node {
  public:
    enum Type {
      AST_ZERO_MARKER = 0,
      AST_TOKEN,
      AST_BASIC_TYPE,
      AST_LIST,
      AST_CLASS_DEF,
      AST_METHOD_DEF,
      AST_UNKNOWN
    };

    void push(Node *node) {
      children.push_back(node);
    };

    void print(int indent, ostream& out) const {
      int i; for(i=0;i<indent;++i) { out << "  "; };
      out << "(" << type_string(type);
      if ( token ) { out << " " << token; }; 
      if (children.size() > 0 ) {
	vector<Node *>::const_iterator it; 
	out << endl; 
	for(it=children.begin(); it!=children.end(); ++it) {
	  if ( *it != 0 ) {
	    (*it)->print(indent+1, out);
	    out << endl;
	  }
	  else {
	    out << "(null)" << endl;
	  }
	}
      }
      out << ")";
    };

    static string const& type_string(Type type) {
      TypeStrings::const_iterator it = __type_strings.find(type); 
      if ( it != __type_strings.end() ) {
	return it->second;
      }
      else {
	return __type_strings[AST_UNKNOWN];
      }
    }

    Node(Type a_type, Token *a_token = 0) { type = a_type; token = a_token; };

    Type              type;
    vector<Node *> children;
    Token*            token;

  private:
    static TypeStrings __type_strings;
  };

  class Parser {
    
  public:
    static StartSet __basic_type_ss;

    Parser(Scanner *scanner) {
      _scanner = scanner;
    };
  
    Node *class_def() {
      Node *node = new Node(Node::AST_CLASS_DEF);
      mustbe_advance(Token::TOK_CLASS); 
      node->push(basic_type());
      mustbe_advance(Token::TOK_NL);
      while( have(Token::TOK_NL) ) advance();
      while( ! have(Token::TOK_END) ) {
	node->push(class_inner());
	if ( have(Token::TOK_ERROR) || have(Token::TOK_EOF) ) {
	  return node;
	}
      };
      mustbe_advance(Token::TOK_END); 
    };

    void mustbe(Token::Type type) {
      if ( ! have(type) ) {
	syntax_error(curr(), 
          "Expected " + Token::type_string(type) + ", but found " + 
          curr()->type_string() + "."
	);
      }
    };

    void mustbe(StartSet const& ss) {
      if ( ! have(ss) ) {
	syntax_error(curr(), "Parse error, token is of unexpected type.");
      }
    };

    void mustbe_advance(Token::Type type) {
      if ( ! have(type) ) {
	syntax_error(curr(), 
          "Expected " + Token::type_string(type) + ", but found " + 
          curr()->type_string() + "."
	);
      }
      else {
	advance();
      };
    };

    void mustbe_advance(StartSet const& ss) {
      if ( ! have(ss) ) {
	syntax_error(curr(), "Token is of unexpected type.");
      }
      else {
	advance();
      }
    };

    bool have(Token::Type type) const {
      return curr()->type() == type;
    };

    bool have(StartSet const& ss) const {
      return ss.find(curr()->type()) != ss.end();
    };

    void syntax_error(Token const* tok, string err) {
      cout << "Error: " << err << endl;
    };

    void advance() {
      _scanner->advance();
    };

    Token * curr() const { return _scanner->curr(); };
    Token * next() const { return _scanner->next(); };

    Node* basic_type() {
      Node *node = new Node(Node::AST_BASIC_TYPE);
      
      while ( have(__basic_type_ss) ) {
	if ( have(Token::TOK_LEFT) ) {
	  node->push(basic_type());
	  mustbe_advance(Token::TOK_RIGHT);
	}
	else if ( have(Token::TOK_IDENT) ) {
	  node->push(new Node(Node::AST_TOKEN, curr()));
	  advance();
	}
	else if ( have(Token::TOK_TYPE_IDENT) ) {
	  node->push(new Node(Node::AST_TOKEN, curr()));
	  advance();
	}
      }
      if ( node->children.size() == 0 ) {
	syntax_error(curr(), "Expected type expression.");
      };

      return node;
    };

    Node *class_inner() {
      Node *node = new Node(Node::AST_BASIC_TYPE);
      if ( have(Token::TOK_DEF) ) {
	node = new Node(Node::AST_METHOD_DEF);
      }
      else {
	syntax_error(curr(), "Expected method definition.");
      };

      return node;
    };

  private:
    Scanner *_scanner;
  };
      

}; // namespace RJL

