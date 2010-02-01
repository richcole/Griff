#include "sarl_diagram.h"
#include "sarl_lattice.h"
#include "sarl_context.h"
#include "sarl_context_examples.h"

#include "sarl_assert.h"

#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_context_t *context = sarl_context_create_b3();
  sarl_lattice_t* lattice = sarl_lattice_new(context);
  sarl_diagram_t* diagram = sarl_diagram_new_from_lattice(lattice);

  sarl_test_assert( diagram != 0 );
};
