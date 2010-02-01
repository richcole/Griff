/* parser.cpp: copyright Richard Cole Jan 2006. See LICENCE for details. */


#include "parser.h"
#include <vector>

namespace RJL {

  static TypeStringPair __type_string_pairs[] = {
    { Node::AST_TOKEN, "AST_TOKEN" },
    { Node::AST_BASIC_TYPE, "AST_BASIC_TYPE" },
    { Node::AST_LIST, "AST_LIST" },
    { Node::AST_CLASS_DEF, "AST_CLASS_DEF" },
    { Node::AST_METHOD_DEF, "AST_METHOD_DEF" },
    { Node::AST_UNKNOWN, "AST_UNKNOWN" },
    { Node::AST_ZERO_MARKER, "AST_ZERO_MARKER" }
  };

  TypeStrings Node::__type_strings = __type_string_pairs; 
  
  Token::Type __basic_type_token_types[] = {
    Token::TOK_IDENT, Token::TOK_TYPE_IDENT, Token::TOK_LEFT, 
    Token::TOK_ZERO_MARKER
  };
  StartSet Parser::__basic_type_ss = __basic_type_token_types;

};
