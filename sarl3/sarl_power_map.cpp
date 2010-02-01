#include "sarl_power_map.h"

int sarl_bit_set_qsort_compare(const void *a_ptr, const void *b_ptr) {
  sarl_bit_set_t *a = *(sarl_bit_set_t **)a_ptr;
  sarl_bit_set_t *b = *(sarl_bit_set_t **)b_ptr;
  if ( a == b ) { return  0; };
  if ( a == 0 ) { return  1; }; /* null entries at the end of the map */
  if ( b == 0 ) { return -1; }; /* null entries at the end of the map */
  if ( sarl_bit_set_is_leq(a, b) ) {
    if ( sarl_bit_set_is_eql(a,b) ) { 
      return 0; 
    }
    else {
      return -1;
    }
  }
  else {
    return 1;
  }
}

void sarl_power_map_sort(sarl_power_map_t *pmap)
{
  qsort(pmap->table, pmap->length, sizeof(sarl_bit_set_t *), 
    &sarl_bit_set_qsort_compare
  );
};
    
sarl_power_map_t* sarl_power_map_inverse(sarl_power_map_t *pmap)
{
  sarl_unsigned_int index, image_index;
  sarl_bit_set_t* extent;
  sarl_power_map_t *result = sarl_power_map_new(pmap->length);

  SARL_POWER_MAP_FOR_EACH(index, extent, pmap);
    SARL_BIT_SET_FOR_EACH(image_index, extent);
      sarl_power_map_insert(result, image_index, index);
    SARL_BIT_SET_END;
  SARL_POWER_MAP_END;

  return result;
};

void sarl_power_map_fill_nulls(sarl_power_map_t *pmap, sarl_unsigned_int length)
{
  if ( pmap == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    if ( length > pmap->length ) {
      sarl_unsigned_int new_length = pmap->length*2;
      while( length >= new_length ) {
        new_length = ( (new_length) * 3) / 2;
      }
      sarl_power_map_extend(pmap, new_length);
    }
    sarl_unsigned_int i;
    for(i=0;i<length;++i) {
      if ( pmap->table[i] == 0 ) { 
        pmap->table[i] = sarl_bit_set_default_new(); 
      }
    }
  }
}

void sarl_power_map_print_to_file(sarl_power_map_t *pmap, FILE *file)
{
  sarl_unsigned_int  x;
  sarl_bit_set_t    *ys;

  fprintf(file, "Powermap: {\n");
  SARL_POWER_MAP_FOR_EACH(x, ys, pmap);
    fprintf(file, "  %u -> ", x);
    sarl_bit_set_print_to_file(ys, file);
    fprintf(file, "\n");
  SARL_POWER_MAP_END;
  fprintf(file, "}\n");
};
