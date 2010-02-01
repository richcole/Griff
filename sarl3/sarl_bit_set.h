#ifndef SARL_BIT_SET_H
#define SARL_BIT_SET_H

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "sarl_types.h"
#include "sarl_exceptions.h"

#include <gc/gc.h>

// #define SARL_TRACE_BIT_SET_MALLOC(x) x
#define SARL_TRACE_BIT_SET_MALLOC(x)

struct _sarl_bit_set_t {
  sarl_unsigned_int  length;
  sarl_unsigned_int  *bits;
};
typedef struct _sarl_bit_set_t sarl_bit_set_t;

extern sarl_unsigned_int sarl_num_allocated_bit_sets;
void sarl_finalize_bit_set(GC_PTR ptr, GC_PTR data);

inline sarl_bit_set_t* sarl_bit_set_new(sarl_unsigned_int length)
{
  sarl_bit_set_t *bset = (sarl_bit_set_t *)
    GC_MALLOC(sizeof(sarl_bit_set_t));

  SARL_TRACE_BIT_SET_MALLOC(
    ++sarl_num_allocated_bit_sets;
    GC_REGISTER_FINALIZER_NO_ORDER(bset, sarl_finalize_bit_set, 0, 0, 0);
  );

  if (bset) {
    bset->bits = (sarl_unsigned_int *)
      GC_MALLOC_ATOMIC(sizeof(sarl_unsigned_int) * length);
    if (bset->bits == 0) {
      // free(bset);
      bset = 0;
      sarl_out_of_memory_error;
    }
    else {
      memset(bset->bits, 0, length * sizeof(sarl_unsigned_int));
      bset->length = length;
    }
  }
  else {
    sarl_out_of_memory_error;
  }

  return bset;
};

inline sarl_bit_set_t* sarl_bit_set_default_new()
{
  return sarl_bit_set_new(1);
};

inline void sarl_bit_set_extend(
  sarl_bit_set_t *bset, sarl_unsigned_int length)
{
  sarl_unsigned_int  *new_bits = (sarl_unsigned_int *)
    GC_MALLOC_ATOMIC(sizeof(sarl_unsigned_int)*length);

  if ( new_bits ) {
    memcpy(new_bits, bset->bits, bset->length * sizeof(sarl_unsigned_int));
    if ( length > bset->length ) {
      memset(new_bits + bset->length, 0, 
        sizeof(sarl_unsigned_int) * (length - bset->length));
    }
    // free(bset->bits);
    bset->bits = new_bits;
    bset->length = length;
  }
  else { sarl_out_of_memory_error; };
};

inline sarl_bit_set_t* sarl_bit_set_and(
  sarl_bit_set_t *src1, sarl_bit_set_t *src2
)
{
  sarl_unsigned_int dest_len = sarl_max_of(src1->length, src2->length);
  sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
  sarl_bit_set_t *dest = sarl_bit_set_new(dest_len);
  sarl_unsigned_int i;

  if (dest) {
    for(i=0;i<min_len;++i) {
      dest->bits[i] = src1->bits[i] & src2->bits[i];
    }
  }

  return dest;
};

inline bool sarl_bit_set_is_disjoint(sarl_bit_set_t *a, sarl_bit_set_t *b)
{
  sarl_unsigned_int i;
  sarl_unsigned_int n = sarl_min_of(a->length, b->length);
  for(i=0;i<n;++i) {
    if (a->bits[i] & b->bits[i]) return false;
  }
  return true;
};

inline sarl_bit_set_t* sarl_bit_set_neg(
  sarl_unsigned_int n, sarl_bit_set_t *src
)
{
  if ( n == 0 ) {
    return sarl_bit_set_new(1);
  }

  sarl_bit_set_t *dst = sarl_bit_set_new(1 + ((n-1) / (sizeof(src->bits[0])*8)));
  sarl_unsigned_int i;

  for(i=0;i<dst->length && i<src->length;++i) {
    dst->bits[i] = ~src->bits[i];
  }
  sarl_unsigned_int rem = 
    (sizeof(dst->bits[0]) * 8) - 
    ((dst->length * sizeof(dst->bits[0]) * 8) - n);

  if ( rem != (sizeof(dst->bits[0])*8) ) {
    dst->bits[dst->length - 1] = (dst->bits[dst->length - 1]) % (0x1 << rem);
  }

  return dst;
};

