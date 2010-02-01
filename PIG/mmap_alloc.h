#ifndef MMAP_ALLOC_H
#define MMAP_ALLOC_H

#include <sys/types.h>
#include <assert.h>

#define FREE_LIST_LEN     256
#define INITIAL_FILE_SIZE (64 * 1024)
#define NUM_KEYS          256
#define NUM_CHUNKS        256

#include <iostream>

struct mmap_free_block 
{
  size_t next;
};

struct mmap_free_entry 
{
  size_t   size;
  size_t   first;
};

struct mmap_chunk 
{
  size_t   offset;
  size_t   length;
  size_t   head;
  char*    data;
};

struct mmap_file
{
  char             magic_number[16];
  int              fd;
  mmap_free_entry  free_lists[FREE_LIST_LEN];
  mmap_chunk       chunks[NUM_CHUNKS];
  size_t           keys[NUM_KEYS];
  size_t           misc_free_list;
};

inline void mmap_setkey(mmap_file *mp, size_t key, size_t value)
{
  if ( key < NUM_KEYS ) {
    mp->keys[key] = value;
  }
};

inline size_t mmap_getkey(mmap_file *mp, size_t key)
{
  if ( key < NUM_KEYS ) {
    return mp->keys[key];
  }
  else {
    return 0;
  };
};

inline char *ptr_from_index(mmap_file *mp, size_t index)
{
  assert(index != 0);
  return (mp->chunks[(index & 0xff)-1].data) + 
    ((index >> 8) + sizeof(size_t));
};

size_t index_from_ptr(mmap_file *mp, char *ptr);

inline size_t *size_ref_from_index(mmap_file *mp, size_t index)
{
  assert(index != 0);
  return (size_t *)
    (mp->chunks[(index & 0xff)-1].data + (index >> 8));
};

inline mmap_free_block* free_block_from_index(mmap_file *mp, size_t index)
{
  return (mmap_free_block *)ptr_from_index(mp, index);
};


inline size_t index_from_chunk(mmap_file *mp, size_t chunk, size_t offset)
{
  return (offset << 8) | ((chunk+1) & 0xff);
};

size_t alloc_block(mmap_file *mp, size_t size);
void  free_block(mmap_file *mp, size_t block);
mmap_file* mmap_init(int fd);
void mmap_finalize(mmap_file *md);

void mmap_ensure_space(mmap_file *mp, size_t size);
void mmap_setkey(mmap_file *mp, size_t key, size_t data);
size_t mmap_getkey(mmap_file *mp, size_t key);

template<class T> class mmap_ptr {
  size_t index;
  
  mmap_ptr(size_t a_index) 
    : index(a_index) {};
  
public:
  mmap_ptr() 
    : index(0) {};

  mmap_ptr(mmap_ptr const& p) 
    : index(p.index)
  {};
  
  mmap_ptr& operator=(mmap_ptr const& p) 
  {
    index = p.index;
    return *this;
  };

  T& ref(mmap_file* mp) {
    return *reinterpret_cast<T*>(ptr_from_index(mp, index));
  };

  T* ptr(mmap_file* mp) {
    return reinterpret_cast<T*>(ptr_from_index(mp, index));
  };

  bool operator==(mmap_ptr const& p) const
  {
    return index == p.get_index();
  };

  bool operator!=(mmap_ptr const& p) const
  {
    return index != p.get_index();
  };

  bool is_null() {
    return index == 0;
  };

  static mmap_ptr<T> null() {
    return mmap_ptr<T>();
  };

  static mmap_ptr<T> alloc(mmap_file* mp, size_t len = 1) 
  {
    return mmap_ptr(alloc_block(mp, sizeof(T) * len));
  };

  static mmap_ptr<T> recover(mmap_file* mp, T* abs_ptr) 
  {
    return mmap_ptr(index_from_ptr(mp, (char *)abs_ptr));
  };

  static mmap_ptr<T> getkey(mmap_file *mp, size_t key)
  {
    return mmap_ptr(mmap_getkey(mp, key));
  };

  void setkey(mmap_file *mp, size_t key)
  {
    mmap_setkey(mp, key, index);
  };

  void free(mmap_file *mp) 
  {
    free_block(mp, index);
    index = 0;
  };

  size_t get_index() const {
    return index;
  };
};

void mmap_print_stats(mmap_file *mp, std::ostream& out);


#endif
