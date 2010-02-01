#include "sarl_lattice.h"
#include "sarl_bit_set.h"
#include "sarl_int_array.h"
#include "sarl_lattice_map.h"

#include "sarl_context.h"

void sarl_lattice_calculate_downrank(sarl_lattice_t *lattice)
{
  sarl_unsigned_int c, lc, max_rank;
  sarl_bit_set_t *lower_covers;

  lattice->concept_downrank = sarl_int_array_new(lattice->num_concepts);
  SARL_LATTICE_BOTTOM_UP_FOR_EACH(c, lattice);
    lower_covers = sarl_power_map_get(lattice->lower_covers, c);
    max_rank = 0;
    SARL_BIT_SET_FOR_EACH(lc, lower_covers);
      max_rank = sarl_max_of(sarl_int_array_get(lattice->concept_downrank, lc), max_rank);
    SARL_BIT_SET_END;
    sarl_int_array_set(lattice->concept_downrank, c, max_rank+1);
  SARL_LATTICE_BOTTOM_UP_END;
};

void sarl_lattice_calculate_uprank(sarl_lattice_t *lattice)
{
  sarl_unsigned_int c, uc, max_rank;
  sarl_bit_set_t *upper_covers;

  lattice->concept_uprank = sarl_int_array_new(lattice->num_concepts);
  SARL_LATTICE_TOP_DOWN_FOR_EACH(c, lattice);
    upper_covers = sarl_power_map_get(lattice->upper_covers, c);
    max_rank = 0;
    SARL_BIT_SET_FOR_EACH(uc, upper_covers);
      max_rank = sarl_max_of(
        sarl_int_array_get(lattice->concept_uprank, uc), max_rank
      );
    SARL_BIT_SET_END;
    sarl_int_array_set(lattice->concept_uprank, c, max_rank+1);
  SARL_LATTICE_TOP_DOWN_END;
};

