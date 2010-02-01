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

#include "tree_node.h"
#include "tree_node.h"
#include "tree_keys.h"
#include "payload.h"

#include <vector>

TreeNodePtr ballance(mmap_file* mp, TreeNodePtr node);
size_t determine_rank(mmap_file *mp, TreeNodePtr n);

void TreeNode::insert_left(mmap_file* mp, TreeNodePtr new_left)
{
  assert( left().is_null() );
  
  new_left.ptr(mp)->set_left_thread(left_thread());
  new_left.ptr(mp)->set_right_thread(TreeNodePtr::recover(mp, this));
  set_left(new_left);
};

TreeNodePtr TreeNode::insert_left_most(mmap_file* mp, TreeNodePtr new_node)
{
  if ( ! left().is_null() ) {
    set_left(left().ptr(mp)->insert_left_most(mp, new_node));
    return ballance(mp, TreeNodePtr::recover(mp, this));
  }
  else {
    new_node.ptr(mp)->set_left_thread(left_thread());
    new_node.ptr(mp)->set_right_thread(TreeNodePtr::recover(mp, this));
    set_left(new_node);
    return TreeNodePtr::recover(mp, this);
  };
};

void TreeNode::insert_right(mmap_file* mp, TreeNodePtr new_right)
{
  if ( right().is_null() ) {
    new_right.ptr(mp)->set_right_thread(right_thread());
    new_right.ptr(mp)->set_left_thread(TreeNodePtr::recover(mp, this));
    set_right(new_right);
  }
  else {
    set_right(right().ptr(mp)->insert_left_most(mp, new_right));
  }
};

void TreeNode::insert_right(mmap_file* mp, BytePtr src_payload, Interval& range)
{
  // find the leftmost leaf node to the right
  if ( right().is_null() ) {
    // create a new node containing the payload
    TreeNodePtr new_right = TreeNodePtr::alloc(mp);
    new_right.ptr(mp)->init(mp);
    new_right.ptr(mp)->_payload = src_payload;
    new_right.ptr(mp)->set_right_thread(right_thread());
    new_right.ptr(mp)->set_left_thread(TreeNodePtr::recover(mp, this));
    assign_eq(new_right.ptr(mp)->interval, range);
    set_right(new_right);
    determine_rank(mp, TreeNodePtr::recover(mp, this));
  }
  else {
    right().ptr(mp)->insert_left(mp, src_payload, range);
  }
};

void TreeNode::insert_left(
  mmap_file* mp, BytePtr value_payload, Interval& range)
{
  if ( left().is_null() ) {

    if ( right().is_null() ) {
      // try to insert into this node
      
      size_t payload_size = mmap_getkey(mp, PAYLOAD_SIZE_KEY);
      BytePtr dest_payload = BytePtr::alloc(mp, payload_size);
      BytePtr overflow_payload = BytePtr::alloc(mp, payload_size);
      
      memset(dest_payload.ptr(mp), 0, payload_size);
      memset(overflow_payload.ptr(mp), 0, payload_size);
      
      PayloadStream dest_st(dest_payload.ptr(mp), payload_size);
      PayloadStream overflow_st(overflow_payload.ptr(mp), payload_size);
      PayloadStream src_st(_payload.ptr(mp), payload_size);
      PayloadStream value_st(value_payload.ptr(mp), payload_size);
      
      payload_insert(dest_st, src_st, value_st, overflow_st);
      assign_eq(interval, dest_st.range);
      
      _payload.free(mp);
      _payload = dest_payload;
      
      value_payload.free(mp);
      
      if ( dest_st.overflow ) {
	TreeNodePtr new_right = TreeNodePtr::alloc(mp);
	new_right.ptr(mp)->init(mp);
	new_right.ptr(mp)->_payload = overflow_payload;
	new_right.ptr(mp)->set_right_thread(right_thread());
	new_right.ptr(mp)->set_left_thread(TreeNodePtr::recover(mp, this));
	assign_eq(new_right.ptr(mp)->interval, overflow_st.range);
	set_right(new_right);
	determine_rank(mp, TreeNodePtr::recover(mp, this));
      }
      else {
	overflow_payload.free(mp);
      }
    }
    else {
      // node is interior create a new left node
      TreeNodePtr new_left = TreeNodePtr::alloc(mp);
      new_left.ptr(mp)->init(mp);
      new_left.ptr(mp)->_payload = value_payload;
      new_left.ptr(mp)->set_right_thread(TreeNodePtr::recover(mp, this));
      new_left.ptr(mp)->set_left_thread(left_thread());
      assign_eq(new_left.ptr(mp)->interval, range);
      set_left(new_left);
      determine_rank(mp, TreeNodePtr::recover(mp, this));
    }
  }
  else {
    left().ptr(mp)->insert_left(mp, value_payload, range);
    determine_rank(mp, TreeNodePtr::recover(mp, this));
    set_left(ballance(mp, left()));
  }
};

