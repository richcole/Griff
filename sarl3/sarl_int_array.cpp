#include "sarl_int_array.h"
#include "sarl_power_map.h"

sarl_power_map_t *sarl_int_array_comapping(sarl_int_array_t *cmap)
{
  sarl_unsigned_int g, m;
  sarl_power_map_t *pmap = sarl_power_map_default_new();

  SARL_INT_ARRAY_FOR_EACH_INDEX(g, m, cmap);
    sarl_power_map_insert(pmap, m, g);
  SARL_INT_ARRAY_END;

  return pmap;
};

sarl_int_array_t *sarl_int_array_inverse(sarl_int_array_t *cmap)
{
  sarl_unsigned_int i, j;
  sarl_int_array_t *result = sarl_int_array_new(cmap->length);

  SARL_INT_ARRAY_FOR_EACH_INDEX(i, j, cmap);
    sarl_int_array_set(result, j, i);
  SARL_INT_ARRAY_END;

  return result;
};


sarl_unsigned_int sarl_num_allocated_int_arrays = 0;

void sarl_finalize_int_array(GC_PTR ptr, GC_PTR data)
{
  --sarl_num_allocated_int_arrays;
};
