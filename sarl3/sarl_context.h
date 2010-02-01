#ifndef SARL_CONTEXT_H
#define SARL_CONTEXT_H

#include <gc/gc.h>
#include "sarl_types.h"
#include "sarl_bit_set.h"
#include "sarl_power_set.h"
#include "sarl_power_map.h"

struct _sarl_context_t {
  sarl_bit_set_t *G;
  sarl_bit_set_t *M;
  sarl_power_map_t *intent_map;
  sarl_power_map_t *extent_map;
};


inline sarl_context_t* sarl_context_new() {
  sarl_context_t *context = (sarl_context_t *)
    GC_MALLOC(sizeof(sarl_context_t));

  sarl_return_zero_error_if_zero(context);

  context->G = sarl_bit_set_default_new();
  context->M = sarl_bit_set_default_new();
  context->intent_map = sarl_power_map_default_new();
  context->extent_map = sarl_power_map_default_new();
  return context;
}

inline void sarl_context_insert(
  sarl_context_t* context, sarl_unsigned_int g, sarl_unsigned_int m) 
{
  if ( context == 0 ) {
    sarl_null_pointer_error;
  }
  if ( g == 0 || m == 0 ) {
    sarl_invalid_index_error;
  }
  sarl_bit_set_set(context->G, g);
  sarl_bit_set_set(context->M, m);
  sarl_power_map_insert(context->intent_map, g, m);
  sarl_power_map_insert(context->extent_map, m, g);
};

inline void sarl_context_remove(
  sarl_context_t* context, sarl_unsigned_int g, sarl_unsigned_int m) 
{
  if ( context == 0 ) {
    sarl_null_pointer_error;
  }
  if ( g == 0 || m == 0 ) {
    sarl_invalid_index_error;
  }
  sarl_power_map_remove(context->intent_map, g, m);
  sarl_power_map_remove(context->extent_map, m, g);
};

inline void sarl_context_insert_object(sarl_context_t* context, sarl_unsigned_int g)
{
  sarl_bit_set_set(context->G, g);
};

inline void sarl_context_insert_attribute(sarl_context_t* context, sarl_unsigned_int m)
{
  sarl_bit_set_set(context->M, m);
};

inline void sarl_context_remove_object(
  sarl_context_t* context, sarl_unsigned_int g)
{
  sarl_bit_set_t* intent = sarl_power_map_get(context->intent_map, g);
  sarl_unsigned_int m;
  SARL_BIT_SET_FOR_EACH(m, intent);
    sarl_bit_set_clear(
      sarl_power_map_get(context->extent_map, m),
      g
    );
  SARL_BIT_SET_END;

  sarl_bit_set_clear(context->G, g);
  sarl_power_map_remove_domain(context->intent_map, g);
};

inline void sarl_context_remove_attribute(
  sarl_context_t* context, sarl_unsigned_int m)
{
  sarl_bit_set_t* extent = sarl_power_map_get(context->extent_map, m);
  sarl_unsigned_int g;
  if ( extent ) {
    SARL_BIT_SET_FOR_EACH(g, extent);
      sarl_bit_set_clear(
        sarl_power_map_get(context->intent_map, g),
        m
      );
    SARL_BIT_SET_END;
  }
  sarl_bit_set_clear(context->M, m);
  sarl_power_map_remove_domain(context->extent_map, m);
};

inline void sarl_context_insert_object_range(sarl_context_t* context, sarl_unsigned_int g1, sarl_unsigned_int g2)
{
  sarl_unsigned_int i;
  for(i=g1;i<=g2;++i) {
    sarl_bit_set_set(context->G, i);
  }
};

inline void sarl_context_insert_attribute_range(
 sarl_context_t* context, sarl_unsigned_int m1, sarl_unsigned_int m2)
{
  sarl_unsigned_int i;
  for(i=m1;i<=m2;++i) {
    sarl_bit_set_set(context->M, i);
  }
};

inline sarl_bit_set_t* sarl_context_object_intent(
  sarl_context_t* context, sarl_unsigned_int g)
{
  sarl_bit_set_t *object_intent = 
    sarl_power_map_get(context->intent_map, g);
  if ( object_intent == 0 ) {
    object_intent = sarl_bit_set_default_new();
    sarl_power_map_set(context->intent_map, g, object_intent);
  }
  return object_intent;
};

