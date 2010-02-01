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
#include "tree_node_persistent_factory.h"
#include "mmap_alloc.h"

PersistentTreeNodeFactory::PersistentTreeNodeFactory(struct mmap_file* a_md)
{
  md = a_md;
  block_size = (unsigned int)mmap_getkey(md, 1);
};

TreeNode *PersistentTreeNodeFactory::alloc_tree_node() 
{
  TreeNode *ptr = (TreeNode *)alloc_block(md, sizeof(TreeNode));
  ptr->init(this);
  return ptr;
};

Byte* PersistentTreeNodeFactory::alloc_payload()
{
  Byte *payload = (Byte *)alloc_block(md, sizeof(Byte)*payload_size());
  int  i;
  for(i=0;i<payload_size();++i) payload[i] = 0;
  return payload;
};

void PersistentTreeNodeFactory::release_tree_node(TreeNode* tree_node) 
{
  if ( tree_node->payload() ) { 
    release_payload(tree_node->payload());
  };
  free_block(md, (size_t)tree_node);
};

void PersistentTreeNodeFactory::release_payload(Byte* payload) 
{
  free_block(md, (size_t)payload);
};

int PersistentTreeNodeFactory::payload_size()
{
  return (block_size - sizeof(TreeNode)) / sizeof(Byte);
};

  


