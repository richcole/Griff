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
#include "tree_node_malloc_factory.h"

TreeNode *MallocTreeNodeFactory::alloc_tree_node() 
{
  TreeNode *ptr = new TreeNode;
  ptr->init(this);
  return ptr;
};

Byte* MallocTreeNodeFactory::alloc_payload()
{
  Byte *payload = new Byte[payload_size()];
  memset(payload, 0, payload_size());
  return payload;
};

void MallocTreeNodeFactory::release_tree_node(TreeNode* tree_node) 
{
  if ( tree_node->payload() ) { 
    release_payload(tree_node->payload());
  };
  delete tree_node;
};

void MallocTreeNodeFactory::release_payload(Byte* payload) 
{
  delete[] payload;
};

int MallocTreeNodeFactory::payload_size()
{
  return (__block_size - sizeof(TreeNode)) / sizeof(Byte);
};

  
MallocTreeNodeFactory::MallocTreeNodeFactory()
{
  __block_size = 1024;
};

