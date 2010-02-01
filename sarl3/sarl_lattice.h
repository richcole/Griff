#ifndef SARL_LATTICE_H
#define SARL_LATTICE_H

#include "sarl_context.h"
#include "sarl_power_map.h"

struct _sarl_lattice_t {
  sarl_context_t* context;
  sarl_bit_set_t* concepts;
  sarl_unsigned_int top;
  sarl_unsigned_int bottom;
  sarl_unsigned_int num_concepts;
  sarl_power_map_t* concept_extents;
  sarl_power_map_t* concept_intents;
  sarl_power_map_t* concept_object_cont;
  sarl_power_map_t* concept_attribute_cont;
  sarl_power_map_t* upper_covers;
  sarl_power_map_t* lower_covers;
  sarl_power_map_t* upset;
  sarl_power_map_t* downset;
  sarl_bit_set_t*   meet_irreducibles;
  sarl_int_array_t* concept_uprank;
  sarl_int_array_t* concept_downrank;
};
typedef struct _sarl_lattice_t sarl_lattice_t;


sarl_lattice_t* sarl_lattice_new(sarl_context_t *context);
inline sarl_unsigned_int sarl_lattice_count(sarl_lattice_t *lattice) {
  return lattice->num_concepts;
};

sarl_context_t* sarl_lattice_reduced_context(sarl_lattice_t *lattice);

sarl_context_t* sarl_lattice_distributive_closure_context(
  sarl_lattice_t *lattice);

sarl_power_set_t* sarl_lattice_intents_as_power_set(sarl_lattice_t *lattice);

void sarl_lattice_print_to_file(sarl_lattice_t* lattice, FILE *file);

#define SARL_LATTICE_FOR_EACH(concept_index, lattice) \
  for(concept_index=1;concept_index<=lattice->num_concepts;++concept_index) {
#define SARL_LATTICE_END }

#define SARL_LATTICE_BOTTOM_UP_FOR_EACH(concept_index, lattice) \
  for(concept_index=1;concept_index<=lattice->num_concepts;++concept_index) {
#define SARL_LATTICE_BOTTOM_UP_END }

#define SARL_LATTICE_TOP_DOWN_FOR_EACH(concept_index, lattice) \
  for(concept_index=lattice->num_concepts;concept_index>0;--concept_index) {
#define SARL_LATTICE_TOP_DOWN_END }

#endif
