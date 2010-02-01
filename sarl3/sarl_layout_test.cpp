#include "sarl_layout.h"

#include "sarl_power_map.h"
#include "sarl_bit_set.h"
#include "sarl_int_array.h"
#include "sarl_context_examples.h"
#include "sarl_diagram.h"
#include "sarl_lattice.h"
#include "sarl_context_formats.h"
#include "sarl_string_table.h"

#include "sarl_assert.h"

#include <sys/time.h>
#include <time.h>

int test_diagram_layout()
{
  sarl_context_t *context = sarl_context_create_b3();
  sarl_diagram_t *diagram = sarl_diagram_layout_from_context(context);
  sarl_point_t*   p;
  sarl_unsigned_int concept;

  SARL_LATTICE_FOR_EACH(concept, diagram->lattice);
    p = sarl_diagram_get_concept_pos(diagram, concept);
    printf("concept=%u, c=(%d,%d)\n", concept, p->x, p->y);
  SARL_LATTICE_END;
    
  return 1;
}

int test_attribute_ordering()
{
  sarl_context_t* context = sarl_context_new();
  sarl_string_table_t* st_objects = sarl_string_table_default_new();
  sarl_string_table_t* st_attributes = sarl_string_table_default_new();

  FILE *inp = fopen("fig8.cxt", "r");
  sarl_test_assert(inp != 0);

  if ( inp == 0 ) {
    return -1;
  };

  sarl_context_load_from_cxt(context, st_objects, st_attributes, inp);
  
  sarl_int_array_t* ms = sarl_context_order_attributes_by_extent_count(context);

  fprintf(stdout, "Attribute ordering: ");
  sarl_unsigned_int m, first = 1;
  SARL_INT_ARRAY_FOR_EACH(m, ms);
    if ( first ) { first = 0; } else { fprintf(stdout, ", "); };
    fprintf(stdout, "%u", m);
  SARL_INT_ARRAY_END;
  fprintf(stdout, "\n");

  sarl_diagram_layout_from_context(context);

  return 1;
};

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_power_map_t* imp = sarl_power_map_default_new();
  sarl_power_map_insert(imp, 3, 1);  
  sarl_power_map_insert(imp, 2, 1);  
  sarl_power_map_insert(imp, 4, 1);  
  sarl_power_map_insert(imp, 4, 2);  
  sarl_power_map_insert(imp, 4, 3);

  sarl_bit_set_t* M = sarl_bit_set_default_new();
  sarl_unsigned_int m;

  for(m=1;m<=4;++m) {
    sarl_bit_set_set(M, m);
  };

  sarl_int_array_t* ms = sarl_order_attributes_by_implication(imp, M);

  sarl_test_assert(sarl_int_array_count(ms) == 4);
  sarl_test_assert(sarl_int_array_get(ms, 1) == 4);
  sarl_test_assert(sarl_int_array_get(ms, 2) == 2);
  sarl_test_assert(sarl_int_array_get(ms, 3) == 3);
  sarl_test_assert(sarl_int_array_get(ms, 4) == 1);

  sarl_test_assert(test_diagram_layout());

  test_attribute_ordering();
  return 0;
};
