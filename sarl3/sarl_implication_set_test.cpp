#include "sarl_implication_set.h"
#include "sarl_context.h"
#include "sarl_context_formats.h"
#include "sarl_string_table.h"
#include "sarl_power_set.h"

#include "sarl_assert.h"

#include <stdio.h>

int main()
{
  sarl_unsigned_int result;

  fprintf(stdout, "Starting sarl_implcation_set_test...\n");

  sarl_context_t *context = sarl_context_new();
  sarl_string_table_t *objects = sarl_string_table_default_new();
  sarl_string_table_t *attributes = sarl_string_table_default_new();

  FILE *inp = fopen("test/mushrooms.cxt", "r");
  sarl_test_assert(inp != 0);

  result = sarl_context_load_from_cxt(context, objects, attributes, inp);
  sarl_test_assert(result == 1);
  fclose(inp);

  sarl_implication_set_t* imps = sarl_context_psudo_intents(context);

  sarl_bit_set_t *psudo;
  sarl_unsigned_int psudo_index;
  SARL_POWER_SET_FOR_EACH_INDEX(psudo_index, psudo, imps->premises);
    fprintf(stdout, "%u: ", psudo_index);
    sarl_string_table_print_bit_set_to_file(attributes, psudo, stdout);
    fprintf(stdout, " -> ");
    sarl_string_table_print_bit_set_to_file(
      attributes, 
      sarl_power_map_get(imps->conclusions, psudo_index), 
      stdout
    );
    fprintf(stdout, "\n");
  SARL_POWER_SET_END;

  fprintf(stdout, "Finished.\n");
};
