namespace RJL {

  using namespace std;

  enum SExprType { SE_IDENT = 0x0, SE_REF = 0x1 };

  class SExpr;
  class SExpr : public vector<SExpr *> {
    size_t _id;
  public:
    SExpr(size_t id, SExprType type) { 
      _id = (id << 0x1) | (type & 0x1);
    };
    size_t id() const { return _id >> 0x1; };
    size_t type() const { return _id & 0x1; };

    void print(ostream& out, Lexicon const& lex) {
      out << "(";
      if ( type() == SE_IDENT ) {
	out << lex.word(id());
	const_iterator it;
	FOR_EACH(it, *this) {
	  it->print(out, lex);
	};
      }
      else {
	out << "#" << id();
      }
	
      out << ")";
    };
  };

  class Dict;
  class Dict : private vector<SExpr *> {
    size_t next_id;
  public:
    Dict(Lexicon lex) next_id(1) {};

    size_t index(SExpr *expr) {
      SExpr::const_iterator it;
      FOR_EACH(it, *this) {
        if ( (*it)[1]->match(expr) ) {
          return it->id();
        }
      }
      SExpr *pair = new SExpr(next_id, SE_REF);
      pair.push_back(expr);
      push_back(pair);
      next_id += 1;
      return next_id - 1;
    };

    SExpr* deref(size_t index) {
      if ( index < 1 || index >= _next_index ) {
	throw InvalidIndex();
      };
      if ( index > size() ) {
	throw ProgramError();
      }
      return (*this)[index-1][0];
    };
  };
};
