#ifndef S_EXPR_H
#define S_EXPR_H

#include <gc/gc_cpp.h>
#include <deque>
#include <vector>
#include <iostream>

namespace RJL {

  using namespace std;

  enum SExprType { SE_IDENT = 0x0, SE_REF = 0x1 };

  class SExpr;
  class SExpr : public deque<SExpr *> {
    size_t _id;
  public:
    SExpr(size_t id, SExprType type = SE_IDENT) { 
      _id = (id << 0x1) | (type & 0x1);
    };
    size_t id() const { return _id >> 0x1; };
    void id(size_t id) { int type = (_id & 0x1); _id = (id << 0x1) | type; };
    size_t type() const { return _id & 0x1; };

    bool match(SExpr const& expr) const {
      if ( type() != expr.type() || id() != expr.id() ) {
	return false;
      }
      const_iterator it = begin(), expr_it = expr.begin();
      while(it != end() && expr_it != expr.end()) {
	if ( ! (*it)->match(**expr_it) ) { return false; };
	++it, ++expr_it;
      }
      return (it == end()) && (expr_it == expr.end());
    };

    void print(ostream& out, Lexicon const& lex) {
      if ( size() > 0 ) { out << "("; };
      if ( type() == SE_IDENT ) {
	out << lex.word(id());
	if ( size() > 0 ) { out << " "; };
	const_iterator it = begin();
	while(it != end()) {
	  if (*it) { (*it)->print(out, lex); } else { out << ":nil"; };
	  ++it;
	  if ( it != end() ) out << ", ";
	};
      }
      else {
	out << "#" << id();
      }
	
      if ( size() > 0 ) { out << ")"; };
    };

    SExpr& operator=(SExpr const& expr) {
      deque<SExpr *>::operator=(expr);
      _id = expr._id;
      return *this;
    };
  };

  class Dict;
  class Dict : public vector<SExpr *> {
    size_t next_id;
  public:
    Dict(Lexicon lex) : next_id(1) {};

    size_t index(SExpr *expr) {
      const_iterator it;
      FOR_EACH(it, *this) {
        if ( (**it)[1]->match(*expr) ) {
          return (*it)->id();
        }
      }
      SExpr *pair = new SExpr(next_id, SE_REF);
      pair->push_back(expr);
      push_back(pair);
      next_id += 1;
      return next_id - 1;
    };

    SExpr* deref(size_t index) {
      if ( index < 1 || index >= next_id ) {
	throw InvalidIndex();
      };
      if ( index > size() ) {
	throw ProgramError();
      }
      return (*this)[index-1];
    };
  };
};

#endif
