#ifndef SARL_INT_ARRAY_H
#define SARL_INT_ARRAY_H

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_finalization.h"

#include <gc/gc.h>
#include <string.h>

// #define SARL_TRACE_INT_ARRAY_MALLOC(x) x
#define SARL_TRACE_INT_ARRAY_MALLOC(x)

struct _sarl_int_array_t {
  sarl_unsigned_int length;
  sarl_unsigned_int* table; 
};
typedef struct _sarl_int_array_t sarl_int_array_t;

inline char* sarl_int_array_name() {
  static char *name = "sarl_int_array_t";
  return name;
};

extern sarl_unsigned_int sarl_num_allocated_int_arrays;
void sarl_finalize_int_array(GC_PTR ptr, GC_PTR data);

inline sarl_int_array_t* sarl_int_array_new(sarl_unsigned_int length) {

  sarl_int_array_t* pset = (sarl_int_array_t *)
    GC_MALLOC(sizeof(sarl_int_array_t));

  SARL_TRACE_INT_ARRAY_MALLOC(
    ++sarl_num_allocated_int_arrays;
    GC_REGISTER_FINALIZER_NO_ORDER(pset, sarl_finalize_int_array, 0, 0, 0);
  );

  if (pset) {
    pset->table = (sarl_unsigned_int *)GC_MALLOC_ATOMIC(sizeof(sarl_unsigned_int *)*length);
    if ( pset->table == 0) {
      pset = 0;
      sarl_out_of_memory_error;
    }
    else {
      // GC_REGISTER_FINALIZER_NO_ORDER(
      //   pset, trace_finalization, sarl_int_array_name(), 0, 0
      // );
      memset(pset->table, 0, length * sizeof(sarl_unsigned_int*));
      pset->length = length;
    }
  }
  else {
    sarl_out_of_memory_error;
  }

  return pset;
};

inline sarl_int_array_t* sarl_int_array_dup(sarl_int_array_t* a)
{
  if ( a == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }
  else {
    sarl_int_array_t* result = sarl_int_array_new(a->length);
    if ( result ) {
      memcpy(result->table, a->table, sizeof(a->table[0])*a->length);
      return result;
    }
    else {
      sarl_out_of_memory_error;
      return 0;
    }
  }
};

inline void sarl_int_array_extend(
  sarl_int_array_t* pmap, sarl_unsigned_int length
)
{
  sarl_unsigned_int * new_table = (sarl_unsigned_int *)
    GC_MALLOC_ATOMIC(sizeof(sarl_unsigned_int *)*length);
  if (new_table != 0) {
    memcpy(new_table, pmap->table, sizeof(sarl_unsigned_int *)*pmap->length); 
    memset(new_table + pmap->length, 0, (length - pmap->length) * sizeof(sarl_unsigned_int*));
    // free(pmap->table);
    pmap->table = new_table;
    pmap->length = length;
  }
  else {
    sarl_out_of_memory_error;
  }
};

inline sarl_int_array_t* sarl_int_array_default_new() {
  return sarl_int_array_new(17);
};

inline sarl_unsigned_int sarl_int_array_count(sarl_int_array_t* pset)
{
  sarl_unsigned_int i, last = 0;
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    for(i=0;i<pset->length;++i) {
      if ( pset->table[i] != 0 ) last = i+1;
    }
  }
  return last;
};

inline sarl_unsigned_int sarl_int_array_count_non_zero(sarl_int_array_t *pset)
{
  sarl_unsigned_int i, result = 0;
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    for(i=0;i<pset->length;++i) {
      if ( pset->table[i] != 0 ) ++result;
    }
  }
  return result;
};

inline void sarl_int_array_push(
  sarl_int_array_t *pset, sarl_unsigned_int bset
)
{
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int index = sarl_int_array_count(pset);
    if ( index >= pset->length ) {
      sarl_int_array_extend(pset, (index*2)+1);
    }
    if (index < pset->length ) {
      pset->table[index] = bset;
    }
  }
}

inline sarl_unsigned_int sarl_int_array_get(
  sarl_int_array_t *pset, sarl_unsigned_int index)
{
  if ( index == 0 ) {
    sarl_invalid_index_error;
    return 0;
  } 
  else if ( pset == 0) {
    sarl_null_pointer_error;
    return 0;
  }
  else if ( index >= pset->length ) {
    return 0;
  }
  else {
    return pset->table[index-1];
  }
};