TreeNodePtr TreeNode::remove(mmap_file *mp, Tuple const& x, bool& remove_node)
{
  if ( _payload.is_null() ) {
    assert(left().is_null());
    assert(right().is_null());
  }
  else {
    int le_start = cmp(x, interval.start);
    int le_end = cmp(x, interval.end);

    if ( le_start < 0 ) {
      if (! left().is_null() ) {
	TreeNodePtr new_left = left().ptr(mp)->remove(mp, x, remove_node);
        if ( remove_node ) {
          if ( 
              new_left.ptr(mp)->left().is_null() && 
              new_left.ptr(mp)->right().is_null()
            ) 
          {
            set_left_thread(new_left.ptr(mp)->left_thread());
            new_left.ptr(mp)->clear(mp);
            new_left.free(mp);
            remove_node = false;
          }
          else {
            TreeNodePtr victim;
            TreeNodePtr replacee = left();
            new_left = new_left.ptr(mp)->find_victim(mp, victim);
            if ( ! victim.is_null() ) {
              assign_eq(replacee.ptr(mp)->interval, victim.ptr(mp)->interval);
              replacee.ptr(mp)->_payload.free(mp);
              replacee.ptr(mp)->_payload = victim.ptr(mp)->_payload;
              victim.free(mp);
            } else {
              replacee.ptr(mp)->_payload.free(mp);
              replacee.free(mp);
            }
            set_left(new_left);
            remove_node = false;
          }
        }
        else {
          set_left(new_left);
        }
      }
    }
    else if ( le_end > 0 ) {
      if (! right().is_null() ) {
	TreeNodePtr new_right = right().ptr(mp)->remove(mp, x, remove_node);
        if ( remove_node ) {
          if ( 
              new_right.ptr(mp)->left().is_null() && 
              new_right.ptr(mp)->right().is_null()
            ) 
          {
            set_right_thread(new_right.ptr(mp)->right_thread());
            new_right.ptr(mp)->clear(mp);
            new_right.free(mp);
            remove_node = false;
          }
          else {
            TreeNodePtr victim;
            TreeNodePtr replacee = right();
            new_right = new_right.ptr(mp)->find_victim(mp, victim);
            if ( ! victim.is_null() ) {
              assign_eq(replacee.ptr(mp)->interval, victim.ptr(mp)->interval);
              replacee.ptr(mp)->_payload.free(mp);
              replacee.ptr(mp)->_payload = victim.ptr(mp)->_payload;
              victim.free(mp);
            } else {
              replacee.ptr(mp)->_payload.free(mp);
              replacee.free(mp);
            }
            set_right(new_right);
            remove_node = false;
          }
        }
        else {
          set_right(new_right);
        }
      }
    }
    else {
      
      size_t payload_size = mmap_getkey(mp, PAYLOAD_SIZE_KEY);
      BytePtr dest_payload = BytePtr::alloc(mp, payload_size);
      BytePtr overflow_payload = BytePtr::alloc(mp, payload_size);

      memset(dest_payload.ptr(mp), 0, payload_size);
      memset(overflow_payload.ptr(mp), 0, payload_size);

      PayloadStream dest_st(dest_payload.ptr(mp), payload_size);
      PayloadStream overflow_st(overflow_payload.ptr(mp), payload_size);
      PayloadStream src_st(_payload.ptr(mp), payload_size);
    
      payload_remove(dest_st, src_st, x, overflow_st);

      if ( dest_st.offset <= 1 ) {
        remove_node = true;
      }

      _payload.free(mp);
      _payload = dest_payload;

      assert( ! dest_st.overflow );
      overflow_payload.free(mp);
    };

  };

  determine_rank(mp, TreeNodePtr::recover(mp, this));
  return ballance(mp, TreeNodePtr::recover(mp, this));
};

