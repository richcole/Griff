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
#include "hash.h"

size_t hash_alloc(mmap_file* mp, size_t length)
{
  size_t ret_block = alloc_block(mp, sizeof(size_t)*length);
  hash_block *hb = (hash_block *)ptr_from_index(mp, ret_block);
  memset(hb, 0, sizeof(size_t)*length);

  // length is smaller by 2 integers because there are 2 integers
  // at the beginning of the struct
  size_t s = sizeof(*hb->data) * 8;
  hb->num_entries = (length - 3) * s / (s + 1);
  assert(hb->num_entries + (hb->num_entries / s) + 1 <= length - 2);
  return ret_block;
};

inline size_t get_hash_dirty_index(hash_block *block, size_t index)
{
  return (index % block->num_entries) / (sizeof(*block->data) * 8);
};

inline size_t get_hash_dirty_offset(hash_block *block, size_t index)
{
  return (index % block->num_entries) % (sizeof(*block->data) * 8);
};

bool hash_is_dirty(hash_block* block, size_t index)
{
  return 
    (
      block->data[get_hash_dirty_index(block,index)] >> 
      get_hash_dirty_offset(block,index)
    )
    & 0x1;
}

void hash_set_dirty(hash_block *block, size_t index)
{
  block->data[get_hash_dirty_index(block,index)] |=
    (0x1 << get_hash_dirty_offset(block,index));
}

void hash_set_clear(hash_block *block, size_t index)
{
  block->data[get_hash_dirty_index(block,index)] &=
    ~(0x1 << get_hash_dirty_offset(block,index));
}

inline size_t get_hash_entry_index(hash_block *block, size_t index)
{
  return (index % block->num_entries) + 
    (block->num_entries / (sizeof(*block->data) * 8)) + 1;
};

size_t hash_get_entry(hash_block *block, size_t index)
{
  return block->data[get_hash_entry_index(block,index)];
};

void hash_set_entry(hash_block *block, size_t index, size_t entry)
{
  block->data[get_hash_entry_index(block,index)] = entry;
};

size_t hash_get_word(mmap_file* mp, master_array_block *ma, size_t index)
{
  int i;
  for(i=0;i<NUM_ARRAY_BLOCKS && ma->blocks[i]!=0;++i) {
    size_t array_size = 
      *size_ref_from_index(mp, ma->blocks[i]) / sizeof(size_t);
    if ( index < array_size ) {
      return ((size_t *)ptr_from_index(mp, ma->blocks[i]))[index];
    }
    else {
      index -= array_size;
    }
  }
  return 0;
};

size_t hash_get_word(mmap_file* mp, size_t index)
{
  hash_get_word(mp, 
    (master_array_block *)ptr_from_index(mp, mmap_getkey(mp, ARRAY_BLOCK_KEY)), 
    index
  );
};

inline bool data_eq(
  mmap_file* mp, size_t entry, char* data_ptr, size_t data_len)
{
  size_t word_index = hash_get_word(mp, entry);
  
  if ( data_len != *size_ref_from_index(mp, word_index) ) return false;
  
  char *word_ptr = ptr_from_index(mp, word_index);
  return memcmp(word_ptr, data_ptr, data_len) == 0;
};

inline size_t data_copy(mmap_file* mp, char *data_ptr, size_t data_len)
{
  size_t index = alloc_block(mp, data_len);
  memcpy(ptr_from_index(mp, index), data_ptr, data_len);
  return index;
};

inline size_t hash_value(char *data_ptr, size_t data_len)
{
  size_t result = 0;
  size_t i;
  size_t primes[] = { 0, 3, 21, 17, 9, 11, 5, 23 };
  for(i=0;i<data_len && i < 64;++i) {
    result ^= ((size_t)data_ptr[i]) << primes[(i % 8)];
  }
  return result;
};

int hash_lookup_count = 0;
int hash_probe_count = 0;
double hash_occupancy_count = 0;

double get_ave_hash_probe_count() {
  return ((double)hash_probe_count)/((double)hash_lookup_count);
};

double get_ave_hash_occupancy_count() {
  return hash_occupancy_count/((double)hash_lookup_count);
};

size_t hash_lookup(
  mmap_file* mp,
  hash_block *block, 
  size_t hash_index,
  char* data,
  size_t data_len)
{
  ++hash_lookup_count;
  hash_occupancy_count += block->num_occupied / (double)block->num_entries;

  size_t entry = hash_get_entry(block, hash_index);

  while ( 
    (entry == 0 || ! data_eq(mp, entry, data, data_len)) && 
    hash_is_dirty(block, hash_index) 
  ) {
    ++hash_probe_count;
    ++hash_index;
    entry = hash_get_entry(block, hash_index);
  }
  
  return entry;
};

size_t hash_is_full(mmap_file* mp, size_t hash_block_index)
{
  hash_block *hb = (hash_block *)ptr_from_index(mp, hash_block_index);
  return hb->num_occupied * 4 > hb->num_entries * 3;
};

size_t hash_insert(
  mmap_file* mp,
  hash_block *block, 
  size_t hash_index,
  size_t data_entry)
{
  size_t entry = hash_get_entry(block, hash_index);

  while ( entry != 0 ) {
    hash_set_dirty(block, hash_index);
    ++hash_index;
    entry = hash_get_entry(block, hash_index);
  }

  ++(block->num_occupied);
  hash_set_entry(block, hash_index, data_entry);
};

