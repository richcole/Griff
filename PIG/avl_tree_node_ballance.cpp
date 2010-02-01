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
#include "tree_node.h"
#include "tree_node_factory.h"
#include "payload.h"

inline size_t rank(mmap_file *mp, TreeNodePtr n)
{
  return n.is_null() ? 0 : n.ptr(mp)->rank();
};

size_t determine_rank(mmap_file *mp, TreeNodePtr n)
{
  if (n.is_null()) {
    return 0;
  }
  else {
    size_t rank_l = rank(mp, n.ptr(mp)->left());
    size_t rank_r = rank(mp, n.ptr(mp)->right());
    size_t r   = (rank_l > rank_r ? rank_l : rank_r) + 1;
    n.ptr(mp)->set_rank(r);
    return r;
  };
};

inline TreeNodePtr rotate_left(mmap_file *mp, TreeNodePtr n) 
{
  if (
    !n.is_null() && 
    !n.ptr(mp)->right().is_null() && 
    !n.ptr(mp)->right().ptr(mp)->right().is_null()
  ) {
    TreeNodePtr t  = n.ptr(mp)->right();
    TreeNodePtr l  = n;
    TreeNodePtr r  = n.ptr(mp)->right().ptr(mp)->right();
    TreeNodePtr lr = n.ptr(mp)->right().ptr(mp)->left();
    
    if ( lr.is_null() ) {
      l.ptr(mp)->set_right_thread(t);
    }
    else {
      l.ptr(mp)->set_right(lr);
    };
    
    t.ptr(mp)->set_left(l);
    t.ptr(mp)->set_right(r);
    return t;
  }
  else {
    return n;
  };
};

inline TreeNodePtr rotate_right(mmap_file *mp, TreeNodePtr n) 
{
  if (
    !n.is_null() && 
    !n.ptr(mp)->left().is_null() && 
    !n.ptr(mp)->left().ptr(mp)->left().is_null()
  ) {
    TreeNodePtr t  = n.ptr(mp)->left();
    TreeNodePtr l  = n.ptr(mp)->left().ptr(mp)->left();
    TreeNodePtr r  = n;
    TreeNodePtr rl = n.ptr(mp)->left().ptr(mp)->right();
    
    if ( rl.is_null() ) {
      r.ptr(mp)->set_left_thread(t);
    }
    else {
      r.ptr(mp)->set_left(rl);
    }

    t.ptr(mp)->set_left(l);
    t.ptr(mp)->set_right(r);
    return t;
  }
  else {
    return n;
  };
};

TreeNodePtr ballance(mmap_file *mp, TreeNodePtr n)
{
  if ( n.is_null() ) abort();
  
  size_t r_l = rank(mp, n.ptr(mp)->left());
  size_t r_r = rank(mp, n.ptr(mp)->right());

  if ( r_l + 1 < r_r ) {
    size_t r_rl = 
      (!n.ptr(mp)->right().ptr(mp)->left().is_null()) 
      ? n.ptr(mp)->right().ptr(mp)->left().ptr(mp)->rank() 
      : 0;
    size_t r_rr = (!n.ptr(mp)->right().ptr(mp)->right().is_null()) ? n.ptr(mp)->right().ptr(mp)->right().ptr(mp)->rank() : 0;
    if (r_rl > r_rr) 
      n.ptr(mp)->set_right(rotate_right(mp, n.ptr(mp)->right()));
    n = rotate_left(mp, n);
    determine_rank(mp, n.ptr(mp)->left());
    determine_rank(mp, n.ptr(mp)->right());
  }
  else if ( r_r + 1 < r_l ) {
    size_t r_ll = (!n.ptr(mp)->left().ptr(mp)->left().is_null()) ? n.ptr(mp)->left().ptr(mp)->left().ptr(mp)->rank() : 0;
    size_t r_lr = (!n.ptr(mp)->left().ptr(mp)->right().is_null()) ? n.ptr(mp)->left().ptr(mp)->right().ptr(mp)->rank() : 0;
    if (r_ll < r_lr) 
      n.ptr(mp)->set_left(rotate_left(mp, n.ptr(mp)->left()));
    n = rotate_right(mp, n);
    determine_rank(mp, n.ptr(mp)->left());
    determine_rank(mp, n.ptr(mp)->right());
  }

  determine_rank(mp, n);
  return n;
};
