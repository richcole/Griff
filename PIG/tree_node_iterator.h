#ifndef TREE_NODE_ITERATOR_H
#define TREE_NODE_ITERATOR_H

#include "tree_node.h"
#include "payload.h"

class TreeNodeIterator
{
  TreeNodePtr      node;
  mmap_file*       mp;
  PayloadStream    payload_st;
  Tuple            curr_tuple;
  Interval         curr_interval;

  
public:
  TreeNodeIterator();
  TreeNodeIterator(TreeNodePtr a_node, mmap_file* mp);

  void init(TreeNodePtr node);
  void next();
  void prev();
  Tuple const& val() ;

  inline bool is_null() { return node.is_null(); };
};
  
#endif
