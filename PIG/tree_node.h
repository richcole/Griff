#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <sys/types.h>
#include "mmap_alloc.h"
#include "interval.h"

#include <vector>

/* Pointer values: 
 *
 *   Pointer values are relative to the this pointer and can be
 *   converted to absolute values via the this pointer.
 *
 */

struct TreeNode;

typedef mmap_ptr<TreeNode> TreeNodePtr;
typedef mmap_ptr<Byte>     BytePtr;

/*! TreeNode 
 *
 * Stored sepearatetly to the payload
 */

struct TreeNode {
  TreeNodePtr _left, _right;
  BytePtr     _payload;

  size_t   flags;
  Interval interval; // from start to end

  void init(mmap_file *mp);

  TreeNodePtr left() const {
    if ( is_threaded_left() ) {
      return TreeNodePtr::null();
    }
    return _left;
  }

  TreeNodePtr right() const {
    if ( is_threaded_right() ) {
      return TreeNodePtr::null();
    }
    return _right;
  }

  TreeNodePtr left_thread() const {
    if ( ! is_threaded_left() ) {
      return TreeNodePtr::null();
    }
    return _left;
  }

  TreeNodePtr right_thread() const {
    if ( ! is_threaded_right() ) {
      return TreeNodePtr::null();
    }
    return _right;
  }

  void set_left(TreeNodePtr ptr) {
    if ( ptr != _left ) _left = ptr;
    set_threaded_left(false);
  };

  void set_right(TreeNodePtr ptr) {
    if ( ptr != _right ) _right = ptr;
    set_threaded_right(false);
  }

  void set_right_thread(TreeNodePtr ptr) {
    if ( ptr != _right ) _right = ptr;
    set_threaded_right(true);
  }

  void set_left_thread(TreeNodePtr ptr) {
    if ( ptr != _left ) _left = ptr;
    set_threaded_left(true);
  };

  void set_threaded_left(bool is_threaded) {
    set_flag(is_threaded, 0x1);
  };

  void set_threaded_right(bool is_threaded) {
    set_flag(is_threaded, 0x2);
  };

  void set_full(bool is_full) {
    set_flag(is_full, 0x4);
  };

  void set_black(bool is_black) {
    set_flag(is_black, 0x8);
  };

  bool is_threaded_left() const {
    return flags & 0x1;
  };

  bool is_threaded_right() const {
    return flags & 0x2;
  };

  bool is_full() const {
    return flags & 0x4;
  };

  bool is_black() const {
    return flags & 0x8;
  };

  void set_rank(size_t rank) 
  {
    flags &= 0x8 | 0x4 | 0x2 | 0x1;
    flags |= rank << 4;
  };

  size_t rank() const 
  {
    return flags >> 4;
  };

  void        insert_left(mmap_file* mp, TreeNodePtr new_left);
  void        insert_right(mmap_file* mp, TreeNodePtr new_right);
  TreeNodePtr insert_left_most(mmap_file* mp, TreeNodePtr new_node);

  void insert_right(mmap_file* mp, BytePtr src_payload, Interval& range);
  void insert_left(mmap_file* mp, BytePtr src_payload, Interval& range);
  
  void set_flag(bool is_set, size_t flag)
  {
    if ( is_set ) {
      flags |= flag;
    }
    else {
      flags &= ~flag;
    }
  };

  void clear(mmap_file* mp) {
    _payload.free(mp);
  }

  TreeNodePtr find_victim(mmap_file* mp, TreeNodePtr& victim);
  TreeNodePtr find_victim_left(mmap_file* mp, TreeNodePtr& victim);

  TreeNodePtr insert(mmap_file* mp, Tuple const& x);
  TreeNodePtr remove(mmap_file* mp, Tuple const& x, bool& remove_node);

  void print(mmap_file* mp, std::ostream& out, int level);
};

bool recursive_check_level_invarient(
  mmap_file *mp, 
  TreeNodePtr n);

struct NodeEntryPair {
  TreeNodePtr ptr;
  enum Direction { left, right } dir;

  NodeEntryPair(TreeNodePtr a_ptr, Direction a_dir) :
    ptr(a_ptr), dir(a_dir) {};
};

typedef std::vector<NodeEntryPair> NodeEntryStack;

bool check_threading(mmap_file *mp, TreeNodePtr n, NodeEntryStack& stack);

#endif
