/* 
Copyright 2004, Richard Cole

This file is part of Griff.

Griff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.

Griff is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Griff; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "tree.h"
#include "tree_node_malloc_factory.h"
#include "tree_keys.h"

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

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


mmap_file* create_mmap_file()
{
  mmap_file *mp = 0;
  int fd = open("test.tree", O_RDWR, 0666);
  if ( fd == -1 ) {
    fd = open("test.tree", O_RDWR | O_CREAT, 0666);

    mp = mmap_init(fd);
    mmap_setkey(mp, PAYLOAD_SIZE_KEY, 256);
    TreePtr tree;
    tree = TreePtr::alloc(mp);
    tree.ptr(mp)->_root = TreeNodePtr::null();
    tree.setkey(mp, TREE_KEY);
  }
  else {
    mp = mmap_init(fd);
  }

  return mp;
};


int main() 
{
  TreePtr tree;

  mmap_file* mp = 0;

  unlink("test.tree");
  mp=create_mmap_file();
  tree = TreePtr::getkey(mp, TREE_KEY);
  
  Set s;
  
  int i, k = 0;

  srand(128);
  for(i=0;i<1024 * 12;++i) {
    Tuple tuple;
    for(k=0;k<4;++k) {
      tuple[k] = (rand() >> 2) % 100;
    }
    tree.ptr(mp)->insert(mp, tuple);
    s.insert(tuple);
  };

  assert(check_tree(tree, mp, s));
  assert(recursive_check_level_invarient(mp, tree.ptr(mp)->_root));

  mmap_finalize(mp);
  
  mp=create_mmap_file();
  tree = TreePtr::getkey(mp, TREE_KEY);
  
  assert(check_tree(tree, mp, s));
  assert(recursive_check_level_invarient(mp, tree.ptr(mp)->_root));

  mmap_print_stats(mp, cout);

  mmap_finalize(mp);
};
