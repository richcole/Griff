#include "sarl_context_examples.h"
#include "sarl_assert.h"

#include <stdio.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_context_t *context = sarl_context_create_b3();
  sarl_test_assert(context != 0);
};
