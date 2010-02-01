#ifndef SARL_INT_SET_H
#define SARL_INT_SET_H

#include "sarl_types.h"
#include "sarl_bit_set.h"

#define SARL_INT_SET_EMPTY_VALUE 0x80000001
#define SARL_INT_SET_DIRTY_VALUE 0x80000000

#define SARL_INT_SET_IS_VALID_VALUE(x) \
  ((x)!=SARL_INT_SET_EMPTY_VALUE && \
   (x)!=SARL_INT_SET_DIRTY_VALUE)

struct _sarl_int_set_t {
  sarl_unsigned_int length;
  sarl_unsigned_int occupancy;
  sarl_unsigned_int *table; 

};
typedef struct _sarl_int_set_t sarl_int_set_t;

inline sarl_int_set_t* sarl_int_set_new(sarl_unsigned_int length) {
  sarl_int_set_t* pset = (sarl_int_set_t *)
    GC_MALLOC(sizeof(sarl_int_set_t));

  if (pset) {
    pset->table = (sarl_unsigned_int *)GC_MALLOC(sizeof(sarl_unsigned_int )*length);
    if ( pset->table == 0) {
      // free(pset);
      pset = 0;
      sarl_out_of_memory_error;
    }
    else {
      pset->length = length;
      pset->occupancy = 0;
      sarl_unsigned_int i;
      for(i=0;i<pset->length;++i) pset->table[i]=SARL_INT_SET_EMPTY_VALUE;
    }
  }
  else {
    sarl_out_of_memory_error;
  }

  return pset;
};

inline sarl_int_set_t* sarl_int_set_default_new() {
  return sarl_int_set_new(17);
};

void sarl_int_set_insert(
  sarl_int_set_t *pset, sarl_unsigned_int bset
);

void sarl_int_set_extend(
  sarl_int_set_t* pset, sarl_unsigned_int length
);

sarl_unsigned_int sarl_int_set_is_member(
  sarl_int_set_t *pset, sarl_unsigned_int bset
);

sarl_int_set_t* sarl_int_set_remove(
  sarl_int_set_t *pset, sarl_unsigned_int bset
);

inline sarl_unsigned_int sarl_int_set_count(sarl_int_set_t* pset)
{
  return (pset == 0) ? 0 : pset->occupancy;
};

inline void sarl_int_set_clear(sarl_int_set_t *pset) {
  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int i;
    for(i=0;i<pset->length;++i) pset->table[i]=SARL_INT_SET_EMPTY_VALUE;
    pset->occupancy = 0;
  }
};

void sarl_int_set_or_equals(sarl_int_set_t *dest, sarl_int_set_t *src);

void sarl_int_set_print_to_file(sarl_int_set_t* pset, FILE *file);

#define SARL_INT_SET_FOR_EACH(extent, pset) \
  { \
    sarl_unsigned_int _xx_spsfe_i; \
    for(_xx_spsfe_i=0;_xx_spsfe_i<pset->length;++_xx_spsfe_i) { \
      if (sarl_is_table_entry_valid(pset->table[_xx_spsfe_i])) { \
        extent = pset->table[_xx_spsfe_i];

#define SARL_INT_SET_END }}}

#endif
