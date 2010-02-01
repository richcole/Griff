#ifndef SARL_ARRAY_H
#define SARL_ARRAY_H

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_finalization.h"

#include <gc/gc.h>
#include <string.h>

// #define SARL_TRACE_ARRAY_MALLOC(x) x
#define SARL_TRACE_ARRAY_MALLOC(x)

struct _sarl_array_t {
  sarl_unsigned_int length;
  void**            table; 
  /* table entries are:
     0x0: empty
     0x1: dirty
     otherwise: valid pointer 
  */
};
typedef struct _sarl_array_t sarl_array_t;

inline char* sarl_array_name() {
  static char *name = "sarl_array_t";
  return name;
};

extern sarl_unsigned_int sarl_num_allocated_arrays;
void sarl_finalize_array(GC_PTR ptr, GC_PTR data);

inline sarl_array_t* sarl_array_new(sarl_unsigned_int length) {

  sarl_array_t* pset = (sarl_array_t *)
    GC_MALLOC(sizeof(sarl_array_t));

  SARL_TRACE_ARRAY_MALLOC(
    ++sarl_num_allocated_arrays;
    GC_REGISTER_FINALIZER_NO_ORDER(pset, sarl_finalize_array, 0, 0, 0);
  );

  if (pset) {
    pset->table = (void**)GC_MALLOC(sizeof(void *)*length);
    if ( pset->table == 0) {
      // free(pset);
      pset = 0;
      sarl_out_of_memory_error;
    }
    else {
      //      GC_REGISTER_FINALIZER_NO_ORDER(
      //        pset, trace_finalization, sarl_array_name(), 0, 0
      //      );
      memset(pset->table, 0, length * sizeof(sarl_unsigned_int*));
      pset->length = length;
    }
  }
  else {
    sarl_out_of_memory_error;
  }

  return pset;
};

inline void sarl_array_extend(
  sarl_array_t* pmap, sarl_unsigned_int length
)
{
  void** new_table = (void **)
    GC_MALLOC(sizeof(void *)*length);
  if (new_table != 0) {
    memcpy(new_table, pmap->table, sizeof(void *)*pmap->length); 
    memset(new_table + pmap->length, 0, (length - pmap->length) * sizeof(void *));
    // free(pmap->table);
    pmap->table = new_table;
    pmap->length = length;
  }
  else {
    sarl_out_of_memory_error;
  }
};

inline sarl_array_t* sarl_array_default_new() {
  return sarl_array_new(17);
};

/* copies references, doesn't duplicate the array objects */
inline sarl_array_t* sarl_array_dup_ref(sarl_array_t *src)
{
  sarl_array_t *dst = sarl_array_new(src->length);

  if ( dst == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }
  memcpy(dst->table, src->table, sizeof(src->table[0])*src->length);
  return dst;
};

// returns the index of the last non zero value
inline sarl_unsigned_int sarl_array_count(sarl_array_t* pset)
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

inline sarl_unsigned_int sarl_array_push(
  sarl_array_t *pset, void *bset
)
{
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int index = sarl_array_count(pset);
    if ( index >= pset->length ) {
      sarl_array_extend(pset, (index*2)+1);
    }
    if (index < pset->length ) {
      pset->table[index] = bset;
    }
    return index+1;
  }

  return 0;
}

inline void sarl_array_truncate(sarl_array_t *arr, sarl_unsigned_int n)
{
  if ( n < arr->length ) {
    memset(arr->table + n, 0, sizeof(arr->table[0])*(arr->length - n));
  }
};

inline void* sarl_array_get(
  sarl_array_t *pset, sarl_unsigned_int index)
{
  if ( index == 0 ) {
    sarl_invalid_index_error;
    return 0;
  } 
  else if ( pset == 0) {
    sarl_null_pointer_error;
    return 0;
  }
  else if ( index > pset->length ) {
    return 0;
  }
  else {
    return pset->table[index-1];
  }
};

inline void sarl_array_set(
  sarl_array_t *pset, sarl_unsigned_int index, void *ptr)
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
      sarl_array_extend(pset, x);
    }
    if (index < pset->length) {
      pset->table[index-1] = ptr;
    }
    else {
      sarl_out_of_memory_error;
    }
  }
};

inline void* sarl_array_pop(sarl_array_t *pset)
{
  sarl_unsigned_int index = sarl_array_count(pset);
  void *result = 0;
  if ( index > 0 ) {
    result = pset->table[index-1];
    pset->table[index-1] = 0;
  }
  return result;
};

inline void sarl_array_clear(sarl_array_t *pset) {
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    memset(pset->table, 0, pset->length * sizeof(pset->table[0]));
  }
};

#define SARL_ARRAY_FOR_EACH(type, extent, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<pset->length;++_xx_spsfe_i) { \
      if (0 != pset->table[_xx_spsfe_i]) { \
        extent = (type *)pset->table[_xx_spsfe_i];

#define SARL_ARRAY_END }}}

#define SARL_ARRAY_FOR_EACH_INDEX(index, type, extent, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<pset->length;++_xx_spsfe_i) { \
      if (0 != pset->table[_xx_spsfe_i]) { \
        index = _xx_spsfe_i + 1; \
        extent = (type *)pset->table[_xx_spsfe_i];

inline void* sarl_array_first(sarl_array_t *pset) {
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int i;
    for(i=0;i<pset->length;++i) {
      if (pset->table[i]) {
        return pset->table[i];
      }
    }
  }
  return 0;
}
    
  

#endif
