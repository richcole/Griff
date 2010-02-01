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
/*
 * It isn't clear whether the nodes need to store the maximum value.
 *
 * It isn't clear that prefix compression is currently working on the first
 * element of each interval.
 */

#include "tree.h"
#include "tree_keys.h"

void Tree::insert(mmap_file *mp, Tuple const& x)
{
  if ( _root.is_null() ) {
    _root = TreeNodePtr::alloc(mp);
    _root.ref(mp).init(mp);
    _root.ref(mp)._payload = 
      BytePtr::alloc(mp, mmap_getkey(mp, PAYLOAD_SIZE_KEY));
    memset(_root.ref(mp)._payload.ptr(mp), 
      0, mmap_getkey(mp, PAYLOAD_SIZE_KEY));
  }

  _root = _root.ptr(mp)->insert(mp, x);
  _root.ptr(mp)->set_black(true);
};

void Tree::remove(mmap_file *mp, Tuple const& x)
{
  bool remove_node = false;
  if ( ! _root.is_null() ) { 
    TreeNodePtr new_root;
    new_root = _root.ptr(mp)->remove(mp, x, remove_node);
    if ( remove_node ) {
      if ( 
          new_root.ptr(mp)->left().is_null() && 
          new_root.ptr(mp)->right().is_null()
          ) 
        {
          new_root.ptr(mp)->clear(mp);
          new_root.free(mp);
          new_root = TreeNodePtr::null();
        }
      else {
        TreeNodePtr victim;
        TreeNodePtr replacee = _root;
        new_root = new_root.ptr(mp)->find_victim(mp, victim);
        assign_eq(replacee.ptr(mp)->interval, victim.ptr(mp)->interval);
        replacee.ptr(mp)->_payload.free(mp);
        replacee.ptr(mp)->_payload = victim.ptr(mp)->_payload;
        victim.free(mp);
        remove_node = false;
      }
    }
    _root = new_root;
  };
};

TreeNodeIterator Tree::begin(mmap_file *mp)
{
  TreeNodePtr curr = _root;
  while( ! curr.is_null() && ! curr.ptr(mp)->left().is_null() ) {
    curr = curr.ptr(mp)->left();
  }
  return TreeNodeIterator(curr, mp);
};

TreeNodeIterator Tree::end(mmap_file *mp)
{
  TreeNodePtr curr = _root;
  while( ! curr.is_null() && ! curr.ptr(mp)->right().is_null() ) {
    curr = curr.ptr(mp)->right();
  }
  return TreeNodeIterator(curr, mp);
};


TreeNodePtr first_gte(TreeNodePtr n, Tuple& x, mmap_file *mp)
{
  if ( n.is_null() ) {
      return n;
  };
  
  int cmp_end = cmp(x, n.ptr(mp)->interval.end);

  if ( cmp_end > 0 ) {
    return first_gte(n.ptr(mp)->right(), x, mp);
  }
  else {
      int cmp_start = cmp(x, n.ptr(mp)->interval.start);
      if ( cmp_start >= 0 ) {
          return n;
      } else {
	  TreeNodePtr l = first_gte(n.ptr(mp)->left(), x, mp);
	  if ( l.is_null() ) {
	      return n;
	  }
	  else {
	      return l;
	  }
      }
  }
};

TreeNodeIterator Tree::first_gte(Tuple& x, mmap_file *mp)
{
  TreeNodePtr n = ::first_gte(_root, x, mp);
  TreeNodeIterator it = TreeNodeIterator(n, mp);
  
  while(! it.is_null() && (cmp(it.val(), x) < 0)) {
    it.next();
  };
  return it;
};

void tree_init(mmap_file *mp)
{
  if ( mmap_getkey(mp, PAYLOAD_SIZE_KEY) == 0 ) {
    mmap_setkey(mp, PAYLOAD_SIZE_KEY, 64);
    TreePtr tree = TreePtr::alloc(mp);
    tree.ptr(mp)->_root = TreeNodePtr::null();
    tree.setkey(mp, TREE_KEY);
  }
};

TreePtr tree_get_tree(mmap_file *mp)
{
  return TreePtr::getkey(mp, TREE_KEY);
};

bool check_threading(mmap_file *mp, TreePtr tree)
{
  if (tree.ptr(mp)->_root.is_null()) {
    return true;
  }

  NodeEntryStack stack;
  return check_threading(mp, tree.ptr(mp)->_root, stack);
}