inline sarl_bit_set_t* sarl_context_attribute_extent(
  sarl_context_t* context, sarl_unsigned_int m)
{
  sarl_bit_set_t * attribute_extent = 
    sarl_power_map_get(context->extent_map, m);
  if ( attribute_extent == 0 ) {
    attribute_extent = sarl_bit_set_default_new();
    sarl_power_map_set(context->extent_map, m, attribute_extent);
  }
  return attribute_extent;
};

inline sarl_bit_set_t* sarl_context_intent(
  sarl_context_t* context, sarl_bit_set_t* A
)
{
  sarl_bit_set_t *result = sarl_bit_set_dup(context->M);
  sarl_unsigned_int index;
  SARL_BIT_SET_FOR_EACH(index, A);
    sarl_bit_set_and_equals(result, 
      sarl_context_object_intent(context, index));
  SARL_BIT_SET_END;
  return result;
}

inline sarl_bit_set_t* sarl_context_extent(
  sarl_context_t* context, sarl_bit_set_t* B
)
{
  sarl_bit_set_t *result = sarl_bit_set_dup(context->G);
  sarl_unsigned_int index;
  SARL_BIT_SET_FOR_EACH(index, B);
    sarl_bit_set_and_equals(result, 
      sarl_context_attribute_extent(context, index));
  SARL_BIT_SET_END;
  return result;
}

inline sarl_bit_set_t* sarl_context_attribute_closure(sarl_context_t *context, sarl_bit_set_t *B)
{
  return sarl_context_intent(context, sarl_context_extent(context, B));
};

inline sarl_bit_set_t* sarl_context_object_closure(sarl_context_t *context, sarl_bit_set_t *A)
{
  return sarl_context_extent(context, sarl_context_intent(context, A));
};

inline sarl_context_t* sarl_context_dup(sarl_context_t* context)
{
  sarl_context_t *dup_context = (sarl_context_t *)
    GC_MALLOC(sizeof(sarl_context_t));

  sarl_return_zero_error_if_zero(dup_context);

  dup_context->G = sarl_bit_set_dup(context->G);
  dup_context->M = sarl_bit_set_dup(context->M);
  dup_context->intent_map = sarl_power_map_dup(context->intent_map);
  dup_context->extent_map = sarl_power_map_dup(context->extent_map);
  return dup_context;
};
  
void sarl_context_insert_object_intent(
  sarl_context_t* context, sarl_unsigned_int i, sarl_bit_set_t* intent
);

sarl_context_t* sarl_context_attribute_subcontext(
  sarl_context_t *context, sarl_bit_set_t *M
);

sarl_context_t* sarl_context_object_subcontext(
  sarl_context_t *context, sarl_bit_set_t *G
);

sarl_power_map_t* sarl_context_object_imps(sarl_context_t *context);
sarl_power_map_t* sarl_context_attribute_imps(sarl_context_t *context);

sarl_bit_set_t* sarl_context_top_intent(sarl_context_t *context);

sarl_int_array_t* sarl_context_object_clarify(sarl_context_t *context);
sarl_int_array_t* sarl_context_attribute_clarify(sarl_context_t *context);

// return true if intent is not a superset of any object intent
sarl_unsigned_int 
  sarl_context_is_maximal_intent(
    sarl_context_t *context, sarl_bit_set_t *intent
  );

void sarl_context_print_to_file(sarl_context_t *context, FILE *file);

sarl_int_array_t* sarl_context_power_map_clarify(
  sarl_bit_set_t *G, sarl_power_map_t *intent_map
);

sarl_clarified_context_t* sarl_context_clarify(sarl_context_t *context);

#define SARL_CONTEXT_FOR_EACH(g, m, context) \
  { sarl_bit_set_t *__scfe_g_intent = 0; \
  SARL_POWER_MAP_FOR_EACH(g, __scfe_g_intent, context->intent_map) \
    SARL_BIT_SET_FOR_EACH(m, __scfe_g_intent);

#define SARL_CONTEXT_END SARL_BIT_SET_END; SARL_POWER_MAP_END; }

#endif
