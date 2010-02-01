#ifndef STD_TREE_HELPER_H
#define STD_TREE_HELPER_H

#include <iostream>
#include <set>

using namespace std;

int count(Tree& tree, mmap_file *fac)
{
  int count = 0;
  TreeNodeIterator it = tree.begin(fac);
  while(! it.is_null() ) {
    ++count;
    it.next();
  };

  return count;
};

class TupleWrap 
{
public:
  Tuple t;

  TupleWrap(Tuple const& a_t) 
  {
    assign_eq(t, a_t);
  };

  TupleWrap& operator=(TupleWrap const& a_t) 
  {
    assign_eq(t, a_t.t);
    return *this;
  };
  
  TupleWrap(TupleWrap const& a_t) 
  {
    *this = a_t;
  };
};

class leq {
public:
  bool operator ()(TupleWrap const& s, TupleWrap const& t)
  {
    return cmp(s.t,t.t) < 0;
  };
};

typedef std::set<TupleWrap,leq> Set;

bool check_tree(TreePtr tree, mmap_file *mp, Set& s)
{
  int count = 0;
  TreeNodeIterator it_t = tree.ptr(mp)->begin(mp);
  Set::const_iterator it_s = s.begin();
  while( (! it_t.is_null()) && it_s != s.end()) {
    if ( cmp(it_t.val(),it_s->t) != 0 ) {
      return false;
    }
    ++count;
    
    it_t.next(); ++it_s;
  };

  if ( it_t.is_null() != (it_s == s.end()) ) {
    return false;
  };

  return true;
};


#endif