sarl_lattice_t* sarl_lattice_new(sarl_context_t *context)
{
  sarl_lattice_t* lattice = (sarl_lattice_t *)
    GC_MALLOC(sizeof(sarl_lattice_t));
  lattice->context = sarl_context_dup(context);

  sarl_power_set_t *attr_extents = sarl_power_set_default_new();
  sarl_unsigned_int m;

  // create a power set of extents from the context
  SARL_BIT_SET_FOR_EACH(m, lattice->context->M);
    sarl_power_set_insert(
      attr_extents,
      sarl_context_attribute_extent(lattice->context, m)
      );
  SARL_BIT_SET_END;

  // take the closure
  sarl_power_set_t *extents = 
    sarl_power_set_closure(attr_extents, lattice->context->G);

  fprintf(stdout, "Number of attributes=%u, Number of concepts=%u\n", 
    sarl_bit_set_count(lattice->context->M), 
    sarl_power_set_count(extents)
  );

  lattice->num_concepts = sarl_power_set_count(extents);
  
  // insert the closure into the extents power map
  sarl_unsigned_int index = 0;
  sarl_bit_set_t *extent;
  lattice->concept_extents = sarl_power_map_new(lattice->num_concepts);
  SARL_POWER_SET_FOR_EACH(extent, extents);
    ++index;
    sarl_power_map_set(lattice->concept_extents, index, extent);
  SARL_POWER_SET_END;

  // sort the power map
  sarl_power_map_sort(lattice->concept_extents);

  // calcuate the intents
  lattice->concept_intents = 
    sarl_power_map_new(lattice->num_concepts);
  SARL_POWER_MAP_FOR_EACH(index, extent, lattice->concept_extents);  
    sarl_power_map_set(
      lattice->concept_intents, index, 
      sarl_context_intent(lattice->context, extent)
    );
  SARL_POWER_MAP_END;

  // calculate the order relation
  sarl_bit_set_t* child_extent;
  sarl_bit_set_t* anc_extent;
  sarl_unsigned_int child_index, anc_index;
  lattice->upset = 
    sarl_power_map_new(lattice->num_concepts);
  SARL_POWER_MAP_FOR_EACH(child_index, child_extent, lattice->concept_extents);
    sarl_power_map_set(lattice->upset, child_index, sarl_bit_set_default_new());
    SARL_POWER_MAP_FOR_EACH(anc_index, anc_extent, lattice->concept_extents);
    if ( sarl_bit_set_is_subset(child_extent, anc_extent) ) {
      sarl_power_map_insert(lattice->upset, child_index, anc_index);
    }
    SARL_POWER_MAP_END;
  SARL_POWER_MAP_END;

  // calculate the covering relation
  sarl_bit_set_t* child_upset;
  sarl_unsigned_int parent_index;
  lattice->upper_covers =
    sarl_power_map_new(lattice->num_concepts);
  SARL_POWER_MAP_FOR_EACH(child_index, child_upset, lattice->upset);
    sarl_bit_set_t* neg_upper_covers = 
      sarl_bit_set_default_new();
    SARL_BIT_SET_FOR_EACH(parent_index, child_upset);
      sarl_bit_set_or_equals(
        neg_upper_covers, sarl_power_map_get(lattice->upset, parent_index)
      );
    SARL_BIT_SET_END;
    sarl_bit_set_t* upper_covers =
      sarl_bit_set_minus(child_upset, neg_upper_covers);
    sarl_power_map_set(lattice->upper_covers, child_index, upper_covers);
  SARL_POWER_MAP_END;

  // invert the upset and the upper covers relation
  lattice->lower_covers = sarl_power_map_inverse(lattice->upper_covers);
  lattice->downset = sarl_power_map_inverse(lattice->upset);

  sarl_power_map_fill_nulls(lattice->upper_covers, lattice->num_concepts);
  sarl_power_map_fill_nulls(lattice->lower_covers, lattice->num_concepts);
  sarl_power_map_fill_nulls(lattice->upset, lattice->num_concepts);
  sarl_power_map_fill_nulls(lattice->downset, lattice->num_concepts);

  // calculate the object contingents
  sarl_bit_set_t *parent_lower_covers;
  lattice->concept_object_cont = 
    sarl_power_map_new(lattice->num_concepts);
  SARL_POWER_MAP_FOR_EACH(
    parent_index, parent_lower_covers, lattice->lower_covers
  );
    sarl_bit_set_t* neg_object_contingent = sarl_bit_set_default_new();
    SARL_BIT_SET_FOR_EACH(child_index, parent_lower_covers);
      sarl_bit_set_or_equals(
        neg_object_contingent, 
        sarl_power_map_get(lattice->concept_extents, child_index)
      );
    SARL_BIT_SET_END;
    sarl_bit_set_t* object_contingent =
      sarl_bit_set_minus(
        sarl_power_map_get(lattice->concept_extents, parent_index), 
        neg_object_contingent
      );
    sarl_power_map_set(
      lattice->concept_object_cont, parent_index, object_contingent
    );
  SARL_POWER_MAP_END;

  // calculate the attribute contingents
  sarl_bit_set_t *child_upper_covers;
  lattice->concept_attribute_cont = 
    sarl_power_map_new(lattice->concept_intents->length);
  SARL_POWER_MAP_FOR_EACH(
    child_index, child_upper_covers, lattice->upper_covers
   );
    sarl_bit_set_t* neg_attribute_contingent = sarl_bit_set_default_new();
    SARL_BIT_SET_FOR_EACH(parent_index, child_upper_covers);
      sarl_bit_set_or_equals(
        neg_attribute_contingent, 
        sarl_power_map_get(lattice->concept_intents, parent_index)
      );
    SARL_BIT_SET_END;
    sarl_bit_set_t* attribute_contingent =
      sarl_bit_set_minus(
        sarl_power_map_get(lattice->concept_intents, child_index), 
        neg_attribute_contingent
      );
    sarl_power_map_set(
      lattice->concept_attribute_cont, child_index, attribute_contingent
    );
  SARL_POWER_MAP_END;

  sarl_power_map_fill_nulls(lattice->concept_attribute_cont, lattice->num_concepts);
  sarl_power_map_fill_nulls(lattice->concept_object_cont, lattice->num_concepts);

  // calculate meet irreducibles
  lattice->meet_irreducibles = sarl_bit_set_default_new();
  SARL_POWER_MAP_FOR_EACH(
    child_index, child_upper_covers, lattice->upper_covers
  );
    if (sarl_bit_set_count(child_upper_covers) == 1) {
      sarl_bit_set_set(lattice->meet_irreducibles, child_index);
    };
  SARL_POWER_MAP_END;

  // calculate ranks
  sarl_lattice_calculate_uprank(lattice);
  sarl_lattice_calculate_downrank(lattice);

  return lattice;
}

