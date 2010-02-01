#include "sarl_int_set.h"
#include "sarl_assert.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_int_set_t* pset = sarl_int_set_new(13);

  sarl_int_set_insert(pset, 1);
  sarl_int_set_insert(pset, 14);
  sarl_int_set_insert(pset, 27);
  sarl_int_set_remove(pset, 14);
  sarl_int_set_insert(pset, 2);
  sarl_int_set_insert(pset, 3);

  sarl_test_assert(sarl_int_set_is_member(pset, 1));
  sarl_test_assert(sarl_int_set_is_member(pset, 2));
  sarl_test_assert(sarl_int_set_is_member(pset, 3));
  sarl_test_assert(! sarl_int_set_is_member(pset, 14));
  sarl_test_assert(sarl_int_set_is_member(pset, 27));
}
