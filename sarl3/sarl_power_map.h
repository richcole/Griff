#ifndef SARL_POWER_MAP_H
#define SARL_POWER_MAP_H

#include "sarl_types.h"
#include "sarl_bit_set.h"
#include "sarl_power_set.h"

#include <gc/gc.h>

struct _sarl_power_map_t {
  sarl_unsigned_int  length;
  sarl_bit_set_t**   table; 
};
typedef struct _sarl_power_map_t sarl_power_map_t;


/* the array simply grows as it is accessed */

inline sarl_power_map_t* sarl_power_map_new(sarl_unsigned_int length) {
  sarl_power_map_t* pmap = (sarl_power_map_t *)
    GC_MALLOC(sizeof(sarl_power_map_t));

  if (pmap) {
    pmap->table = (sarl_bit_set_t**)GC_MALLOC(
      sizeof(sarl_bit_set_t *)*length
    );
    if ( pmap->table == 0) {
      // free(pmap);
      pmap = 0;
      sarl_out_of_memory_error;
    }
    else {
      memset(pmap->table, 0, sizeof(sarl_unsigned_int*));
      pmap->length = length;
    }
  }
  else {
    sarl_out_of_memory_error;
  }

  return pmap;
};

inline sarl_power_map_t* sarl_power_map_default_new() {
  return sarl_power_map_new(12);
};

inline void sarl_power_map_extend(
  sarl_power_map_t* pmap, sarl_unsigned_int length
)
{
  sarl_bit_set_t** new_table = (sarl_bit_set_t **)
    GC_MALLOC(sizeof(sarl_bit_set_t *)*length);
  if (new_table != 0) {
    memcpy(new_table, pmap->table, sizeof(sarl_bit_set_t *)*pmap->length); 
    // free(pmap->table);
    pmap->table = new_table;
    pmap->length = length;
  }
  else {
    sarl_out_of_memory_error;
  }
};

inline void sarl_power_map_set(
  sarl_power_map_t* pmap, sarl_unsigned_int index, sarl_bit_set_t *pset
)
{
  if ( pmap == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    if ( index > pmap->length ) {
      sarl_unsigned_int new_length = pmap->length*2;
      while( index >= new_length ) new_length = ( (new_length) * 3) / 2;
      sarl_power_map_extend(pmap, new_length);
    }
    if ( index <= pmap->length ) {
      pmap->table[index-1] = pset;
    }
    else {
      sarl_internal_error;
    }
  }
}

inline sarl_bit_set_t* sarl_power_map_get(
  sarl_power_map_t* pmap, sarl_unsigned_int index
)
{
  if ( pmap == 0 || index == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }
  else {
    if ( index > pmap->length ) {
      return 0;
    }
    else {
      return pmap->table[index-1];
    }
  }
}

inline void sarl_power_map_insert(
  sarl_power_map_t *pmap, sarl_unsigned_int coimage, sarl_unsigned_int image)
{
  sarl_bit_set_t *image_set = sarl_power_map_get(pmap, coimage);
  if (image_set == 0) {
    image_set = sarl_bit_set_default_new();
    sarl_power_map_set(pmap, coimage, image_set);
  }
  sarl_bit_set_set(image_set, image);
}

inline void sarl_power_map_remove(
  sarl_power_map_t *pmap, sarl_unsigned_int coimage, sarl_unsigned_int image)
{
  sarl_bit_set_t *image_set = sarl_power_map_get(pmap, coimage);
  if (image_set != 0) {
    sarl_bit_set_clear(image_set, image);
  }
}

inline void sarl_power_map_remove_domain(
  sarl_power_map_t *pmap, sarl_unsigned_int coimage)
{
  sarl_power_map_set(pmap, coimage, 0);
}

inline sarl_power_map_t* sarl_power_map_dup(sarl_power_map_t *pmap)
{
  sarl_power_map_t* dup_pmap = sarl_power_map_new(pmap->length);
  if ( dup_pmap ) {
    sarl_unsigned_int i;
    for(i=0;i<pmap->length;++i) {
      if ( pmap->table[i] ) {
        dup_pmap->table[i] = sarl_bit_set_dup(pmap->table[i]);
      }
    }
  }
  else {
    sarl_out_of_memory_error;
  }
  return dup_pmap;
};

void sarl_power_map_sort(sarl_power_map_t *pmap);
sarl_power_map_t* sarl_power_map_inverse(sarl_power_map_t *pmap);

void sarl_power_map_fill_nulls(
  sarl_power_map_t *pmap, sarl_unsigned_int length
);

void sarl_power_map_print_to_file(sarl_power_map_t *pmap, FILE *file);

#define SARL_POWER_MAP_FOR_EACH(index, extent, pmap) \
  { \
    for(index=1;index<=pmap->length;++index) { \
        extent = pmap->table[index-1]; \
        if (extent) {


#define SARL_POWER_MAP_END }}}


#endif
