#include "sarl_hash_map.h"
#include "sarl_assert.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_hash_map_t* pset = sarl_hash_map_new(13);

  void *p1 = (void *)0x1;
  void *p2 = (void *)0x2;
  void *p3 = (void *)0x3;

  sarl_hash_map_insert(pset, 1, p1);
  sarl_hash_map_insert(pset, 14, p2);
  sarl_hash_map_insert(pset, 27, p3);
  sarl_hash_map_remove(pset, 14);
  sarl_hash_map_insert(pset, 2, p1);
  sarl_hash_map_insert(pset, 3, p2);

  sarl_test_assert(sarl_hash_map_image(pset, 1) == p1);
  sarl_test_assert(sarl_hash_map_image(pset, 2) == p1);
  sarl_test_assert(sarl_hash_map_image(pset, 3) == p2);
  sarl_test_assert(sarl_hash_map_image(pset, 14) == 0);
  sarl_test_assert(sarl_hash_map_image(pset, 27) == p3);
}
