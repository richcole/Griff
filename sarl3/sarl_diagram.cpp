#include "sarl_diagram.h"
#include "sarl_lattice.h"

#include <math.h>
#include <gc/gc.h>

sarl_diagram_t* sarl_diagram_new(sarl_unsigned_int num_concepts)
{
  sarl_diagram_t* diagram = (sarl_diagram_t*)
    GC_MALLOC(sizeof(sarl_diagram_t));

  diagram->lattice = 0;
  diagram->length = num_concepts;

  diagram->meet_vectors = (sarl_point_t *)
    GC_MALLOC_ATOMIC(sizeof(sarl_point_t)*diagram->length);

  diagram->concept_vectors = (sarl_point_t *)
    GC_MALLOC_ATOMIC(sizeof(sarl_point_t)*diagram->length);

  diagram->concept_pos = (sarl_point_t *)
    GC_MALLOC_ATOMIC(sizeof(sarl_point_t)*diagram->length);

  if ( 
      diagram->meet_vectors == 0 ||
      diagram->concept_vectors == 0 ||
      diagram->concept_pos == 0
      ) {
    sarl_out_of_memory_error;
  }

  memset(diagram->meet_vectors,    0,   sizeof(sarl_point_t)*diagram->length);
  memset(diagram->concept_vectors, 0,   sizeof(sarl_point_t)*diagram->length);
  memset(diagram->concept_pos,     0,   sizeof(sarl_point_t)*diagram->length);

  return diagram;
};

sarl_diagram_t* sarl_diagram_new_from_lattice(sarl_lattice_t* lattice)
{
  sarl_diagram_t* diagram = sarl_diagram_new(lattice->num_concepts);
  diagram->lattice = lattice;

  sarl_unsigned_int concept_index;
  sarl_bit_set_t* concept_upset;
  SARL_POWER_MAP_FOR_EACH(concept_index, concept_upset, lattice->upset);
    sarl_bit_set_t *rep = 
      sarl_bit_set_and(concept_upset, lattice->meet_irreducibles);
    sarl_unsigned_int rep_index;
    SARL_BIT_SET_FOR_EACH(rep_index, rep);
      sarl_point_plus_equals(
        diagram->concept_pos + (concept_index-1), 
        diagram->meet_vectors + (rep_index-1)
      );
    SARL_BIT_SET_END;
    sarl_point_plus_equals(
      diagram->concept_pos + (concept_index - 1), 
      diagram->meet_vectors + (concept_index - 1)
    );
    sarl_point_plus_equals(
      diagram->concept_pos + (concept_index - 1), 
      diagram->concept_vectors + (concept_index - 1)
    );
  SARL_POWER_MAP_END;

  memset(diagram->concept_vectors, 0, sizeof(sarl_point_t)*diagram->length);
  sarl_diagram_assign_meet_vectors(diagram);
  sarl_diagram_calculate_positions(diagram);
  return diagram;
}

void sarl_diagram_assign_meet_vectors(sarl_diagram_t *diagram)
{
  memset(diagram->meet_vectors, 0, sizeof(sarl_point_t)*diagram->length);
  sarl_unsigned_int meet_index, i;
  sarl_signed_int meet_count = -1;
  double theta, length, sign;
  SARL_BIT_SET_FOR_EACH(meet_index, diagram->lattice->meet_irreducibles);
  if ( meet_count == -1 ) {
    i = 0;
    length = SARL_DIAGRAM_SCALE * (((double)i)+1)*sqrt(((double)i)+2.0);
    diagram->meet_vectors[meet_index-1].x = 0;
    diagram->meet_vectors[meet_index-1].y = (int)(length / sqrt(2));
  }
  else {
    i = meet_count / 2;
    sign = (meet_count % 2) ? -1 : 1;
    theta = sarl_pi_on_2 - (sarl_pi_on_2 / (sqrt(3.0+i)));
    length = SARL_DIAGRAM_SCALE * (((double)i)+1)*sqrt(((double)i)+2.0);
    diagram->meet_vectors[meet_index-1].x = (int)(length * sign * sin(theta));
    diagram->meet_vectors[meet_index-1].y = (int)(length * cos(theta));
  }
  ++meet_count;
  SARL_BIT_SET_END;
};

void sarl_diagram_print_pos_array(sarl_point_t* points, sarl_unsigned_int length)
{
  sarl_unsigned_int i;
  for(i=0;i<length;++i) {
    fprintf(stdout, "%d: %d %d\n", 
      i+1, points[i].x, points[i].y
    );
  }
};

void sarl_diagram_calculate_positions(sarl_diagram_t *diagram)
{
  // run the additivity calculation
  memset(diagram->concept_pos, 0, sizeof(sarl_point_t)*diagram->length);
  sarl_bit_set_t* concept_upset;
  sarl_unsigned_int concept_index;
  SARL_POWER_MAP_FOR_EACH(
    concept_index, concept_upset, diagram->lattice->upset
  );
    sarl_bit_set_t *rep = sarl_bit_set_and(
      concept_upset, 
      diagram->lattice->meet_irreducibles
    );
    sarl_unsigned_int rep_index;
    SARL_BIT_SET_FOR_EACH(rep_index, rep);
      sarl_point_plus_equals(
        diagram->concept_pos + (concept_index-1), 
        diagram->meet_vectors + (rep_index-1)
      );
    SARL_BIT_SET_END;
    sarl_point_plus_equals(
      diagram->concept_pos + (concept_index-1),
      diagram->meet_vectors + (concept_index-1)
    );
    sarl_point_plus_equals(
      diagram->concept_pos + (concept_index-1), 
      diagram->concept_vectors + (concept_index-1)
    );
  SARL_POWER_MAP_END;
  // sarl_diagram_print_pos_array(diagram->concept_pos, diagram->length);
  // sarl_diagram_print_pos_array(diagram->meet_vectors, diagram->length);
};

sarl_diagram_t* sarl_diagram_new_from_context(sarl_context_t* context)
{
  if ( context == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }

  sarl_lattice_t *lattice = sarl_lattice_new(context);

  if ( lattice == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }

  return sarl_diagram_new_from_lattice(lattice);
};

void sarl_diagram_move_concept_by(
  sarl_diagram_t *diagram, 
  sarl_unsigned_int concept,
  sarl_signed_int dx,
  sarl_signed_int dy
  )
{
  // if the concept a meet irreducible
  if ( sarl_bit_set_is_set(diagram->lattice->meet_irreducibles, concept) ) {
    sarl_point_t *point = diagram->meet_vectors + (concept-1);
    point->x += dx;
    point->y += dy;
  }
  else {
    // FIXME
  }
  sarl_diagram_calculate_positions(diagram);
};

void sarl_diagram_scale_diagram(sarl_diagram_t* diagram, float scale)
{
  sarl_unsigned_int i;
  for(i=0;i<diagram->length;++i) {
    diagram->concept_pos[i].x = (sarl_signed_int)(diagram->concept_pos[i].x * scale);
    diagram->meet_vectors[i].x = (sarl_signed_int)(diagram->meet_vectors[i].x * scale);
    diagram->concept_vectors[i].x = (sarl_signed_int)(diagram->concept_vectors[i].x * scale);
    diagram->concept_pos[i].y = (sarl_signed_int)(diagram->concept_pos[i].y * scale);
    diagram->meet_vectors[i].y = (sarl_signed_int)(diagram->meet_vectors[i].y * scale);
    diagram->concept_vectors[i].y = (sarl_signed_int)(diagram->concept_vectors[i].y * scale);
  }
}
