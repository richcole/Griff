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
#include "persistent_tree.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "mmap_alloc.h"

PersistentTree::PersistentTree(char *filename) 
  throw (PersistentTree::CreateFailure)
{
  // try to open the file
  fd = open(filename, O_RDWR);

  if ( fd == -1 ) {
    switch errno {
    case EACCES:
      throw CreateFailure();
    case ENOENT:
	break;
    default:
      throw CreateFailure();
    };

    // try to create the file
    fd = open(filename, O_RDWR | O_CREAT, 0666);
    if ( fd == -1 ) {
      throw CreateFailure();
    };

    md = mmap_init(fd);

    if ( md == 0 ) {
      throw CreateFailure();
    }
    
    mmap_setkey(md, 1, 1024); // set the block size
    tree = (Tree *)alloc_block(md, sizeof(Tree));
    tree->init();
    mmap_setkey(md, 2, (size_t)tree); // set the position of the tree
  }
  else {
    md = mmap_init(fd);
    if ( md == 0 ) {
      throw CreateFailure();
    }

    tree = (Tree *)mmap_getkey(md, 2); // retrieve the position of the tree
  }

  fac = new PersistentTreeNodeFactory(md);
};

PersistentTree::~PersistentTree()
{
  delete fac;
  close(fd);
};

