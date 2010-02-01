#include "sarl_array.h"

sarl_unsigned_int sarl_num_allocated_arrays = 0;

void sarl_finalize_array(GC_PTR ptr, GC_PTR data)
{
  --sarl_num_allocated_arrays;
};