inline sarl_bit_set_t* sarl_bit_set_minus(
  sarl_bit_set_t *src1, sarl_bit_set_t *src2
)
{
  if ( src1 == 0 || src2 == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }
  else {
    sarl_unsigned_int dest_len = src1->length;
    sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
    sarl_bit_set_t *dest = sarl_bit_set_new(dest_len);
    sarl_unsigned_int i;
    
    if (dest) {
      for(i=0;i<min_len;++i) {
        dest->bits[i] = src1->bits[i] & (~ (src2->bits[i]));
      }
      for(;i<dest_len;++i) {
        dest->bits[i] = src1->bits[i];
      }
    }

    return dest;
  }
};

inline sarl_bit_set_t* sarl_bit_set_sym_diff(
  sarl_bit_set_t *src1, sarl_bit_set_t *src2
)
{
  if ( src1 == 0 || src2 == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }
  else {
    sarl_unsigned_int dest_len = sarl_max_of(src1->length, src2->length);
    sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
    sarl_bit_set_t *dest = sarl_bit_set_new(dest_len);
    sarl_unsigned_int i;
    
    if (dest) {
      for(i=0;i<min_len;++i) {
        dest->bits[i] = (src1->bits[i]) ^ (src2->bits[i]);
      }
      for(;i<src1->length;++i) {
        dest->bits[i] = src1->bits[i];
      }
      for(;i<src2->length;++i) {
        dest->bits[i] = src2->bits[i];
      }
    }

    return dest;
  }
};

inline void sarl_bit_set_or_equals(
  sarl_bit_set_t *dest, sarl_bit_set_t *src
)
{
  sarl_unsigned_int i;
  if ( dest == 0 || src == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    if ( dest->length < src->length ) {
      sarl_bit_set_extend(dest, src->length);
    }
    for(i=0;i<src->length;++i) {
      dest->bits[i] |= src->bits[i];
    }
  }
};

inline void sarl_bit_set_assign_equals(
  sarl_bit_set_t *dest, sarl_bit_set_t *src
)
{
  sarl_unsigned_int i;
  if ( dest == 0 || src == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    if ( dest->length < src->length ) {
      sarl_bit_set_extend(dest, src->length);
    }
    for(i=0;i<src->length;++i) {
      dest->bits[i] = src->bits[i];
    }
    for(;i<dest->length;++i) {
      dest->bits[i] = 0;
    }
  }
};

inline void sarl_bit_set_and_equals(sarl_bit_set_t *bset, sarl_bit_set_t *src)
{
  if ( bset == 0 || src == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int min_len = sarl_min_of(bset->length, src->length);
    sarl_unsigned_int i;
    
    for(i=0;i<min_len;++i) {
      bset->bits[i] &= src->bits[i];
    }
    for(;i<bset->length;++i) {
      bset->bits[i] = 0;
    }
  }
};

inline sarl_bit_set_t* sarl_bit_set_or(
  sarl_bit_set_t *src1, sarl_bit_set_t *src2)
{
  sarl_unsigned_int dest_len = sarl_max_of(src1->length, src2->length);
  sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
  sarl_bit_set_t *dest = sarl_bit_set_new(dest_len);
  sarl_unsigned_int i;

  if (dest) {
    for(i=0;i<min_len;++i) {
      dest->bits[i] = src1->bits[i] | src2->bits[i];
    }
    for (; i < src1->length; ++i) {
      dest->bits[i] = src1->bits[i];
    }
    for (; i < src2->length; ++i) {
      dest->bits[i] = src2->bits[i];
    }
  }

  return dest;
};

inline sarl_unsigned_int sarl_bit_set_is_subseteq(sarl_bit_set_t *src1, sarl_bit_set_t *src2)
{
  sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
  sarl_unsigned_int i;

  for(i=0;i<min_len;++i) {
    if ((src1->bits[i] | src2->bits[i]) != src2->bits[i]) {
      return 0;
    }
  }
  for(;i<src1->length;++i) {
    if (src1->bits[i] != 0) {
      return 0;
    }
  }
  return 1;
};

inline sarl_unsigned_int sarl_bit_set_is_subset(sarl_bit_set_t *src1, sarl_bit_set_t *src2)
{
  sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
  sarl_unsigned_int i;
  sarl_unsigned_int is_equal = 1;

  for(i=0;i<min_len;++i) {
    if ((src1->bits[i] != src2->bits[i])) { is_equal = 0; }
    if ((src1->bits[i] | src2->bits[i]) != src2->bits[i]) {
      return 0;
    }
  }
  for(;i<src1->length;++i) {
    if (src1->bits[i] != 0) {
      return 0;
    }
  }
  for(;i<src2->length;++i) {
    if ( src2->bits[i] != 0 ) {
      return 1;
    }
  }
  return ! is_equal;
};