inline void sarl_int_array_set(
  sarl_int_array_t *pset, sarl_unsigned_int index, sarl_unsigned_int value)
{
  if (index == 0) {
    sarl_invalid_index_error;
  }
  else if ( pset == 0) {
    sarl_null_pointer_error;
  }
  else {
    if (index >= pset->length) {
      sarl_unsigned_int x = index*2+1;
      while(x <= index) x = x*2+1;
      sarl_int_array_extend(pset, x);
    }
    if (index < pset->length) {
      pset->table[index-1] = value;
    }
    else {
      sarl_out_of_memory_error;
    }
  }
};

inline sarl_unsigned_int sarl_int_array_pop(sarl_int_array_t *pset)
{
  sarl_unsigned_int index = sarl_int_array_count(pset);
  sarl_unsigned_int result = 0;
  if ( index > 0 ) {
    result = pset->table[index-1];
    pset->table[index-1] = 0;
  }
  return result;
};

inline void sarl_int_array_clear(sarl_int_array_t *pset) {
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    memset(pset->table, 0, pset->length * sizeof(pset->table[0]));
  }
};

inline sarl_unsigned_int sarl_int_array_first(sarl_int_array_t *arr)
{
  if ( arr == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int index;
    for(index=0;index<arr->length;++index) {
      if ( arr->table[index] != 0 ) {
        return arr->table[index];
      }
    }
  }
  return 0;
}

inline void sarl_int_array_heap_push(
  sarl_int_array_t *heap, sarl_int_array_t *values, sarl_unsigned_int index)
{
  // do insertion sort for the time being
  sarl_unsigned_int index_value, curr, curr_value, tmp;
  index_value = sarl_int_array_get(values, index);
  for(curr=0;curr<heap->length && heap->table[curr];++curr) {
    curr_value = sarl_int_array_get(values, heap->table[curr]);
    if ( index_value < curr_value ) {
      tmp = index;
      index = heap->table[curr];
      heap->table[curr] = tmp;
      index_value = curr_value;
    }
  }

  sarl_int_array_set(heap, curr+1, index);
};

inline 
  void sarl_int_array_incr(sarl_int_array_t *arr, sarl_unsigned_int index)
{
  sarl_int_array_set(arr, index, sarl_int_array_get(arr, index)+1);
};


inline 
  void sarl_int_array_decr(sarl_int_array_t *arr, sarl_unsigned_int index)
{
  sarl_int_array_set(arr, index, sarl_int_array_get(arr, index)-1);
};


inline 
  sarl_unsigned_int sarl_int_array_pop(
    sarl_int_array_t *heap, sarl_int_array_t *value
  )
{
  sarl_unsigned_int last_index = sarl_int_array_count(heap);
  sarl_unsigned_int last = sarl_int_array_get(heap, last_index);
  sarl_int_array_set(heap, last_index, 0);
  return last;
};

inline void sarl_int_array_truncate(sarl_int_array_t *arr, sarl_unsigned_int n)
{
  if ( n < arr->length ) {
    memset(arr->table + n, 0, sizeof(arr->table[0])*(arr->length - n));
  }
};

inline void sarl_int_array_swap(sarl_int_array_t *arr, sarl_unsigned_int i, sarl_unsigned_int j)
{
  if ( i == 0 || i > arr->length ) {
    sarl_invalid_index_error;
  }
  else if ( j == 0 || j > arr->length ) {
    sarl_invalid_index_error;
  }
  else {
    sarl_unsigned_int tmp = arr->table[i-1];
    arr->table[i-1] = arr->table[j-1];
    arr->table[j-1] = tmp;
  }
}; 

sarl_power_map_t *sarl_int_array_comapping(sarl_int_array_t *cmap);
sarl_int_array_t *sarl_int_array_inverse(sarl_int_array_t *cmap);


#define SARL_INT_ARRAY_FOR_EACH(extent, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<pset->length;++_xx_spsfe_i) { \
      if (0 != pset->table[_xx_spsfe_i]) { \
        extent = pset->table[_xx_spsfe_i];

#define SARL_INT_ARRAY_END }}}

#define SARL_INT_ARRAY_FOR_EACH_INDEX(coimage, image, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<pset->length;++_xx_spsfe_i) { \
      if (0 != pset->table[_xx_spsfe_i]) { \
        coimage = _xx_spsfe_i+1; \
        image = pset->table[_xx_spsfe_i]; 

#define SARL_INT_ARRAY_END }}}


    
  

#endif
