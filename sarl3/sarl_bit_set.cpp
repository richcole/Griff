#include "sarl_bit_set.h"
#include "sarl_int_array.h"

void sarl_bit_set_print_to_file(sarl_bit_set_t *bset, FILE *file) {
  sarl_unsigned_int i, j;
  sarl_unsigned_int first = 1;
  
  if ( bset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    fprintf(file, "{");
    for(i=0;i<bset->length;++i) {
      for(j=0;j<sarl_unsigned_int_num_bits;++j) {
        if (bset->bits[i] & (0x1 << j)) {
          if (first) { 
            fprintf(file, "%u", 1+((i*sarl_unsigned_int_num_bits)+j));
            first = 0;
          }
          else {
            fprintf(file, ", %u", 1+((i*sarl_unsigned_int_num_bits)+j));
          }
        }
      }
    }
    fprintf(file, "}");
  }
};

void sarl_bit_set_assign_from_array(
  sarl_bit_set_t *dst, sarl_int_array_t *arr, sarl_bit_set_t *src)
{
  sarl_unsigned_int i;
  sarl_bit_set_clear(dst);

  SARL_INT_ARRAY_FOR_EACH(i, arr);
    sarl_bit_set_set(dst, sarl_bit_set_nth(src, i));
  SARL_INT_ARRAY_END;
};

void sarl_bit_set_assign_k_from_array_plus_one(
  sarl_bit_set_t *dst, sarl_unsigned_int k, sarl_int_array_t *arr, sarl_bit_set_t *src)
{
  sarl_unsigned_int index, value;
  sarl_bit_set_clear(dst);
  
  for(index=1;index<=arr->length;++index) {
    value = arr->table[index-1];
    sarl_bit_set_set(dst, sarl_bit_set_nth(src, value+1));
    if ( index >= k ) break;
  }
};

sarl_unsigned_int sarl_num_allocated_bit_sets = 0;

void sarl_finalize_bit_set(GC_PTR ptr, GC_PTR data)
{
  --sarl_num_allocated_bit_sets;
};
