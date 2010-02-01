#ifndef SARL_CLARIFIED_CONTEXT_H
#define SARL_CLARIFIED_CONTEXT_H

#include <gc/gc.h>
#include "sarl_types.h"
#include "sarl_bit_set.h"
#include "sarl_power_set.h"
#include "sarl_power_map.h"

struct _sarl_clarified_context_t {
  sarl_context_t   *context;
  sarl_int_array_t *object_map;
  sarl_int_array_t *attribute_map;
  sarl_power_map_t *object_comap;
  sarl_power_map_t *attribute_comap;
};

inline sarl_clarified_context_t* sarl_clarified_context_blank_new()
{
  sarl_clarified_context_t *cc = (sarl_clarified_context_t *)
    GC_MALLOC(sizeof(sarl_clarified_context_t));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);
  return cc;
};

inline sarl_clarified_context_t* sarl_clarified_context_new(
  sarl_context_t   *context,
  sarl_int_array_t *object_map,
  sarl_int_array_t *attribute_map,
  sarl_power_map_t *object_comap,
  sarl_power_map_t *attribute_comap
) {
  sarl_clarified_context_t *cc = sarl_clarified_context_blank_new();
  sarl_return_zero_error_if_zero(cc);
  cc->context          = context;
  cc->object_map       = object_map;
  cc->attribute_map    = attribute_map;
  cc->object_comap     = object_comap;
  cc->attribute_comap  = attribute_comap;

  return cc;
}

#endif





