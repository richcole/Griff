#ifndef TREE_NODE_FACTORY_H
#define TREE_NODE_FACTORY_H

#include "tree_node.h"

class TreeNodeFactory 
{
public:
  TreeNodeFactory();
  
  virtual TreeNode* alloc_tree_node() = 0;
  virtual Byte* alloc_payload() = 0;
  virtual void release_tree_node(TreeNode* tree_node) = 0;
  virtual void release_payload(Byte* tree_node) = 0;
  virtual int payload_size() = 0;
};

#endif
