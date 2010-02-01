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

#include <fstream>

#include "std_tree_helper.h"

int main(int argc, char **argv) 
{
  TreePtr tree;

  int        fd = -1;
  mmap_file* mp = 0;

  /* Load the tree file */
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
  
  int i;

  /* load the input file */
  Tuple tuple_array[1024*32];
  int   tuple_array_len = 1024*32;
  int   num_tuples = 0;

  {
    std::ifstream input(argv[1]);
    while (num_tuples < tuple_array_len && input.good()) {
      for(i=0;i<4;++i) {
        input >> tuple_array[num_tuples][i];
      }
      if ( ! input.fail() ) {
        ++num_tuples;
      }
    }
  }

  /* insert each tuple into the tree */
  for(i=0;i<num_tuples;++i) {
    tree.ptr(mp)->insert(mp, tuple_array[i]);
  };

  /* search for the entry which is apparently missing */
  Tuple t = { 4, 18, 1583, 1014 };
  TreeNodeIterator it = tree.ptr(mp)->first_gte(t, mp);
  if ( it.is_null() ) {
    cerr << "Error, first_gte returned null." << endl;
  } else {
    if ( cmp(it.val(), t) != 0 ) {
      cerr << "Error, returned different value." << endl;
    }
  }

  mmap_print_stats(mp, cout);
};
