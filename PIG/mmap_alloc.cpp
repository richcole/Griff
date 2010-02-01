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
#include "mmap_alloc.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fcntl.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <assert.h>

/* Remapping doesn't preserve the address space. The solution is to map the
   file in chunks. Then each pointer must then encode the chunk that the data
   was allocated from. Then dereferencing the pointer must be done within the
   memory management unit by decoding the chunk.
*/

size_t mmap_create_chunk(mmap_file *mp, size_t chunk_size);
size_t mmap_alloc_chunk_block(mmap_file *mp, size_t size);
size_t alloc_block(mmap_file *mp, size_t size);
void   free_block(mmap_file* mp, size_t index);
char*  mmap_map_new_chunk(mmap_file *mp, size_t offset, size_t chunk_size);
void   mmap_map_chunk(mmap_file *mp, int chunk);


size_t mmap_create_chunk(mmap_file *mp, size_t chunk_size)
{
  if ( mp->chunks[0].offset == 0) {
    int page_size = getpagesize();
    int initial_offset = ((sizeof(*mp) / page_size) + 1) * page_size;
    mp->chunks[0].offset = initial_offset;
    mp->chunks[0].length = chunk_size;
    mp->chunks[0].data   = mmap_map_new_chunk(
      mp, mp->chunks[0].offset, mp->chunks[0].length
    );
    return 0;
  }
  else {
    size_t i;
    for(i=1;i<NUM_CHUNKS;++i) {
      if (mp->chunks[i].offset == 0) {
        mp->chunks[i].offset = mp->chunks[i-1].offset +
          mp->chunks[i-1].length;
        mp->chunks[i].length = chunk_size;
        mp->chunks[i].data = mmap_map_new_chunk(
          mp, mp->chunks[i].offset, mp->chunks[i].length);
        break;
      }
    }
    return i;
  }
};

size_t mmap_alloc_chunk_block(mmap_file *mp, size_t size)
{
  /* this allocation strategy doesn't work well for the hash table
     because the hash table elements outgrow the hash table size
  */

  /* check for space in each chunk */
  size_t real_size = size + sizeof(size_t);
  size_t std_chunk_size = getpagesize() * 64;
  int i;
  
  for(i=0;i<NUM_CHUNKS;++i) {
    if (mp->chunks[i].offset != 0) {
      if ( mp->chunks[i].head + real_size < mp->chunks[i].length ) {
        size_t new_block = index_from_chunk(mp, i, mp->chunks[i].head);
        mp->chunks[i].head += real_size;
        *size_ref_from_index(mp, new_block) = size;
        return new_block;
      };
    }
    else {
      while ( real_size * 4 > std_chunk_size ) {
        std_chunk_size *= 2;
      }
      mmap_create_chunk(mp, std_chunk_size);
      return mmap_alloc_chunk_block(mp, size);
    }
  };

  // admit defeat
  return 0;
};

size_t alloc_block(mmap_file *mp, size_t size)
{
  int i;
  size_t block;
  
  for(i=0;i<FREE_LIST_LEN;++i) {
    if ( mp->free_lists[i].size == 0 ) 
      break;
    if ( mp->free_lists[i].size == size ) {
      if ( mp->free_lists[i].first != 0 ) {
	block = mp->free_lists[i].first;
	mp->free_lists[i].first = free_block_from_index(mp, block)->next;
	return block;
      }
      else {
        return mmap_alloc_chunk_block(mp, size);
      }
    }
  }

  if (i == FREE_LIST_LEN) {
    if ( mp->misc_free_list == 0 ) {
      return mmap_alloc_chunk_block(mp, size);
    }
    else {
      if ( *size_ref_from_index(mp, mp->misc_free_list) > size ) {
        block = mp->misc_free_list;
        mp->misc_free_list = free_block_from_index(mp, block)->next;
        return block;
      }
      else {
        size_t prev = mp->misc_free_list;
        block = free_block_from_index(mp, prev)->next;
        while(block!=0 && size > *size_ref_from_index(mp, block)) {
          prev=block;
          block=free_block_from_index(mp, block)->next;
        };
        if ( block != 0 ) {
          free_block_from_index(mp, prev)->next = 
            free_block_from_index(mp, block)->next;
          return block;
        }
        else {
          return mmap_alloc_chunk_block(mp, size);
        }
      }
    }
  }
  else {
    return mmap_alloc_chunk_block(mp, size);
  };
};

void free_block(mmap_file* mp, size_t index)
{
  // add the block to a free list
  int i;
  for(i=0;i<FREE_LIST_LEN;++i) {
    if ( mp->free_lists[i].size == 0 ) {
      mp->free_lists[i].size = *size_ref_from_index(mp, index);
      mp->free_lists[i].first = index;
      return;
    }
    else if ( mp->free_lists[i].size == *size_ref_from_index(mp, index) ) {
      mmap_free_block* free_block = free_block_from_index(mp, index);
      free_block->next = mp->free_lists[i].first;
      mp->free_lists[i].first = index;
      return;
    }
  }

  mmap_free_block* free_block = free_block_from_index(mp, index);
  free_block->next = mp->misc_free_list;
  mp->misc_free_list = index;
};

