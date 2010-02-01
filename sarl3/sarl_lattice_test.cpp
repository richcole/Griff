#include "sarl_lattice.h"
#include "sarl_context.h"
#include "sarl_context_examples.h"

#include "sarl_assert.h"

#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_context_t *context = sarl_context_create_boolean(6);

  sarl_lattice_t* lattice = sarl_lattice_new(context);

  sarl_unsigned_int concept_index;
  sarl_bit_set_t* extent;

  SARL_POWER_MAP_FOR_EACH(concept_index, extent, lattice->concept_extents);
    fprintf(stdout, "%u: extent=", concept_index);
    sarl_bit_set_print_to_file(extent, stdout);
    fprintf(stdout, ", upset=");
    sarl_bit_set_print_to_file(sarl_power_map_get(lattice->upset, concept_index), stdout);
    fprintf(stdout, ", upper_covers=");
    sarl_bit_set_print_to_file(sarl_power_map_get(lattice->upper_covers, concept_index), stdout);
    fprintf(stdout, ", lower_covers=");
    sarl_bit_set_print_to_file(sarl_power_map_get(lattice->lower_covers, concept_index), stdout);
    fprintf(stdout, "\n");
  SARL_POWER_MAP_END;

  fprintf(stdout, "meet_irreducibles=");
  sarl_bit_set_print_to_file(lattice->meet_irreducibles, stdout);
  fprintf(stdout, "\n");

  sarl_unsigned_int boolean_size = 10;
  context = sarl_context_create_boolean(boolean_size);
  fprintf(stdout, "intent=");
  sarl_bit_set_print_to_file(sarl_context_object_intent(context, 1), stdout);
  fprintf(stdout, "\n");
  fprintf(stdout, "intent=");
  sarl_bit_set_print_to_file(sarl_context_object_intent(context, 2), stdout);
  fprintf(stdout, "\n");
  
  timeval before, after;

  gettimeofday(&before, 0);
  lattice = sarl_lattice_new(context);
  gettimeofday(&after, 0);

  if ( before.tv_usec > after.tv_usec ) {
    after.tv_sec -= 1;
    after.tv_usec = after.tv_usec + (1000000 - before.tv_usec);
    after.tv_sec -= before.tv_sec;
  }
  else {
    after.tv_sec -= before.tv_sec;
    after.tv_usec -= before.tv_usec;
  }
  fprintf(stdout, "Lattice Creation Time: %2d.%06d\n", after.tv_sec, after.tv_usec);

  sarl_unsigned_int expected_size = (((unsigned int)0x1) << boolean_size);
  fprintf(stdout, "Lattice count=%u, expected=%u\n", 
    sarl_lattice_count(lattice), expected_size
  );
  sarl_test_assert( sarl_lattice_count(lattice) == expected_size);

  // test that the object contingents are correct
  sarl_bit_set_t *lower_covers;
  SARL_POWER_MAP_FOR_EACH(concept_index, lower_covers, lattice->lower_covers);
  sarl_bit_set_t *ob_cont = 
    sarl_power_map_get(lattice->concept_object_cont, concept_index);
  sarl_test_assert(
    (sarl_bit_set_count(lower_covers) == 1) == 
    (sarl_bit_set_count(ob_cont)      == 1)
  );
  sarl_test_assert(
    (sarl_bit_set_count(ob_cont) == 1) ||
    (sarl_bit_set_count(ob_cont) == 0)
  );
  SARL_POWER_MAP_END;

  fprintf(stdout, "Finished.\n");
};
