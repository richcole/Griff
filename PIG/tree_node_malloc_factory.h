#ifndef TREE_NODE_MALLOC_FACTORY_H
#define TREE_NODE_MALLOC_FACTORY_H

#include "tree_node_factory.h"

class MallocTreeNodeFactory : public TreeNodeFactory
{
  int __block_size;
public:
  MallocTreeNodeFactory();

  virtual TreeNode* alloc_tree_node();
  virtual Byte* alloc_payload();
  virtual void release_tree_node(TreeNode* tree_node);
  virtual void release_payload(Byte* tree_node);
  virtual int payload_size();
};

#endif