inline sarl_unsigned_int sarl_bit_set_is_equal(sarl_bit_set_t *src1, sarl_bit_set_t *src2)
{
  sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
  sarl_unsigned_int i;

  for(i=0;i<min_len;++i) {
    if ((src1->bits[i] != src2->bits[i])) { return 0; }
  }
  for(;i<src1->length;++i) {
    if (src1->bits[i] != 0) {
      return 0;
    }
  }
  for(;i<src2->length;++i) {
    if ( src2->bits[i] != 0 ) {
      return 0;
    }
  }
  return 1;
};

inline sarl_unsigned_int sarl_bit_set_is_leq(sarl_bit_set_t *src1, sarl_bit_set_t *src2)
{
  sarl_unsigned_int min_len = sarl_min_of(src1->length, src2->length);
  sarl_unsigned_int i;

  for(i=0;i<min_len;++i) {
    if ((src1->bits[i] != src2->bits[i])) {
      return src1->bits[i] < src2->bits[i];
    }
  }
  for(;i<src1->length;++i) {
    if (src1->bits[i] != 0) {
      return 0;
    }
  }
  return 1;
};

inline sarl_bit_set_t* sarl_bit_set_dup(sarl_bit_set_t *bset)
{
  if ( bset == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }
  else {
    sarl_bit_set_t* result = sarl_bit_set_new(bset->length);
    
    if (result) {
      memcpy(result->bits, bset->bits, sizeof(sarl_unsigned_int)*bset->length);
    }
    else { sarl_out_of_memory_error; };
    
    return result;
  }
};

inline void sarl_bit_set_set(
  sarl_bit_set_t *bset, 
  sarl_unsigned_int index
)
{
  if (bset == 0 ) { 
    sarl_null_pointer_error; 
  }
  else if (index == 0 ) {
    sarl_invalid_index_error;
  }
  else {
    index -= 1;
    if ((index / sarl_unsigned_int_num_bits) >= bset->length) {
      sarl_bit_set_extend(bset,(index / sarl_unsigned_int_num_bits)+1);
      if (bset) {
        bset->bits[index / sarl_unsigned_int_num_bits] |= 
          0x1 << (index % sarl_unsigned_int_num_bits);
      }
    }
    else {
      bset->bits[index / sarl_unsigned_int_num_bits] |= 
        0x1 << (index % sarl_unsigned_int_num_bits);
      bset = bset;
    }
  }
};

inline void sarl_bit_set_clear_all(sarl_bit_set_t* bset)
{
  if ( bset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    memset(bset->bits, 0, sizeof(bset->bits[0]) * bset->length);
  }
}

inline void sarl_bit_set_clear(
  sarl_bit_set_t *bset, 
  sarl_unsigned_int index
)
{
  if (bset == 0) {
    sarl_null_pointer_error;
  }
  else if (index == 0 ) {
    sarl_invalid_index_error;
  }
  else {
    --index;
    if ((index / sarl_unsigned_int_num_bits) >= bset->length) {
      sarl_bit_set_extend(bset,(index/sarl_unsigned_int_num_bits)+1);
      if (bset) {
        bset->bits[index / sarl_unsigned_int_num_bits] &= 
          ~ ( ((sarl_unsigned_int)0x1) << (index % sarl_unsigned_int_num_bits));
      }
    }
    else {
      bset->bits[index / sarl_unsigned_int_num_bits] &= 
        ~ ( ((sarl_unsigned_int)0x1) << (index % sarl_unsigned_int_num_bits));
      bset = bset;
    }
  }
};

inline sarl_unsigned_int 
  sarl_bit_set_is_set(sarl_bit_set_t *bset, sarl_unsigned_int index)
{
  if (index == 0 ) {
    sarl_invalid_index_error;
    return 0;
  }
  else {
    --index;
    if (bset == 0 || (index / sarl_unsigned_int_num_bits) >= bset->length) {
      return 0;
    }
    else {
      return 
        bset->bits[index / sarl_unsigned_int_num_bits] &
        (((sarl_unsigned_int)0x1) << (index % sarl_unsigned_int_num_bits));
    }
  }
};

inline sarl_unsigned_int sarl_bit_set_hash(sarl_bit_set_t *bset)
{
  sarl_unsigned_int i, result = 0;
  for(i=0;i<bset->length;++i) {
    result ^= (bset->bits[i] << i % 5);
  }

  return result;
}

