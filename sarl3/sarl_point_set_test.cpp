#include "sarl_point_set.h"

#include "sarl_point_set.h"
#include "sarl_assert.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_point_set_t* pset = sarl_point_set_default_new();

  sarl_point_t p1 = { 10, 10 };
  sarl_point_t p2 = { 20, 20 };
  sarl_point_t p3 = { 10, 20 };

  sarl_point_t p4 = { 30, 10 };
  sarl_point_t p5 = { 20, 30 };
  sarl_point_t p6 = { 20, 10 };

  sarl_point_set_insert(pset, &p1);
  sarl_point_set_insert(pset, &p2);
  sarl_point_set_insert(pset, &p3);

  sarl_test_assert(sarl_point_set_is_member(pset, &p1));
  sarl_test_assert(sarl_point_set_is_member(pset, &p2));
  sarl_test_assert(sarl_point_set_is_member(pset, &p3));

  sarl_test_assert(! sarl_point_set_is_member(pset, &p4));
  sarl_test_assert(! sarl_point_set_is_member(pset, &p5));
  sarl_test_assert(! sarl_point_set_is_member(pset, &p6));
}
