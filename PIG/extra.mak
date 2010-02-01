
index_documents.exe: index_documents.cpp payload.cpp tuple.cpp \
  tree.cpp tree_node.cpp tree_node_factory.cpp tree_node_iterator.cpp \
  avl_tree_node_ballance.cpp mmap_alloc.cpp hash.cpp
	g++ -O -march=i686 -o index_documents.exe \
          index_documents.cpp payload.cpp tuple.cpp \
          tree.cpp tree_node.cpp tree_node_factory.cpp tree_node_iterator.cpp \
          avl_tree_node_ballance.cpp mmap_alloc.cpp hash.cpp