sarl_context_t* sarl_lattice_reduced_context(sarl_lattice_t *lattice)
{
  sarl_context_t *result = sarl_context_new();
  sarl_unsigned_int g, m;
  sarl_bit_set_t *lower_covers, *intent;

  SARL_POWER_MAP_FOR_EACH(g, lower_covers, lattice->lower_covers);
    if (sarl_bit_set_count(lower_covers) == 1) {
      sarl_context_insert_object(result, g);
      intent = sarl_bit_set_dup(sarl_power_map_get(lattice->upset, g));
      sarl_bit_set_set(intent, g);
      sarl_bit_set_and_equals(intent, lattice->meet_irreducibles);
      SARL_BIT_SET_FOR_EACH(m, intent);
        sarl_context_insert(result, g,m);
      SARL_BIT_SET_END;
    }
  SARL_POWER_MAP_END;

  SARL_BIT_SET_FOR_EACH(m, lattice->meet_irreducibles);
    sarl_context_insert_attribute(result, m);
  SARL_BIT_SET_END;

  return result;
};

sarl_context_t* sarl_lattice_distributive_closure_context(
  sarl_lattice_t *lattice)
{
  sarl_bit_set_t *irr = lattice->meet_irreducibles;
  sarl_context_t *dc_context = sarl_context_new();
  sarl_unsigned_int g, m;

  SARL_BIT_SET_FOR_EACH(g, irr);
  SARL_BIT_SET_FOR_EACH(m, irr);
    sarl_bit_set_t *g_upper = sarl_power_map_get(lattice->upset, g);
    if ( ! sarl_bit_set_is_set(g_upper, m) ) {
      sarl_context_insert(dc_context, g, m);
    }
  SARL_BIT_SET_END;
  SARL_BIT_SET_END;

  return dc_context;
};

sarl_lattice_map_t* sarl_lattice_distributive_closure_embedding(
  sarl_lattice_t *lattice)
{
  sarl_unsigned_int c, image;
  sarl_bit_set_t *c_upset;

  sarl_lattice_t       *dc_lattice = 
    sarl_lattice_new(sarl_lattice_distributive_closure_context(lattice));

  sarl_power_set_t     *dc_intents = 
    sarl_lattice_intents_as_power_set(dc_lattice);

  sarl_lattice_map_t   *result = 
    sarl_lattice_map_new(lattice, dc_lattice);

  SARL_POWER_MAP_FOR_EACH(c, c_upset, lattice->upset);
    sarl_bit_set_t *rep = 
      sarl_bit_set_minus(lattice->meet_irreducibles, c_upset);
    sarl_bit_set_clear(rep, c);
    image = sarl_power_set_get_index(dc_intents, rep);
    sarl_int_array_set(result->map, c, image);
  SARL_POWER_MAP_END;

  result->comap = sarl_int_array_inverse(result->map);

  return result;
};

void sarl_lattice_print_to_file(sarl_lattice_t* lattice, FILE *file)
{
  sarl_unsigned_int c;
  fprintf(file, "Lattice: {\n");
  SARL_LATTICE_FOR_EACH(c, lattice);
    fprintf(file, "  concept=%u\n", c);
    fprintf(file, "    lower_covers="); 
    sarl_bit_set_print_to_file(
      sarl_power_map_get(lattice->lower_covers, c), file);
    fprintf(file, "\n");
  SARL_LATTICE_END;
  fprintf(file, "  meet_irreducibles=");
  sarl_bit_set_print_to_file(lattice->meet_irreducibles, file);
  fprintf(file, "\n");
  fprintf(file, "}\n");
};

sarl_power_set_t* sarl_lattice_intents_as_power_set(sarl_lattice_t *lattice)
{
  return sarl_power_set_new_from_power_map(lattice->concept_intents);
};

sarl_power_set_t* sarl_lattice_etxents_as_power_set(sarl_lattice_t *lattice)
{
  return sarl_power_set_new_from_power_map(lattice->concept_extents);
};

