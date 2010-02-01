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
#include "persistent_tree.h"
#include "tree_node_persistent_factory.h"

#include <iostream>

using namespace std;


Tuple tuples[] = {
 { 2,1,1,1 }, 
 { 2,1,1,2 }, 
 { 1,1,1,2 }, 
 { 1,1,1,4 },
 { 1,1,1,3 }
};

int main() 
{
  PersistentTree tree("test.tree");

  Tuple tuple = 
    {
      4, 3, 2, 1 
    };

  int i, k;
  
  for(i=0;i<1024;++i) {
    for(k=0;k<4;++k) {
      tuple[k] = (rand() >> 2) % 100;
    }
    tree.insert(tuple);
  };

  TreeNodeIterator it = tree.begin();
  while(! it.is_null() ) {
    Tuple const& t = it.val();
    cout << t[0] << " " << t[1] << " " << t[2] << " " << t[3] << endl;
    it.next();
  };
};
