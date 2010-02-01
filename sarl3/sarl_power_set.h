#ifndef SARL_POWER_SET_H
#define SARL_POWER_SET_H

#include "sarl_types.h"
#include "sarl_bit_set.h"

struct _sarl_power_set_entry_t {
  sarl_bit_set_t      *ptr;
  sarl_unsigned_int    index;
};

typedef _sarl_power_set_entry_t sarl_power_set_entry_t;

struct _sarl_power_set_t {
  sarl_unsigned_int        length;
  sarl_unsigned_int        occupancy;
  sarl_unsigned_int        next_index;
  sarl_power_set_entry_t  *entry_table; 
};

sarl_power_set_t* sarl_power_set_new_from_power_map(sarl_power_map_t *pmap);

inline sarl_power_set_t* sarl_power_set_new(sarl_unsigned_int length) {
  sarl_power_set_t* pset = (sarl_power_set_t *)
    GC_MALLOC(sizeof(sarl_power_set_t));

  if (pset) {
    pset->entry_table = (sarl_power_set_entry_t*)
      GC_MALLOC(sizeof(*pset->entry_table)*length);
    if ( pset->entry_table == 0) {
      pset = 0;
      sarl_out_of_memory_error;
    }
    else {
      memset(pset->entry_table, 0, length * sizeof(*pset->entry_table));
      pset->length = length;
      pset->occupancy = 0;
      pset->next_index = 1;
    }
  }
  else {
    sarl_out_of_memory_error;
  }

  return pset;
};

inline sarl_power_set_t* sarl_power_set_default_new() {
  return sarl_power_set_new(17);
};

sarl_unsigned_int sarl_power_set_insert_with_index(
  sarl_power_set_t *pset, sarl_bit_set_t *bset, sarl_unsigned_int index
);

inline sarl_unsigned_int sarl_power_set_insert(
  sarl_power_set_t *pset, sarl_bit_set_t *bset) 
{
  return sarl_power_set_insert_with_index(pset, bset, 0);
};

void sarl_power_set_extend(
  sarl_power_set_t* pset, sarl_unsigned_int length
);

sarl_unsigned_int sarl_power_set_get_index(
  sarl_power_set_t *pset, sarl_bit_set_t *bset
);

inline sarl_unsigned_int sarl_power_set_is_member(
  sarl_power_set_t *pset, sarl_bit_set_t *bset
)
{
  return sarl_power_set_get_index(pset, bset);
};

sarl_power_set_t* sarl_power_set_remove(
  sarl_power_set_t *pset, sarl_bit_set_t *bset
);

inline sarl_unsigned_int sarl_power_set_count(sarl_power_set_t* pset)
{
  return (pset == 0) ? 0 : pset->occupancy;
};

inline void sarl_power_set_clear(sarl_power_set_t *pset) {
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    memset(pset->entry_table, 0, pset->length * sizeof(*pset->entry_table));
    pset->occupancy = 0;
  }
};

void sarl_power_set_or_equals(sarl_power_set_t *dest, sarl_power_set_t *src);

void sarl_power_set_add_extent(sarl_power_set_t *pset, sarl_bit_set_t *bset);

sarl_power_set_t* sarl_power_set_closure(sarl_power_set_t *pset, sarl_bit_set_t *object_set);

bool sarl_power_set_has_subseteq_in(sarl_power_set_t *pset, sarl_bit_set_t *superset);

void sarl_power_set_print_to_file(sarl_power_set_t* pset, FILE *file);

#define SARL_POWER_SET_FOR_EACH(extent, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<pset->length;++_xx_spsfe_i) { \
      if (sarl_is_table_entry_valid(pset->entry_table[_xx_spsfe_i].ptr)) { \
        extent = pset->entry_table[_xx_spsfe_i].ptr;

#define SARL_POWER_SET_END }}}

#define SARL_POWER_SET_FOR_EACH_INDEX(extent_index, extent, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<pset->length;++_xx_spsfe_i) { \
      if (sarl_is_table_entry_valid(pset->entry_table[_xx_spsfe_i].ptr)) { \
        extent = pset->entry_table[_xx_spsfe_i].ptr; \
        extent_index = pset->entry_table[_xx_spsfe_i].index;



    
  

#endif
