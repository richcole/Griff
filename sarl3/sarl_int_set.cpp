#include "sarl_int_set.h"
#include <stdio.h>

void sarl_int_set_insert(
  sarl_int_set_t *pset, sarl_unsigned_int bset) 
{
  if (pset == 0) {
    sarl_null_pointer_error;
  }
  else if (! SARL_INT_SET_IS_VALID_VALUE(bset)) {
    sarl_invalid_index_error;
  }
  else {

    sarl_unsigned_int hash_value = bset;
    sarl_unsigned_int hash_index = hash_value % pset->length;
    sarl_unsigned_int initial_hash_index = hash_index;
    sarl_unsigned_int finished = 0;
    
    if (pset->occupancy * 4 > pset->length * 3) {
      sarl_int_set_extend(pset, (pset->length*2)+1);
    }
    
    sarl_unsigned_int first_dirty_index = pset->length;
    
    do {
      switch ( (sarl_unsigned_int) pset->table[hash_index] ) {
      case SARL_INT_SET_EMPTY_VALUE:
        if (first_dirty_index == pset->length) {
          pset->table[hash_index] = bset;
          pset->occupancy++;
        }
        else {
          pset->table[first_dirty_index] = bset;
          pset->occupancy++;
        }
        finished = 1;
        break;
      case SARL_INT_SET_DIRTY_VALUE:
        if (first_dirty_index == pset->length) {
          first_dirty_index = hash_index;
        }
        break;
      default:
        if ( bset == pset->table[hash_index] ) { 
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

void sarl_int_set_extend(
  sarl_int_set_t* pset, sarl_unsigned_int length
)
{
  sarl_int_set_t* new_pset = sarl_int_set_new(length);
  sarl_unsigned_int index = 0;

  if ( new_pset ) {
    for(index=0;index<pset->length;++index) {
      if ( SARL_INT_SET_IS_VALID_VALUE(pset->table[index]) ) {
        sarl_int_set_insert(new_pset, pset->table[index]);
      }
    }
    // free(pset->table);
    pset->table = new_pset->table;
    pset->occupancy = new_pset->occupancy;
    pset->length = new_pset->length;
  }
  else {
    sarl_out_of_memory_error;
  }
};

sarl_unsigned_int sarl_int_set_is_member(
  sarl_int_set_t *pset, sarl_unsigned_int bset
) 
{
  if (pset == 0) {
    sarl_null_pointer_error;
    return 0;
  }
  if (! SARL_INT_SET_IS_VALID_VALUE(bset) ) {
    sarl_invalid_index_error;
    return 0;
  }

  sarl_unsigned_int hash_value = bset;
  sarl_unsigned_int hash_index = bset;

  do {
    hash_index = hash_value % pset->length;
    switch ( (sarl_unsigned_int)pset->table[hash_index] ) {
    case SARL_INT_SET_EMPTY_VALUE:
      return 0;
    case SARL_INT_SET_DIRTY_VALUE:
      break;
    default:
      if ( bset == pset->table[hash_index] ) { 
        return true;
      }
    }
    ++hash_value;
  } while( 1 );
}

sarl_int_set_t* sarl_int_set_remove(
  sarl_int_set_t *pset, sarl_unsigned_int bset) 
{
  if ( pset == 0 ) {
    sarl_null_pointer_error;
    return pset;
  }
  if (! SARL_INT_SET_IS_VALID_VALUE(bset) ) {
    sarl_invalid_index_error;
    return pset;
  }


  sarl_unsigned_int hash_value = bset;
  sarl_unsigned_int hash_index = bset;

  do {
    hash_index = hash_value % pset->length;
    switch ( (sarl_unsigned_int) pset->table[hash_index] ) {
    case 0x0:
      return pset;
      break;
    case SARL_INT_SET_DIRTY_VALUE:
      break;
    default:
      if ( bset == pset->table[hash_index] ) { 
        pset->table[hash_index] = (sarl_unsigned_int)SARL_INT_SET_DIRTY_VALUE;
        pset->occupancy--;
        return pset;
      }
    }
    ++hash_value;
  } while( 1 );
};

void sarl_int_set_or_equals(sarl_int_set_t *dest, sarl_int_set_t *src)
{
  if (dest == 0 || src == 0 ) {
    sarl_null_pointer_error;
    return;
  }

  sarl_unsigned_int i;
  for(i=0;i<src->length;++i) {
    if (SARL_INT_SET_IS_VALID_VALUE(src->table[i])) {
      sarl_int_set_insert(dest, src->table[i]);
    }
  }
}

void sarl_int_set_print_to_file(sarl_int_set_t* pset, FILE *file)
{
  sarl_unsigned_int i, first = 1;

  if ( pset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    fprintf(file, "{");
    for(i=0;i<pset->length;++i) {
      if ( SARL_INT_SET_IS_VALID_VALUE(pset->table[i]) ) {
        if (first) { 
          first = 0;
        }
        else {
          fprintf(file, ", ");
        } 
        fprintf(file, "%u", pset->table[i]);
     }
    }
    fprintf(file, "}");
  }
}
