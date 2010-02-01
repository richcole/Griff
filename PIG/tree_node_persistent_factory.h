#ifndef TREE_NODE_PERSISTENT_FACTORY_H
#define TREE_NODE_PERSISTENT_FACTORY_H

#include "tree_node_factory.h"

class PersistentTreeNodeFactory : public TreeNodeFactory
{
  struct mmap_file* md;          // malloc descriptor

public:
  PersistentTreeNodeFactory(struct mmap_file* a_md);

  virtual TreeNodePtr alloc_tree_node();
  virtual BytePtr     alloc_payload();

  virtual void release_tree_node(TreeNodePtr tree_node);
  virtual void release_payload(BytePtr tree_node);
  
  virtual int payload_size();
};
  


#endif
