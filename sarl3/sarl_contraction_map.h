#ifndef SARL_CONTRACTION_MAP_H
#define SARL_CONTRACTION_MAP_H

#include "sarl_types.h"
#include "sarl_int_array.h"
#include "sarl_bit_set.h"

// return the cont_map and fill in inv_cont_map if it is non zero
inline sarl_int_array_t *sarl_contraction_map(
  sarl_bit_set_t *s, sarl_int_array_t *inv_cont_map)
{
  sarl_int_array_t *cont_map = sarl_int_array_default_new();
  
  sarl_unsigned_int i, count = 1;
  
  SARL_BIT_SET_FOR_EACH(i, s);
    sarl_int_array_set(cont_map, i, count);
    if ( inv_cont_map ) { sarl_int_array_set(inv_cont_map, count, i); };
    count += 1;
  SARL_BIT_SET_END;

  return cont_map;
};

#endif
