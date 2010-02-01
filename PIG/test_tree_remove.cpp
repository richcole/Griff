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

#include "std_tree_helper.h"

#define LENGTH 24*1024

int main() 
{
  TreePtr tree;

  int        fd = -1;
  mmap_file* mp = 0;

  unlink("test.tree");
  fd = open("test.tree", O_RDWR, 0666);
  if ( fd == -1 ) {
    fd = open("test.tree", O_RDWR | O_CREAT, 0666);

    mp = mmap_init(fd);
    mmap_setkey(mp, PAYLOAD_SIZE_KEY, 1024);
    tree = TreePtr::alloc(mp);
    tree.ptr(mp)->_root = TreeNodePtr::null();
    tree.setkey(mp, TREE_KEY);
  }
  else {
    mp = mmap_init(fd);
    tree = TreePtr::getkey(mp, TREE_KEY);
  };
  
  Set s;
  
  int i, k, j = 0;

  Tuple tuples[LENGTH];

  for(i=0;i<LENGTH;++i) {
    Tuple tuple;
    for(k=0;k<4;++k) {
      tuple[k] = (rand() >> 2) % 100;
    }
    tree.ptr(mp)->insert(mp, tuple);
    s.insert(tuple);
    assign_eq(tuples[i], tuple);
    if ( i % 3 == 0 ) {
      tree.ptr(mp)->remove(mp, tuples[i/3]);
      s.erase(tuples[i/3]);
    };
    if ( ! check_tree(tree, mp, s) ) {
      abort();
    }
    if ( ! recursive_check_level_invarient(mp, tree.ptr(mp)->_root) ) {
      abort();
    }
    if ( ! check_threading(mp, tree) ) {
      abort();
    }
  };

  int count = 0;
  Set::const_iterator it;
  for(it=s.begin(); it!=s.end(); it=s.begin()) {
    ++count;
    int k = (rand() >> 2) % s.size();
    while(k > 0) {
      ++it; --k;
    }
    //    if ( count == 8164 ) {
    //      tree.ptr(mp)->print(mp, cout);
    //    }
    tree.ptr(mp)->remove(mp, it->t);
    s.erase(*it);
    //    if ( count == 8164 ) {
    //      tree.ptr(mp)->print(mp, cout);
    //    }
    if ( ! check_tree(tree, mp, s) ) {
      abort();
    }
    if ( ! recursive_check_level_invarient(mp, tree.ptr(mp)->_root) ) {
      abort();
    }
    if ( ! check_threading(mp, tree) ) {
      abort();
    }
  }
};