TreeNodePtr TreeNode::insert(mmap_file *mp, Tuple const& x)
{
  if ( _payload.is_null() ) {
    size_t payload_size = mmap_getkey(mp, PAYLOAD_SIZE_KEY);
    _payload = BytePtr::alloc(mp, payload_size);
    memset(_payload.ptr(mp), 0, payload_size);
    PayloadStream _payload_st(_payload.ptr(mp), payload_size);
    payload_write_tuple(_payload_st, x);
    assign_eq(interval.start, x);
    assign_eq(interval.end, x);
  }
  else {

    int le_start = cmp(x, interval.start);
    int le_end = cmp(x, interval.end);

    if ( ! left().is_null() && le_start < 0 ) {  
      // insertion into left subtree
      set_left(left().ptr(mp)->insert(mp, x));
    }
    else if ( !right().is_null() && le_end > 0 ) { 
      // insertion into right subtree
      set_right(right().ptr(mp)->insert(mp, x));
    }
    else if ( is_full() && le_start < 0 ) {
      assert( left().is_null());
      TreeNodePtr new_left = TreeNodePtr::alloc(mp);
      new_left.ptr(mp)->init(mp);
      insert_left(mp, new_left);
      set_left(left().ptr(mp)->insert(mp, x));
    }
    else if ( is_full() && le_end > 0 ) {
      assert(right().is_null());
      TreeNodePtr new_right = TreeNodePtr::alloc(mp);
      new_right.ptr(mp)->init(mp);
      insert_right(mp, new_right);
      set_right(right().ptr(mp)->insert(mp, x));
    }
    else {

      size_t payload_size = mmap_getkey(mp, PAYLOAD_SIZE_KEY);
      BytePtr dest_payload = BytePtr::alloc(mp, payload_size);
      BytePtr overflow_payload = BytePtr::alloc(mp, payload_size);

      memset(dest_payload.ptr(mp), 0, payload_size);
      memset(overflow_payload.ptr(mp), 0, payload_size);

      PayloadStream dest_st(dest_payload.ptr(mp), payload_size);
      PayloadStream overflow_st(overflow_payload.ptr(mp), payload_size);
      PayloadStream src_st(_payload.ptr(mp), payload_size);
    
      payload_insert(dest_st, src_st, x, overflow_st);
      assign_eq(interval, dest_st.range);

      _payload.free(mp);
      _payload = dest_payload;

      if ( dest_st.overflow ) {
	insert_right(mp, overflow_payload, overflow_st.range);
      }
      else {
        overflow_payload.free(mp);
      };
    }
  };
  
  return ballance(mp, TreeNodePtr::recover(mp, this));
};

TreeNodePtr TreeNode::find_victim(mmap_file* mp, TreeNodePtr& victim)
{
  if ( right().is_null() ) {
    victim = TreeNodePtr::null();
    if ( ! left().is_null() ) { // fix up threads pointing to the victim
      TreeNodePtr curr = left();
      while ( (! curr.ptr(mp)->right().is_null()) && ! curr.ptr(mp)->is_threaded_right() ) {
        curr = curr.ptr(mp)->right();
      }
      if ( curr.ptr(mp)->is_threaded_right() ) {
        curr.ptr(mp)->set_right_thread(right_thread());
      }
    }
    return left();
  }
  else {
    TreeNodePtr new_right = right().ptr(mp)->find_victim_left(mp, victim);
    if ( new_right.is_null() ) {
      set_right_thread(right().ptr(mp)->right_thread());
    } 
    else {
      set_right(new_right);
    }
    determine_rank(mp, TreeNodePtr::recover(mp, this));
    return ballance(mp, TreeNodePtr::recover(mp, this));
  }
}

TreeNodePtr TreeNode::find_victim_left(mmap_file* mp, TreeNodePtr& victim)
{
  if ( left().is_null() ) {
    victim = TreeNodePtr::recover(mp, this);
    if ( ! right().is_null() ) { // fix up threads pointing to the victim
      TreeNodePtr curr = right();
      while ( (! curr.ptr(mp)->left().is_null()) && ! curr.ptr(mp)->is_threaded_left() ) {
        curr = curr.ptr(mp)->left();
      }
      if ( curr.ptr(mp)->is_threaded_left() ) {
        curr.ptr(mp)->set_left_thread(victim.ptr(mp)->left_thread());
      }
    }
    return right();
  }
  else {
    TreeNodePtr new_left = left().ptr(mp)->find_victim_left(mp, victim);
    if ( new_left.is_null() ) {
      if ( ! left().ptr(mp)->right().is_null() ) {
        set_left(left().ptr(mp)->right());
      }
      else {
        set_left_thread(left().ptr(mp)->left_thread());
      }
    } 
    else {
      set_left(new_left);
    }
    determine_rank(mp, TreeNodePtr::recover(mp, this));
    return ballance(mp, TreeNodePtr::recover(mp, this));
  }
}

