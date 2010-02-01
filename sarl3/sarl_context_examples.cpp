#include "sarl_context_examples.h"
#include "sarl_context.h"

sarl_context_t* sarl_context_create_b3()
{
  sarl_context_t *context = sarl_context_new();
  sarl_context_insert(context, 1, 2);
  sarl_context_insert(context, 1, 3);
  sarl_context_insert(context, 2, 1);
  sarl_context_insert(context, 2, 3);
  sarl_context_insert(context, 3, 1);
  sarl_context_insert(context, 3, 2);

  return context;
};

sarl_context_t* sarl_context_create_boolean(sarl_unsigned_int boolean_size)
{
  sarl_context_t *context = sarl_context_new();
  sarl_unsigned_int i;

  sarl_context_insert_object_range(context, 1, boolean_size);

  for(i=1;i<=boolean_size;++i) {
    sarl_context_insert_object_intent(context, i, context->G);
    sarl_context_remove(context, i, i);
  };

  return context;
};


