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
#include "tree_node_iterator.h"
#include "tree_node_factory.h"
#include "payload.h"
#include "tree_keys.h"

#include <iostream>

using namespace std;


TreeNodeIterator::TreeNodeIterator(
  TreeNodePtr a_node, mmap_file *a_mp)
  :
  mp(a_mp),
  payload_st(0, 0)
{
  init(a_node);
};

void TreeNodeIterator::init(TreeNodePtr a_node) 
{
  node = a_node;

  if ( ! a_node.is_null() ) {
    while ( 
      node.ptr(mp)->_payload.is_null() && 
      (
	!node.ptr(mp)->right().is_null() || 
	!node.ptr(mp)->right_thread().is_null()
      ) 
    ) 
    {
      if ( node.ptr(mp)->is_threaded_right() ) {
	node = node.ptr(mp)->right_thread();
      }
      else {
	node = node.ptr(mp)->right();
      }
    }
    
    if ( ! node.ptr(mp)->_payload.is_null() ) {
      payload_st.init(
	node.ptr(mp)->_payload.ptr(mp), 
	mmap_getkey(mp, PAYLOAD_SIZE_KEY)
      );
      decode_interval(payload_st, curr_interval);
      assign_eq(curr_tuple, curr_interval.start);
    }
    else {
      cerr << "Internal Error" << endl;
    }
  }
};
  
void TreeNodeIterator::next() 
{
  if ( cmp(curr_tuple, curr_interval.end) < 0 ) {
    plus_equals_one(curr_tuple);
  }
  else {
    decode_interval(payload_st, curr_interval);

    if ( ::is_null(curr_interval.start) ||  payload_st.overflow ) {
      if ( ! node.ptr(mp)->right().is_null() ) {
	TreeNodePtr curr = node.ptr(mp)->right();
	while ( ! curr.ptr(mp)->left().is_null() ) curr = curr.ptr(mp)->left();
	init(curr);
      }
      else if ( ! node.ptr(mp)->right_thread().is_null() ) {
	init(node.ptr(mp)->right_thread());
      }
      else {
	init(TreeNodePtr::null());
      }
    } 
    else {
      assign_eq(curr_tuple, curr_interval.start);
    };
  }
}
  
void TreeNodeIterator::prev()
{
  cerr << "Not implemented yet." << endl;
};
  
Tuple const& TreeNodeIterator::val() 
{
  return curr_tuple;
};