void TreeNode::init(mmap_file *mp)
{
  _left = _right = TreeNodePtr::null();
  _payload = BytePtr::null();

  flags = 0;
  set_rank(1);
  interval.clear();
};

    
void TreeNode::print(mmap_file* mp, std::ostream& out, int level) 
{
  int i;
  out << "index=" << TreeNodePtr::recover(mp, this).get_index();
  out << ", interval=";
  write_interval(out, interval);
  out << std::endl;
  
  for ( i=0; i<level*2; ++i) out << " ";
  if ( left().is_null() ) {
    out << "Left (thread): " << left_thread().get_index() << std::endl;
  }
  else {
    out << "Left: "; left().ptr(mp)->print(mp, out, level+1);
  }
  
  for ( i=0; i<level*2; ++i) out << " ";
  if ( right().is_null() ) {
    out << "Right (thread): " << right_thread().get_index() << std::endl;
  }
  else {
    out << "Right: "; right().ptr(mp)->print(mp, out, level+1);
  }
};

size_t recursive_determine_rank(mmap_file *mp, TreeNodePtr n)
{
  if ( n.is_null() ) {
    return 0;
  }
  else {
    size_t l = recursive_determine_rank(mp, n.ptr(mp)->left());
    size_t r = recursive_determine_rank(mp, n.ptr(mp)->right());
    size_t v = (l > r ? l : r);
    return v+1;
  }
};

inline size_t rank(mmap_file *mp, TreeNodePtr n)
{
  return n.is_null() ? 0 : n.ptr(mp)->rank();
};

bool recursive_check_level_invarient(
  mmap_file *mp, 
  TreeNodePtr n)
{
  if (n.is_null()) {
    return true;
  }
  else {
    size_t rn = recursive_determine_rank(mp, n);
    if ( n.ptr(mp)->rank() != rn ) {
      return false;
    }
    size_t rl = rank(mp, n.ptr(mp)->left());
    size_t rr = rank(mp, n.ptr(mp)->right());
    if ( abs(rl - rr) > 2) {
      return false;
    }
    return 
      recursive_check_level_invarient(mp, n.ptr(mp)->left()) &&
      recursive_check_level_invarient(mp, n.ptr(mp)->right());
  }
};

bool check_threading(mmap_file *mp, TreeNodePtr n, NodeEntryStack& stack)
{
  TreeNodePtr left_thread = n.ptr(mp)->left_thread();
  TreeNodePtr right_thread = n.ptr(mp)->right_thread();
  int i;

  if ( ! left_thread.is_null() ) {
    for(
        i = stack.size() - 1; 
        i > 0 && stack[i].dir == NodeEntryPair::left; 
        --i) ;
    if ( i > 0 ) {
      if ( left_thread.operator !=(stack[i-1].ptr) ) {
        return false;
      }
    }
  }

  if ( ! right_thread.is_null() ) {
    for(
        i = stack.size() - 1; 
        i > 0 && stack[i].dir == NodeEntryPair::right; 
        --i) ;
    if ( i > 0 ) {
      if ( right_thread != stack[i-1].ptr ) {
        return false;
      }
    }
  }

  // recursive left
  if ( ! n.ptr(mp)->left().is_null() ) {
    stack.push_back(NodeEntryPair(n.ptr(mp)->left(), NodeEntryPair::left));
    if ( ! check_threading(mp, n.ptr(mp)->left(), stack) ) {
      return false;
    }
    stack.pop_back();
  }

  // recurse right
  if ( ! n.ptr(mp)->right().is_null() ) {
    stack.push_back(NodeEntryPair(n.ptr(mp)->right(), NodeEntryPair::right));
    if ( ! check_threading(mp, n.ptr(mp)->right(), stack) ) {
      return false;
    }
    stack.pop_back();
  }

  return true;
};
    
  
  