mmap_file* mmap_init(int fd)
{
  struct stat stat_buf;
  char buf;
  
  mmap_file* md = 0;

  fstat(fd, &stat_buf);
  if ( stat_buf.st_size < INITIAL_FILE_SIZE ) {
    lseek(fd, INITIAL_FILE_SIZE, SEEK_SET);
    write(fd, &buf, 1);
    fstat(fd, &stat_buf);
  };

  md = (mmap_file *)
    mmap(0, sizeof(*md), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if ( md == 0 ) {
    perror("Unable to map file");
  };

  if ( strncmp(md->magic_number, "mmap_alloc", strlen("mmap_alloc")) != 0 ) {
    memset(md, 0, sizeof(*md));
    strcpy(md->magic_number, "mmap_alloc");
  };

  md->fd = fd;

  int i;
  for(i=0;i<NUM_CHUNKS;++i) {
    if ( md->chunks[i].offset != 0 ) {
      mmap_map_chunk(md, i);
    }
  }

  return md;
};

char *mmap_map_new_chunk(mmap_file *mp, size_t offset, size_t chunk_size)
{
  int ret_value;
  struct stat stat_buf;
  char c = 0;
  fstat(mp->fd, &stat_buf);
  if ( offset + chunk_size >= (size_t)stat_buf.st_size ) {
    ret_value = lseek(mp->fd, offset + chunk_size, SEEK_SET);
    ret_value = write(mp->fd, &c, 1);
  }
  return (char *)mmap(
    0, chunk_size, PROT_READ | PROT_WRITE, MAP_SHARED, mp->fd, 
    offset
  );
};

void mmap_map_chunk(mmap_file *mp, int chunk)
{
  mp->chunks[chunk].data   = mmap_map_new_chunk(
    mp, mp->chunks[chunk].offset, mp->chunks[chunk].length
  );
};


void mmap_finalize(mmap_file *md) 
{
  int i;
  int fd = md->fd;
  
  for(i=0;i<NUM_CHUNKS;++i) {
    if (md->chunks[i].data) {
      msync(md->chunks[i].data, md->chunks[i].length, MS_SYNC);
      munmap(md->chunks[i].data, md->chunks[i].length);
    }
  }
  
  msync(md, sizeof(*md), MS_SYNC);
  munmap(md, sizeof(*md));
  close(fd);
};

size_t index_from_ptr(mmap_file *mp, char *ptr)
{
  int i;
  for(i=0;i<NUM_CHUNKS;++i) {
    if (
      ptr > mp->chunks[i].data && 
      ptr < mp->chunks[i].data + mp->chunks[i].head
    ) {
      size_t offset = ((size_t)ptr) - ((size_t)mp->chunks[i].data) - sizeof(size_t);
      return index_from_chunk(mp, i, offset);
    }
  }

  abort(); // no containing chunk found
  return 0;
};


void mmap_print_stats(mmap_file *mp, std::ostream& out)
{
  int i;
  size_t curr;

  out << "mmap_file:" << std::endl;
  out << "  magic_number:" << mp->magic_number << std::endl;
  out << "  fd:" << mp->fd << std::endl;

  // free lists
  for(i=0;i<FREE_LIST_LEN;++i) {
    if ( mp->free_lists[i].size == 0 ) break;
    out << "  free_lists[" << i << "]:" << std::endl;
    out << "    size:" << mp->free_lists[i].size << std::endl;
    out << "    first:" << mp->free_lists[i].first << std::endl;
    curr=mp->free_lists[i].first; 
    while(curr!=0) {
      mmap_free_block *f = free_block_from_index(mp, curr);
      out << "    size:" << *size_ref_from_index(mp, curr) << std::endl;
      out << "    next:" << f->next << std::endl;
      curr = f->next;
    }
  };

  // chunks
  for(i=0;i<NUM_CHUNKS;++i) {
    if ( mp->chunks[i].offset == 0 ) break;
    out << "  chunk[" << i << "]:" << std::endl;
    out << "    offset:" << mp->chunks[i].offset << std::endl;
    out << "    length:" << mp->chunks[i].length << std::endl;
    out << "    head:" << mp->chunks[i].head << std::endl;
    out << "    data:" << (void *)mp->chunks[i].data << std::endl;
  };

  // keys
  for(i=0;i<NUM_KEYS;++i) {
    if ( mp->keys[i] == 0 && (i+1 == NUM_KEYS || mp->keys[i+1] == 0)) break;
    out << "  key[" << i << "]: " << mp->keys[i] << std::endl;
  };
  
  out << "  misc_free_list:" << mp->misc_free_list;
  curr=mp->misc_free_list; 
  while(curr!=0) {
    mmap_free_block *f = free_block_from_index(mp, curr);
    out << "    size:" << *size_ref_from_index(mp, curr) << std::endl;
    out << "    next:" << f->next << std::endl;
    curr = f->next;
  }

  out << std::endl;
};

  
    
      
