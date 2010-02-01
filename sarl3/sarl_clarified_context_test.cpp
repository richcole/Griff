#include "sarl_clarified_context.h"

#include "sarl_context.h"
#include "sarl_power_set.h"
#include "sarl_bit_set.h"
#include "sarl_assert.h"

#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  sarl_test_begin("clairifed_test");

  sarl_context_t *context = sarl_context_new();
  sarl_context_insert(context, 1, 1);
  sarl_context_insert(context, 1, 2);
  sarl_context_insert(context, 1, 4);
  sarl_context_insert(context, 2, 3);
  sarl_context_insert(context, 2, 1);
  sarl_context_insert(context, 3, 1);
  sarl_context_insert(context, 3, 2);
  sarl_context_insert(context, 3, 4);

  sarl_clarified_context_t *cc = sarl_context_clarify(context);

  sarl_test_assert(sarl_bit_set_count(cc->context->G) == 2);
  sarl_test_assert(sarl_bit_set_count(cc->context->M) == 3);

  sarl_test_end;
};

  
