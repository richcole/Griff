#include "sarl_context.h"
#include "sarl_int_array.h"
#include "sarl_clarified_context.h"

void sarl_context_insert_object_intent(
  sarl_context_t* context, sarl_unsigned_int g, sarl_bit_set_t* intent)
{
  sarl_bit_set_or_equals(context->M, intent);
  sarl_bit_set_or_equals(sarl_context_object_intent(context, g), intent);
  sarl_unsigned_int m;
  SARL_BIT_SET_FOR_EACH(m, intent);
    sarl_power_map_insert(context->extent_map, m, g);
  SARL_BIT_SET_END;
};

sarl_context_t* sarl_context_attribute_subcontext(sarl_context_t *context, sarl_bit_set_t *M)
{
  sarl_context_t    *result = sarl_context_new();
  sarl_bit_set_t    *extent;
  sarl_unsigned_int g, m;

  SARL_BIT_SET_FOR_EACH(m, M);
    extent = sarl_context_attribute_extent(context, m);
    SARL_BIT_SET_FOR_EACH(g, extent);
      sarl_context_insert(result, g, m);
    SARL_BIT_SET_END;
  SARL_BIT_SET_END;

  return result;
};

sarl_context_t* sarl_context_object_subcontext(sarl_context_t *context, sarl_bit_set_t *G)
{
  sarl_context_t    *result = sarl_context_new();
  sarl_bit_set_t    *intent;
  sarl_unsigned_int g, m;

  SARL_BIT_SET_FOR_EACH(g, G);
    intent = sarl_context_object_intent(context, g);
    SARL_BIT_SET_FOR_EACH(m, intent);
      sarl_context_insert(result, g, m);
    SARL_BIT_SET_END;
  SARL_BIT_SET_END;

  return result;
};

sarl_power_map_t* sarl_context_attribute_imps(sarl_context_t *context)
{
  sarl_unsigned_int m, n;
  sarl_bit_set_t* m_extent;
  sarl_bit_set_t* n_extent;
  sarl_power_map_t* result = sarl_power_map_default_new();

  SARL_BIT_SET_FOR_EACH(m, context->M);
    m_extent = sarl_context_attribute_extent(context, m);
    SARL_BIT_SET_FOR_EACH(n, context->M);
      if ( n != m ) {
        n_extent = sarl_context_attribute_extent(context, n);
        if ( sarl_bit_set_is_subseteq(m_extent, n_extent) ) {
          sarl_power_map_insert(result, m, n);
        };
      }
    SARL_BIT_SET_END;
  SARL_BIT_SET_END;

  return result;
};

sarl_power_map_t* sarl_context_object_imps(sarl_context_t *context)
{
  sarl_unsigned_int g, h;
  sarl_bit_set_t* g_intent;
  sarl_bit_set_t* h_intent;
  sarl_power_map_t* result = sarl_power_map_default_new();

  SARL_BIT_SET_FOR_EACH(g, context->G);
    g_intent = sarl_context_object_intent(context, g);
    SARL_BIT_SET_FOR_EACH(h, context->G);
      if ( g != h ) {
        h_intent = sarl_context_object_intent(context, h);
        if ( sarl_bit_set_is_subseteq(g_intent, h_intent) ) {
          sarl_power_map_insert(result, g, h);
        };
      }
    SARL_BIT_SET_END;
  SARL_BIT_SET_END;

  return result;
};

void sarl_context_print_to_file(sarl_context_t *context, FILE *file)
{
  sarl_unsigned_int g, m;

  fprintf(file, "Context {\n");
  fprintf(file, "  objects=");
  sarl_bit_set_print_to_file(context->G, file);
  fprintf(file, "\n");
  fprintf(file, "  attributes=");
  sarl_bit_set_print_to_file(context->M, file);
  fprintf(file, "\n");
  SARL_BIT_SET_FOR_EACH(g, context->G);
    fprintf(file, "  object=%u, intent=", g);
    sarl_bit_set_print_to_file(
      sarl_context_object_intent(context, g), file);
    fprintf(file, "\n");
  SARL_BIT_SET_END;
  SARL_BIT_SET_FOR_EACH(m, context->M);
    fprintf(file, "  attribute=%u, extent=", m);
    sarl_bit_set_print_to_file(
      sarl_context_attribute_extent(context, m), file);
    fprintf(file, "\n");
  SARL_BIT_SET_END;
  fprintf(file, "}\n");
};

sarl_int_array_t* sarl_context_power_map_clarify(
  sarl_bit_set_t *G, sarl_power_map_t *intent_map
)
{
  sarl_power_set_t  *pset = sarl_power_set_default_new();
  sarl_int_array_t  *cmap = sarl_int_array_default_new();
  sarl_unsigned_int  g;

  SARL_BIT_SET_FOR_EACH(g, G);
    sarl_int_array_set(
      cmap, g,
      sarl_power_set_insert(pset, sarl_power_map_get(intent_map, g))
    );
  SARL_BIT_SET_END;

  return cmap;
};

sarl_int_array_t* sarl_context_object_clarify(sarl_context_t *context)
{
  sarl_return_zero_error_if_zero_3(context, context->G, context->M);
  return sarl_context_power_map_clarify(context->G, context->intent_map);
};

sarl_int_array_t* sarl_context_attribute_clarify(sarl_context_t *context)
{
  sarl_return_zero_error_if_zero_3(context, context->G, context->M);
  return sarl_context_power_map_clarify(context->M, context->extent_map);
};

sarl_clarified_context_t* sarl_context_clarify(sarl_context_t *context)
{
  sarl_clarified_context_t *cc = sarl_clarified_context_blank_new();
  sarl_unsigned_int g, m;
  
  cc->context          = sarl_context_new();
  cc->object_map       = sarl_context_object_clarify(context);
  cc->attribute_map    = sarl_context_attribute_clarify(context);
  cc->object_comap     = sarl_int_array_comapping(cc->object_map);
  cc->attribute_comap  = sarl_int_array_comapping(cc->attribute_map);

  SARL_CONTEXT_FOR_EACH(g, m, context);
    sarl_context_insert(
      cc->context,
      sarl_int_array_get(cc->object_map, g), 
      sarl_int_array_get(cc->attribute_map, m)
    );
  SARL_CONTEXT_END;

  return cc;
};

sarl_bit_set_t* sarl_context_top_intent(sarl_context_t *context)
{
  sarl_bit_set_t *bset = sarl_bit_set_dup(context->M);
  sarl_unsigned_int g;

  SARL_BIT_SET_FOR_EACH(g, context->G);
    sarl_bit_set_and_equals(bset, sarl_context_object_intent(context, g));
  SARL_BIT_SET_END;

  return bset;
};

// return true if intent is not a superset of any object intent
sarl_unsigned_int 
  sarl_context_is_maximal_intent(
    sarl_context_t *context, sarl_bit_set_t *intent
  )
{
  sarl_unsigned_int g;

  SARL_BIT_SET_FOR_EACH(g, context->G);
    if (
      sarl_bit_set_is_subseteq(
        sarl_context_object_intent(context, g), intent
      )
    ) {
      return 0;
    };
  SARL_BIT_SET_END;

  return 1;
};
