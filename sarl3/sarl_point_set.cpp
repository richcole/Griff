#include "sarl_point_set.h"

sarl_unsigned_int sarl_point_set_count(sarl_point_set_t *ps)
{
  sarl_unsigned_int count = 0;
  sarl_point_t p;
  SARL_POINT_SET_FOR_EACH(&p, ps);
    ++count;
  SARL_POINT_SET_END;
  return count;
};

