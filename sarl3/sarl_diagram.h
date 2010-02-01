#ifndef SARL_DIAGRAM_H
#define SARL_DIAGRAM_H

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_point.h"

#include <gc/gc.h>
#include <string.h>

struct _sarl_diagram_t {
  sarl_lattice_t     *lattice;
  sarl_unsigned_int   length;
  sarl_point_t       *meet_vectors;
  sarl_point_t       *concept_vectors;
  sarl_point_t       *concept_pos;
};

sarl_diagram_t* sarl_diagram_new(sarl_unsigned_int num_concepts);
sarl_diagram_t* sarl_diagram_new_from_lattice(sarl_lattice_t* lattice);
sarl_diagram_t* sarl_diagram_new_from_context(sarl_context_t* context);
sarl_diagram_t* sarl_diagram_new_empty();

inline sarl_point_t* sarl_diagram_get_concept_pos(
  sarl_diagram_t* diagram, 
  sarl_unsigned_int index
)
{
  if ( index > diagram->length ) {
    sarl_invalid_index_error;
    return 0;
  }
  else {
    return diagram->concept_pos + (index-1);
    return 0;
  }
}

inline void sarl_point_plus_equals(sarl_point_t *dest, sarl_point_t *src)
{
  dest->x += src->x;
  dest->y += src->y;
};

void sarl_diagram_assign_meet_vectors(sarl_diagram_t *diagram);
void sarl_diagram_calculate_positions(sarl_diagram_t *diagram);

void sarl_diagram_move_concept_by(
  sarl_diagram_t *diagram, 
  sarl_unsigned_int concept,
  sarl_signed_int dx,
  sarl_signed_int dy
);

void sarl_diagram_scale_diagram(sarl_diagram_t* diagram, float scale);

#define SARL_DIAGRAM_SCALE 1000

#define SARL_DIAGRAM_COVERING_EACH(pa, pb, diagram) \
  { sarl_unsigned_int __sl_a, __sl_b; sarl_bit_set_t* __sl_lc; \
  SARL_POWER_MAP_FOR_EACH(__sl_a, __sl_lc, (diagram)->lattice->lower_covers); \
    (pa) = sarl_diagram_get_concept_pos(diagram, __sl_a); \
    SARL_BIT_SET_FOR_EACH(__sl_b, __sl_lc); \
      (pb) = sarl_diagram_get_concept_pos(diagram, __sl_b);

#define SARL_DIAGRAM_COVERING_END \
   SARL_BIT_SET_END; SARL_POWER_MAP_END; }

#endif
