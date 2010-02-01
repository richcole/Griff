#include "tree.h"
#include "tree_node_persistent_factory.h"

class PersistentTree 
{
public:
  struct mmap_file *md;
  int   fd;
  PersistentTreeNodeFactory *fac;
  Tree *tree;

  class CreateFailure {};
  
  PersistentTree(char *filename) throw (CreateFailure);
  ~PersistentTree();

  void insert(Tuple const& v) {
    tree->insert(fac, v);
  };
  
  void remove(Tuple const& v) 
  {
    tree->remove(fac, v);
  };

  TreeNodeIterator begin() 
  {
    return tree->begin(fac);
  };

  TreeNodeIterator end() 
  {
    return tree->end(fac);
  };
};
