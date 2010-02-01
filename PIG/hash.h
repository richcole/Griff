#include "mmap_alloc.h"

#define NUM_HASH_BLOCKS  255
#define NUM_ARRAY_BLOCKS 255
#define ARRAY_BLOCK_SIZE 254
#define DEFAULT_HASH_SIZE 512

#define INITIALIZED_KEY  1
#define ARRAY_BLOCK_KEY  2
#define HASH_BLOCK_KEY   3

struct hash_block
{
  size_t num_entries;
  size_t num_occupied;
  size_t data[1];
};

struct master_hash_block
{
  size_t blocks[NUM_HASH_BLOCKS];
};

struct master_array_block
{
  size_t num_entries;
  size_t blocks[NUM_ARRAY_BLOCKS];
};

void hash_init(mmap_file *mp);
size_t hash_get_index(mmap_file* mp, char *data, size_t data_len);
size_t hash_get_word(mmap_file* mp, size_t index);
size_t hash_get_index_count(mmap_file* mp);

double get_ave_hash_probe_count();
double get_ave_hash_occupancy_count();
