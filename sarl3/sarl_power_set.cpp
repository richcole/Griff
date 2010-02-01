#include "sarl_power_set.h"
#include "sarl_power_map.h"
#include <stdio.h>

// #define SARL_TRACE_POWER_SET

#if ! defined(NDEBUG) && defined(SARL_TRACE_POWER_SET)

#define sarl_power_set_print_trace(pset) \
  fprintf(stdout, "%s = ", #pset); \
  sarl_power_set_print_to_file(pset, stdout); \
  fprintf(stdout, "\n"); 

#define sarl_bit_set_print_trace(pset) \
  fprintf(stdout, "%s = ", #pset); \
  sarl_bit_set_print_to_file(pset, stdout); \
  fprintf(stdout, "\n"); 

#define sarl_print_trace(string) \
  fprintf(stdout, "%s\n", string);

#else

#define sarl_power_set_print_trace(pset) 
#define sarl_bit_set_print_trace(pset) 
#define sarl_print_trace(pset) 

#endif

sarl_unsigned_int sarl_power_set_insert_with_index(
  sarl_power_set_t  *pset, 
  sarl_bit_set_t    *bset, 
  sarl_unsigned_int  bset_index) 
{
  if (pset == 0) {
    sarl_null_pointer_error;
  }

  sarl_unsigned_int hash_value = sarl_bit_set_hash(bset);
  sarl_unsigned_int hash_index = hash_value % pset->length;
  sarl_unsigned_int initial_hash_index = hash_index;
  sarl_unsigned_int finished = 0;

  if (pset->occupancy * 4 > pset->length * 3) {
    sarl_power_set_extend(pset, (pset->length*2)+1);
  }

  sarl_unsigned_int first_dirty_index = pset->length;

  do {
    switch ( (sarl_unsigned_int) pset->entry_table[hash_index].ptr ) {
    case 0x0:
      if (first_dirty_index == pset->length) {
        pset->entry_table[hash_index].ptr = bset;
        pset->occupancy++;
        if ( bset_index ) {
          pset->entry_table[hash_index].index = bset_index;
          pset->next_index = sarl_max_of(pset->next_index, bset_index+1);
        }          
        else {
          pset->entry_table[hash_index].index = (bset_index = pset->next_index++);
        }
      }
      else {
        pset->entry_table[first_dirty_index].ptr = bset;
        pset->occupancy++;
        if ( bset_index ) {
          pset->entry_table[first_dirty_index].index = bset_index;
          pset->next_index = sarl_max_of(pset->next_index, bset_index+1);
        }          
        else {
          pset->entry_table[first_dirty_index].index = (bset_index = pset->next_index++);
        }
      }
      finished = 1;
      break;
    case 0x1:
      if (first_dirty_index == pset->length) {
        first_dirty_index = hash_index;
      }
      break;
    default:
      if ( sarl_bit_set_is_eql(bset, pset->entry_table[hash_index].ptr ) ) {
        bset_index = pset->entry_table[hash_index].index;
        finished = true;
      }
    }
    hash_index = (hash_index + 1) % pset->length;
    if ( hash_index == initial_hash_index ) {
      sarl_internal_error;
      break;
    }
  } while( ! finished );

  return bset_index;
};

void sarl_power_set_extend(
  sarl_power_set_t* pset, sarl_unsigned_int length
)
{
  sarl_power_set_t* new_pset = sarl_power_set_new(length);
  sarl_unsigned_int index = 0;

  if ( new_pset ) {
    for(index=0;index<pset->length;++index) {
      if (((sarl_unsigned_int)pset->entry_table[index].ptr)>0x1) {
        sarl_power_set_insert_with_index(
          new_pset, 
          pset->entry_table[index].ptr, 
          pset->entry_table[index].index
        );
      }
    }
    pset->entry_table = new_pset->entry_table;
    pset->occupancy = new_pset->occupancy;
    pset->length = new_pset->length;
  }
  else {
    sarl_out_of_memory_error;
  }
};

sarl_unsigned_int sarl_power_set_get_index(
  sarl_power_set_t *pset, sarl_bit_set_t *bset
) 
{
  if (pset == 0) {
    return 0;
  }

  sarl_unsigned_int hash_value = sarl_bit_set_hash(bset);
  sarl_unsigned_int hash_index = sarl_bit_set_hash(bset);

  do {
    hash_index = hash_value % pset->length;
    switch ( (sarl_unsigned_int)pset->entry_table[hash_index].ptr ) {
    case 0x0:
      return 0;
    case 0x1:
      break;
    default:
      if ( sarl_bit_set_is_eql(bset, pset->entry_table[hash_index].ptr ) ) {
        return pset->entry_table[hash_index].index;
      }
    }
    ++hash_value;
  } while( 1 );
}

sarl_power_set_t* sarl_power_set_remove(
  sarl_power_set_t *pset, sarl_bit_set_t *bset) 
{
  if ( pset == 0 ) {
    return pset;
  }

  sarl_unsigned_int hash_value = sarl_bit_set_hash(bset);
  sarl_unsigned_int hash_index = sarl_bit_set_hash(bset);

  do {
    hash_index = hash_value % pset->length;
    switch ( (sarl_unsigned_int) pset->entry_table[hash_index].ptr ) {
    case 0x0:
      return pset;
      break;
    case 0x1:
      break;
    default:
      if ( sarl_bit_set_is_eql(bset, pset->entry_table[hash_index].ptr ) ) {
        pset->entry_table[hash_index].ptr = (sarl_bit_set_t *)0x1;
        pset->occupancy--;
        return pset;
      }
    }
    ++hash_value;
  } while( 1 );
};

void sarl_power_set_or_equals(sarl_power_set_t *dest, sarl_power_set_t *src)
{
  sarl_unsigned_int i;
  for(i=0;i<src->length;++i) {
    if (sarl_is_table_entry_valid(src->entry_table[i].ptr)) {
      sarl_power_set_insert(dest, src->entry_table[i].ptr);
    }
  }
}

void sarl_power_set_add_extent(sarl_power_set_t *pset, sarl_bit_set_t *bset)
{
  if (pset == 0 || bset == 0) {
    sarl_null_pointer_error;
  }
  else {
    sarl_print_trace("++ Add Extent");
    sarl_bit_set_print_trace(bset);

    sarl_unsigned_int i;
    sarl_power_set_t* new_extents = sarl_power_set_default_new();
    sarl_bit_set_t* tmp_set = 0;

    sarl_power_set_insert(new_extents, bset);
    for(i=0;i<pset->length;++i) {
      if (sarl_is_table_entry_valid(pset->entry_table[i].ptr)) {
        tmp_set = sarl_bit_set_and(pset->entry_table[i].ptr, bset);
        sarl_bit_set_print_trace(pset->entry_table[i].ptr)
        sarl_bit_set_print_trace(bset);
        sarl_bit_set_print_trace(tmp_set);
        sarl_power_set_insert(new_extents, tmp_set);
        sarl_potential_code_leak(tmp_set);
      }
    }
    sarl_power_set_or_equals(pset, new_extents);
    sarl_power_set_print_trace(new_extents);
    sarl_power_set_print_trace(pset);
  }
};

sarl_power_set_t* sarl_power_set_closure(
  sarl_power_set_t *pset, sarl_bit_set_t *object_set)
{
  sarl_unsigned_int i;
  sarl_power_set_t *result = sarl_power_set_new((pset->length*2)+1);
  sarl_power_set_insert(result, object_set);

  for(i=0;i<pset->length;++i) {
    if (sarl_is_table_entry_valid(pset->entry_table[i].ptr)) {
      sarl_power_set_add_extent(result, pset->entry_table[i].ptr);
    }
  }

  return result;
};


void sarl_power_set_print_to_file(sarl_power_set_t* pset, FILE *file)
{
  sarl_unsigned_int i, first = 1;

  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    fprintf(file, "{");
    for(i=0;i<pset->length;++i) {
      if ( sarl_is_table_entry_valid(pset->entry_table[i].ptr) ) {
        if (first) { 
          first = 0;
        }
        else {
          fprintf(file, ", ");
        } 
        sarl_bit_set_print_to_file(pset->entry_table[i].ptr, file);
     }
    }
    fprintf(file, "}");
  }
}

bool sarl_power_set_has_subseteq_in(
  sarl_power_set_t *pset, sarl_bit_set_t *superset)
{
  sarl_bit_set_t *bset;
  SARL_POWER_SET_FOR_EACH(bset, pset);
  if ( sarl_bit_set_is_subseteq(bset, superset) ) {
    return true;
  }
  SARL_POWER_SET_END;
  return false;
};

// this is some strange function. Look at the code
sarl_power_set_t* sarl_power_set_new_from_power_map(sarl_power_map_t *pmap)
{
  sarl_bit_set_t *image;
  sarl_power_set_t *result = sarl_power_set_default_new();
  sarl_unsigned_int index;

  SARL_POWER_MAP_FOR_EACH(index, image, pmap);
    sarl_power_set_insert_with_index(result, image, index);
  SARL_POWER_MAP_END;

  return result;
};
