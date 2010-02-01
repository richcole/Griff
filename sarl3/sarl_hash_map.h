#ifndef SARL_HASH_MAP_H
#define SARL_HASH_MAP_H

#include "sarl_types.h"
#include "sarl_bit_set.h"

#define SARL_HASH_MAP_EMPTY_VALUE 0x80000001
#define SARL_HASH_MAP_DIRTY_VALUE 0x80000000

#define SARL_HASH_MAP_IS_VALID_VALUE(x) \
  ((x)!=SARL_HASH_MAP_EMPTY_VALUE && \
   (x)!=SARL_HASH_MAP_DIRTY_VALUE)

struct _sarl_hash_map_t {
  sarl_unsigned_int length;
  sarl_unsigned_int occupancy;
  sarl_unsigned_int *key_table;
  void*             *data_table;
};

inline sarl_hash_map_t* sarl_hash_map_new(sarl_unsigned_int length) {
  sarl_hash_map_t* pset = (sarl_hash_map_t *)
    GC_MALLOC(sizeof(sarl_hash_map_t));

  if (pset) {
    pset->key_table = (sarl_unsigned_int *)
      GC_MALLOC(sizeof(sarl_unsigned_int )*length);
    pset->data_table = (void **)
      GC_MALLOC(sizeof(void*)*length);

    if ( pset->key_table == 0 || pset->data_table == 0 ) {
      pset = 0;
      sarl_out_of_memory_error;
    }
    else {
      sarl_unsigned_int i;
      pset->length = length;
      pset->occupancy = 0;
      for(i=0;i<pset->length;++i) pset->key_table[i]=SARL_HASH_MAP_EMPTY_VALUE;
      memset(pset->data_table, 0, length * sizeof(void*));
    }
  }
  else {
    sarl_out_of_memory_error;
  }

  return pset;
};

inline sarl_hash_map_t* sarl_hash_map_default_new() {
  return sarl_hash_map_new(17);
};

void sarl_hash_map_insert(
  sarl_hash_map_t *pset, sarl_unsigned_int key, void* data
);

void sarl_hash_map_extend(
  sarl_hash_map_t* pset, sarl_unsigned_int length
);

void* sarl_hash_map_image(
  sarl_hash_map_t *pset, sarl_unsigned_int bset
);

sarl_hash_map_t* sarl_hash_map_remove(
  sarl_hash_map_t *pset, sarl_unsigned_int bset
);

inline sarl_unsigned_int sarl_hash_map_count(sarl_hash_map_t* pset)
{
  return (pset == 0) ? 0 : pset->occupancy;
};

inline void sarl_hash_map_clear(sarl_hash_map_t *pset) {
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int i;
    for(i=0;i<pset->length;++i) pset->key_table[i]=SARL_HASH_MAP_EMPTY_VALUE;
    memset(pset->data_table, 0, pset->length * sizeof(pset->data_table[0]));
    pset->occupancy = 0;
  }
};

void sarl_hash_map_or_equals(sarl_hash_map_t *dest, sarl_hash_map_t *src);

void sarl_hash_map_print_to_file(sarl_hash_map_t* pset, FILE *file);

#define SARL_HASH_MAP_FOR_EACH(type, extent, ptr, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<(pset)->length;++_xx_spsfe_i) { \
      if (SARL_HASH_MAP_IS_VALID_VALUE((pset)->key_table[_xx_spsfe_i])) { \
        extent = (pset)->key_table[_xx_spsfe_i]; \
        ptr = (type *)(pset)->data_table[_xx_spsfe_i];

#define SARL_HASH_MAP_END }}}

#endif
