#include "sarl_diagram_examples.h"
#include "sarl_assert.h"

#include <stdio.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_diagram_t *diagram = sarl_diagram_create_b3();
  sarl_test_assert(diagram != 0);
};