inline sarl_unsigned_int sarl_bit_set_is_eql(
  sarl_bit_set_t *bset1, sarl_bit_set_t *bset2
) 
{
  sarl_unsigned_int index;
  sarl_unsigned_int min_index = sarl_min_of(bset1->length, bset2->length);
  sarl_unsigned_int max_index = sarl_max_of(bset1->length, bset2->length);

  for(index=0;index<min_index;++index) {
    if ( bset1->bits[index] != bset2->bits[index] ) {
      return 0;
    }
  }
  for(;index<max_index;++index) {
    if ( index < bset1->length && bset1->bits[index] != 0 ) {
      return 0;
    }
    if ( index < bset2->length && bset2->bits[index] != 0 ) {
      return 0;
    }
  }
  return 1;
}

inline sarl_unsigned_int sarl_bit_set_is_empty(sarl_bit_set_t *bset)
{
  sarl_unsigned_int index;
  for(index=0;index<bset->length;++index) {
    if ( bset->bits[index] != 0 ) { return 0; }
  }
  return 1;
};

inline void sarl_bit_set_clear(sarl_bit_set_t *bset)
{
  if ( bset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    memset(bset->bits, 0, bset->length * sizeof(bset->bits[0]));
  }
}

inline sarl_unsigned_int sarl_bit_set_count(sarl_bit_set_t *bset)
{
  sarl_unsigned_int count = 0;
  if ( bset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int i, value;
    for(i=0;i<bset->length;++i) {
      value = bset->bits[i];
      while(value != 0) { 
        count += (value & 0x1); 
        value >>= 1;
      };
    }
  }
  return count;
}

inline void sarl_bit_set_populate(sarl_bit_set_t *bset, sarl_unsigned_int n)
{
  sarl_unsigned_int i;
  for(i=1;i<=n;i++) {
    sarl_bit_set_set(bset, i);
  }
};

void sarl_bit_set_print_to_file(sarl_bit_set_t *bset, FILE *file);

inline sarl_unsigned_int 
  sarl_bit_set_next(sarl_bit_set_t *bset, sarl_unsigned_int n)
{
  sarl_unsigned_int i;
  for(i=1;i<=n && sarl_bit_set_is_set(bset, i); ++i);
  if ( i > n ) {
    return 0;
  } 
  else {
    sarl_bit_set_set(bset, i);
    for(--i; i>0; --i) {
      sarl_bit_set_clear(bset, i);
    }
    return 1;
  }
}

#define SARL_BIT_SET_FOR_EACH(index, bset) \
  { \
    sarl_unsigned_int _xx_sbsfe_i; \
    sarl_unsigned_int _xx_sbsfe_j; \
    for(_xx_sbsfe_i=0;_xx_sbsfe_i<bset->length;++_xx_sbsfe_i) \
      for(_xx_sbsfe_j=0;_xx_sbsfe_j<sarl_unsigned_int_num_bits;++_xx_sbsfe_j)  \
        if ( bset->bits[_xx_sbsfe_i] & (0x1 << _xx_sbsfe_j) ) { \
          index = 1 + (_xx_sbsfe_i * sarl_unsigned_int_num_bits) + _xx_sbsfe_j;

#define SARL_BIT_SET_END } }

inline sarl_unsigned_int sarl_bit_set_first(sarl_bit_set_t *bset) {
  if ( bset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int index;
    SARL_BIT_SET_FOR_EACH(index, bset);
    return index;
    SARL_BIT_SET_END;
  }
  return 0;
}

inline sarl_unsigned_int sarl_bit_set_nth(
  sarl_bit_set_t *bset, sarl_unsigned_int nth) 
{
  if ( bset == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_unsigned_int index, count = 0;
    SARL_BIT_SET_FOR_EACH(index, bset);
      ++count; 
      if (count >= nth) return index;
    SARL_BIT_SET_END;
  }
  return 0;
}

inline sarl_unsigned_int sarl_bit_set_last(sarl_bit_set_t *bset)
{
  sarl_unsigned_int last = 0;
  SARL_BIT_SET_FOR_EACH(last, bset);
  SARL_BIT_SET_END;
  return last;
}

inline sarl_bit_set_t* sarl_bit_set_new_from_array(sarl_unsigned_int *ints)
{
  sarl_bit_set_t *bs = sarl_bit_set_default_new();
  while(*ints) {
    sarl_bit_set_set(bs, *ints);
    ++ints;
  }

  return bs;
};

void sarl_bit_set_assign_from_array(
				    sarl_bit_set_t *dst, sarl_int_array_t *arr, sarl_bit_set_t *src);

// this is strange function, look at the code
void sarl_bit_set_assign_k_from_array_plus_one(
				    sarl_bit_set_t *dst, sarl_unsigned_int k, sarl_int_array_t *arr, sarl_bit_set_t *src);

#endif
