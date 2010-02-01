#ifndef TYPE_DECL_H
#define TYPE_DECL_H

#include <gc/gc_cpp.h>
#include "SExpr.h"
#include "Lexicon.h"

namespace RJL {

  using namespace std;

  inline void prune_levels(SExpr *expr, Lexicon& lex, bool prune_terminals = false);

  inline void prune_children(SExpr *expr, Lexicon& lex) {
    if ( expr == 0 ) return;
    if ( 
	(expr->id() == lex.index(":type") || expr->id() == lex.index("fun-type")) ||
	expr->size() > 1 
    ) {
      SExpr::const_iterator it;
      FOR_EACH(it, *expr) {
	prune_levels(*it, lex, true);
      };
    };
  };

  inline void prune_levels(SExpr *expr, Lexicon& lex, bool prune_terminals) {
    if ( expr == 0 ) return;
    while ( 
      expr->size() == 1 && 
      ( prune_terminals || ((*expr)[0]->id() == lex.index(":type")) ||
        ((*expr)[0]->id() == lex.index(":fun-type")) )
    ) *expr = *((*expr)[0]);
    prune_children(expr, lex);
  };

}; // namespace RJL

#endif
