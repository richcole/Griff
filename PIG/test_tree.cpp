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

  for(j=0;j<10;j++) {
    Tuple tuple = 
      {
	4, 3, 2, 1 
      };

    for(i=0;i<1024;++i) {
      tuple[i%4]++;
      tree.ptr(mp)->insert(mp, tuple);
      s.insert(tuple);
      if (! check_tree(tree, mp, s)) {
        abort();
      }
      if ( !recursive_check_level_invarient(mp, tree.ptr(mp)->_root)) {
        abort();
      }
    };
  };

  for(i=0;i<1024;++i) {
    Tuple tuple;
    for(k=0;k<4;++k) {
      tuple[k] = (rand() >> 2) % 100;
    }
    tree.ptr(mp)->insert(mp, tuple);
    s.insert(tuple);
    if (! check_tree(tree, mp, s)) {
      abort();
    }
    if (! recursive_check_level_invarient(mp, tree.ptr(mp)->_root)) {
      abort();
    }
  };

  TreeNodeIterator it = tree.ptr(mp)->begin(mp);
  while(! it.is_null() ) {
    Tuple const& t = it.val();
    cout << t[0] << " " << t[1] << " " << t[2] << " " << t[3] << endl;
    it.next();
  };

  mmap_print_stats(mp, cout);
  
};
