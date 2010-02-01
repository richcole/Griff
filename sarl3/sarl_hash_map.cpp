#include "sarl_hash_map.h"
#include <stdio.h>

void sarl_hash_map_insert(
  sarl_hash_map_t *pset, sarl_unsigned_int key,
  void *data) 
{
  if (pset == 0) {
    sarl_null_pointer_error;
  }
  else if (! SARL_HASH_MAP_IS_VALID_VALUE(key)) {
    sarl_invalid_index_error;
  }
  else {

    sarl_unsigned_int hash_value = key;
    sarl_unsigned_int hash_index = hash_value % pset->length;
    sarl_unsigned_int initial_hash_index = hash_index;
    sarl_unsigned_int finished = 0;
    
    if (pset->occupancy * 4 > pset->length * 3) {
      sarl_hash_map_extend(pset, (pset->length*2)+1);
    }
    
    sarl_unsigned_int first_dirty_index = pset->length;
    
    do {
      switch ( (sarl_unsigned_int) pset->key_table[hash_index] ) {
      case SARL_HASH_MAP_EMPTY_VALUE:
        if (first_dirty_index == pset->length) {
          pset->key_table[hash_index] = key;
          pset->data_table[hash_index] = data;
          pset->occupancy++;
        }
        else {
          pset->key_table[first_dirty_index] = key;
          pset->data_table[first_dirty_index] = data;
          pset->occupancy++;
        }
        finished = 1;
        break;
      case SARL_HASH_MAP_DIRTY_VALUE:
        if (first_dirty_index == pset->length) {
          first_dirty_index = hash_index;
        }
        break;
      default:
        if ( key == pset->key_table[hash_index] ) { 
          pset->data_table[hash_index] = data;
          finished = true;
        }
      }
      hash_index = (hash_index + 1) % pset->length;
      if ( hash_index == initial_hash_index ) {
        sarl_internal_error;
        break;
      }
    } while( ! finished );
  }
};

void sarl_hash_map_extend(
  sarl_hash_map_t* pset, sarl_unsigned_int length
)
{
  sarl_hash_map_t* new_pset = sarl_hash_map_new(length);
  sarl_unsigned_int index = 0;

  if ( new_pset ) {
    for(index=0;index<pset->length;++index) {
      if ( SARL_HASH_MAP_IS_VALID_VALUE(pset->key_table[index]) ) {
        sarl_hash_map_insert(
          new_pset, pset->key_table[index], pset->data_table[index]
        );
      }
    }
    pset->key_table = new_pset->key_table;
    pset->data_table = new_pset->data_table;
    pset->occupancy = new_pset->occupancy;
    pset->length = new_pset->length;
  }
  else {
    sarl_out_of_memory_error;
  }
};

void* sarl_hash_map_image(
  sarl_hash_map_t *pset, sarl_unsigned_int key
) 
{
  if (pset == 0) {
    sarl_null_pointer_error;
    return 0;
  }
  if (! SARL_HASH_MAP_IS_VALID_VALUE(key) ) {
    sarl_invalid_index_error;
    return 0;
  }

  sarl_unsigned_int hash_value = key;
  sarl_unsigned_int hash_index = key;

  do {
    hash_index = hash_value % pset->length;
    switch ( (sarl_unsigned_int)pset->key_table[hash_index] ) {
    case SARL_HASH_MAP_EMPTY_VALUE:
      return 0;
    case SARL_HASH_MAP_DIRTY_VALUE:
      break;
    default:
      if ( key == pset->key_table[hash_index] ) { 
        return pset->data_table[hash_index];
      }
    }
    ++hash_value;
  } while( 1 );
}

sarl_hash_map_t* sarl_hash_map_remove(
  sarl_hash_map_t *pset, sarl_unsigned_int key) 
{
  if ( pset == 0 ) {
    sarl_null_pointer_error;
    return pset;
  }
  if (! SARL_HASH_MAP_IS_VALID_VALUE(key) ) {
    sarl_invalid_index_error;
    return pset;
  }


  sarl_unsigned_int hash_value = key;
  sarl_unsigned_int hash_index = key;

  do {
    hash_index = hash_value % pset->length;
    switch ( (sarl_unsigned_int) pset->key_table[hash_index] ) {
    case 0x0:
      return pset;
      break;
    case SARL_HASH_MAP_DIRTY_VALUE:
      break;
    default:
      if ( key == pset->key_table[hash_index] ) { 
        pset->key_table[hash_index] = 
          (sarl_unsigned_int) SARL_HASH_MAP_DIRTY_VALUE;
        pset->data_table[hash_index] = 0;
        pset->occupancy--;
        return pset;
      }
    }
    ++hash_value;
  } while( 1 );
};

void sarl_hash_map_or_equals(sarl_hash_map_t *dest, sarl_hash_map_t *src)
{
  if (dest == 0 || src == 0 ) {
    sarl_null_pointer_error;
    return;
  }

  sarl_unsigned_int i;
  for(i=0;i<src->length;++i) {
    if (SARL_HASH_MAP_IS_VALID_VALUE(src->key_table[i])) {
      sarl_hash_map_insert(dest, src->key_table[i], src->data_table[i]);
    }
  }
}

void sarl_hash_map_print_to_file(sarl_hash_map_t* pset, FILE *file)
{
  sarl_unsigned_int i, first = 1;

  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    fprintf(file, "{");
    for(i=0;i<pset->length;++i) {
      if ( SARL_HASH_MAP_IS_VALID_VALUE(pset->key_table[i]) ) {
        if (first) { 
          first = 0;
        }
        else {
          fprintf(file, ", ");
        } 
        fprintf(file, "%u -> %p", pset->key_table[i], pset->data_table[i]);
     }
    }
    fprintf(file, "}");
  }
}
