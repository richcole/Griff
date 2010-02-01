#include "sarl_context.h"
#include "sarl_context_formats.h"
#include "sarl_context_examples.h"

#include "sarl_string_table.h"
#include "sarl_assert.h"

#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  sarl_context_t *context = sarl_context_create_b3();
  sarl_string_table_t *gs = sarl_string_table_default_new();
  sarl_string_table_t *ms = sarl_string_table_default_new();

  sarl_string_table_populate(gs, 
    sarl_bit_set_first(context->G), sarl_bit_set_last(context->G));
  sarl_string_table_populate(ms, 
    sarl_bit_set_first(context->G), sarl_bit_set_last(context->G));

  FILE *outp = fopen("b3.cxt", "w");
  sarl_context_print_to_cxt(context, gs, ms, outp);
  fclose(outp);
};


