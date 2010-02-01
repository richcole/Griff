#ifndef SARL_GC_H
#define SARL_GC_H

extern std::hash_set<sarl_unsigned_int> first_heap;
extern std::hash_set<sarl_unsigned_int> second_heap;

#error "Old File, do not try to compile!"

inline void* sarl_calloc(
  sarl_unsigned_int elems_size, sarl_unsigned_int num_elems
) 
{
  sarl_unsigned_int *ptr = (sarl_unsigned_int *)
    malloc((elems_size * num_elems)+sizeof(sarl_unsigned_int));
  if ( ptr ) {
    sarl_register_pointer(ptr);
  }
  return ptr+1;
}

inline sarl_register_pointer(sarl_unsigned_int *ptr) {
  first_heap.insert(ptr)
}

inline sarl_garbage_collect(ptr) {
  static std::hash_set<sarl_unsigned_int>

#endif
