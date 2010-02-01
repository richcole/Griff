#ifndef TREE_H
#define TREE_H

#include "tree_node.h"
#include "tree_node_iterator.h"
#include "mmap_alloc.h"

struct Tree {
  TreeNodePtr _root;

  void insert(mmap_file *mp, Tuple const& v);
  void remove(mmap_file *mp, Tuple const& v);

  TreeNodeIterator begin(mmap_file *mp);
  TreeNodeIterator end(mmap_file *mp);

  TreeNodeIterator first_gte(Tuple& x, mmap_file *mp);

  void print(mmap_file *mp, std::ostream& out) {
    if ( ! _root.is_null() ) {
      _root.ptr(mp)->print(mp, out, 0);
    }
  }
};

void tree_init(mmap_file *mp);

typedef mmap_ptr<Tree> TreePtr;

TreePtr tree_get_tree(mmap_file *mp);
bool check_threading(mmap_file *mp, TreePtr tree);


#endif