size_t alloc_array_block(mmap_file* mp, size_t num_entries)
{
  size_t block_index = alloc_block(mp, num_entries * sizeof(size_t));
  memset(ptr_from_index(mp, block_index), 0, num_entries * sizeof(size_t));
  return block_index;
};
  
size_t hash_copy_word(mmap_file* mp, master_array_block *ma, 
  char *data, size_t data_len)
{
  size_t i, k, offset;

  ma->num_entries++;

  offset = 0;
  for(i=0;i<NUM_ARRAY_BLOCKS && ma->blocks[i]!=0;++i) {
    offset += (*size_ref_from_index(mp, ma->blocks[i])) / sizeof(size_t);
  };
  
  if ( i == 0 ) {
    ma->blocks[i] = alloc_array_block(mp, ARRAY_BLOCK_SIZE);
    ((size_t *)ptr_from_index(mp, ma->blocks[i]))[1] = 
      data_copy(mp, data, data_len);
    return 1;
  }
  else {
    size_t array_len = (*size_ref_from_index(mp, ma->blocks[i-1])) / sizeof(size_t);
    if ( ((size_t *)ptr_from_index(mp, ma->blocks[i-1]))[array_len-1] == 0 ) {
      offset -= array_len;
      for(k=1;k<array_len;++k) {
	if (((size_t *)ptr_from_index(mp, ma->blocks[i-1]))[k] == 0) break;
      }
      ((size_t *)ptr_from_index(mp, ma->blocks[i-1]))[k] = 
	data_copy(mp, data, data_len);
      return offset + k;
    }
    else {
      ma->blocks[i] = alloc_array_block(mp, 
	((*size_ref_from_index(mp, ma->blocks[i-1])) * 2) / sizeof(size_t)
      );
      ((size_t *)ptr_from_index(mp, ma->blocks[i]))[0] = 
	data_copy(mp, data, data_len);
      return offset;
    }
  }
};

size_t hash_get_index(
  mmap_file* mp, 
  size_t master_hash_block_index, 
  size_t master_array_block_index,
  char *data, 
  size_t data_len)
{
  master_hash_block* mb = (master_hash_block *)ptr_from_index(mp, master_hash_block_index);
  size_t hv = hash_value(data, data_len);

  size_t i;
  for(i=0;i<NUM_HASH_BLOCKS && mb->blocks[i] != 0;++i) {
    size_t index = hash_lookup(
      mp, (hash_block *)ptr_from_index(mp, mb->blocks[i]),
      hv, data, data_len
    );
    if ( index != 0 && data_eq(mp, index, data, data_len)) {
      return index;
    }
  }

  if ( i == NUM_HASH_BLOCKS ) {
    fprintf(stderr, "Out of memory\n");
    return 0;
  }

  size_t index;
  if ( mb->blocks[i] == 0 ) {
    if ( i == 0 ) {
      mb->blocks[i] = hash_alloc(mp, DEFAULT_HASH_SIZE);
      hash_insert(mp, 
	(hash_block *)ptr_from_index(mp, mb->blocks[i]), 
	hv,
	index = hash_copy_word(
	  mp, 
	  (master_array_block *)ptr_from_index(mp, master_array_block_index), 
	  data, data_len
	)
      );
    }
    else if (  hash_is_full(mp, mb->blocks[i-1]) ) {
      size_t prev_array_length = 
        *size_ref_from_index(mp, mb->blocks[i-1]) / sizeof(size_t);
      mb->blocks[i] = hash_alloc(mp, prev_array_length*2);
      hash_insert(mp, 
	(hash_block *)ptr_from_index(mp, mb->blocks[i]), 
	hv, 
	index = hash_copy_word(
	  mp, 
          (master_array_block *)ptr_from_index(mp, master_array_block_index), 
          data, data_len
	)
      );
    }
    else {
      hash_insert(mp, 
	(hash_block *)ptr_from_index(mp, mb->blocks[i-1]), 
	hv, 
	index = hash_copy_word(
	  mp, (master_array_block *)ptr_from_index(mp, master_array_block_index), data, data_len
	)
      );
    }
  }

  return index;
};

size_t hash_get_index(mmap_file* mp, char *data, size_t data_len)
{
  hash_get_index(mp, 
    mmap_getkey(mp, HASH_BLOCK_KEY), 
    mmap_getkey(mp, ARRAY_BLOCK_KEY), 
    data, data_len
  );
};

void hash_init(mmap_file *mp)
{
  if ( mmap_getkey(mp, INITIALIZED_KEY) == 0 ) {
    size_t ab = alloc_block(mp, sizeof(master_array_block));
    size_t hb = alloc_block(mp, sizeof(master_hash_block));
    memset(ptr_from_index(mp, ab), 0, sizeof(master_array_block));
    memset(ptr_from_index(mp, hb), 0, sizeof(master_hash_block));
    mmap_setkey(mp, ARRAY_BLOCK_KEY, ab);
    mmap_setkey(mp, HASH_BLOCK_KEY, hb);
    mmap_setkey(mp, INITIALIZED_KEY, 1);
  }
};

size_t hash_get_index_count(mmap_file* mp)
{
  size_t ab = mmap_getkey(mp, ARRAY_BLOCK_KEY);
  master_array_block* ma = (master_array_block *)ptr_from_index(mp, ab);
  return ma->num_entries;
};
